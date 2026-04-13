#pragma GCC optimize ("Os")

#include "msx_host_internal.h"

#include <cstdlib>
#include <cstring>

#include "esp_heap_caps.h"

extern "C" unsigned int InitAudio(unsigned int Rate, unsigned int)
{
    using namespace msx;

    if (!g_host.audioBuf) {
        g_host.audioBuf = (sample**)std::calloc(kAudioBufCount, sizeof(sample*));
    }
    if (!g_host.audioBuf) return 0;

    auto cfg = M5Cardputer.Speaker.config();
    cfg.sample_rate      = Rate;
    cfg.stereo           = false;
    cfg.dma_buf_len      = 512;
    cfg.dma_buf_count    = 8;
    cfg.task_priority    = 4;
    cfg.task_pinned_core = 0;
    M5Cardputer.Speaker.config(cfg);

    if (!M5Cardputer.Speaker.isRunning()) {
        M5Cardputer.Speaker.begin();
    }

    M5Cardputer.Speaker.setVolume(60);
    M5Cardputer.Speaker.stop(kAudioChannel);

    for (int i = 0; i < kAudioBufCount; ++i) {
        if (!g_host.audioBuf[i]) {
            g_host.audioBuf[i] = (sample*)heap_caps_malloc(
                kAudioChunk * sizeof(sample),
                MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT
            );
        }
    }

    g_host.audioBufIndex = 0;
    g_host.pendingUsec = 0;
    return Rate;
}

extern "C" void TrashAudio(void)
{
    M5Cardputer.Speaker.stop(msx::kAudioChannel);

    if (msx::g_host.audioBuf) {
        for (int i = 0; i < msx::kAudioBufCount; ++i) {
            std::free(msx::g_host.audioBuf[i]);
            msx::g_host.audioBuf[i] = nullptr;
        }
        std::free(msx::g_host.audioBuf);
        msx::g_host.audioBuf = nullptr;
    }
}

extern "C" unsigned int GetFreeAudio(void)
{
    const size_t queued = M5Cardputer.Speaker.isPlaying(msx::kAudioChannel);
    if (queued == 0) return msx::kAudioChunk * 2;
    if (queued == 1) return msx::kAudioChunk;
    return 0;
}

extern "C" unsigned int WriteAudio(sample* data, unsigned int length)
{
    using namespace msx;

    if (!data || !length || length > (unsigned)kAudioChunk || !g_host.audioBuf) return 0;

    if (M5Cardputer.Speaker.isPlaying(kAudioChannel) > 1) {
        return 0;
    }

    sample* dst = g_host.audioBuf[g_host.audioBufIndex];
    if (!dst) return 0;

    std::memcpy(dst, data, length * sizeof(sample));
    g_host.audioBufIndex = (g_host.audioBufIndex + 1) % kAudioBufCount;

    M5Cardputer.Speaker.playRaw(
        dst,
        length,
        (uint32_t)kAudioRate,
        false,
        1,
        kAudioChannel,
        false
    );

    return length;
}

extern "C" void PlayAllSound(int uSec)
{
    using namespace msx;

    g_host.pendingUsec += uSec;
    while (g_host.pendingUsec >= (uint64_t)kAudioChunk * 1000000 / kAudioRate) {
        RenderAndPlayAudio(kAudioChunk);
        g_host.pendingUsec -= (uint64_t)kAudioChunk * 1000000 / kAudioRate;
    }
}
