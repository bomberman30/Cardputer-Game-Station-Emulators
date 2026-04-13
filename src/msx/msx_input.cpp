#pragma GCC optimize ("Os")

#include "msx_host_internal.h"

#include <cstring>

#include "share/input.h"

namespace {

static void msx_set_printable_key(char ch)
{
    const unsigned char code = static_cast<unsigned char>(ch);
    if (code < (sizeof(Keys) / sizeof(Keys[0]))) {
        KBD_SET(code);
    }
}

static void msx_apply_keyboard_state(const Keyboard_Class::KeysState& ks)
{
    if (ks.shift) KBD_SET(KBD_SHIFT);
    if (ks.ctrl)  KBD_SET(KBD_CONTROL);
    if (ks.tab)   KBD_SET(KBD_TAB);
    if (ks.enter) KBD_SET(KBD_ENTER);
    if (ks.del)   KBD_SET(KBD_BS);
    if (ks.space) KBD_SET(KBD_SPACE);

    for (char ch : ks.word) {
        msx_set_printable_key(ch);
    }

    // FN+SHIFT remains a global escape hatch to quit the current game.
    if (ks.fn && ks.shift) KBD_SET(KBD_ESCAPE);
}

// Build the full keyboard matrix + joystick mask from current hardware state.
// Returns a BTN_* bitmask for the joystick callback.
static unsigned int poll_input()
{
    M5Cardputer.update();
    const Keyboard_Class::KeysState ks = M5Cardputer.Keyboard.keysState();

    share::checkCommonInput(ks);

    // Screen / zoom toggle
    if (M5Cardputer.Keyboard.isChange() &&
        M5Cardputer.Keyboard.isKeyPressed(CARDPUTER_SCREEN_TOGGLE)) {
        if (!msx::g_host.fullscreen) {
            msx::g_host.fullscreen  = true;
            msx::g_host.zoomPercent = 100;
        } else {
            msx::g_host.zoomPercent += 10;
            if (msx::g_host.zoomPercent > 150) {
                msx::g_host.zoomPercent = 100;
                msx::g_host.fullscreen  = false;
            }
        }
    }

    // Fine zoom
    if (ks.fn && M5Cardputer.Keyboard.isKeyPressed(CARDPUTER_ZOOM_PLUS)) {
        if (!msx::g_host.fullscreen) msx::g_host.fullscreen = true;
        if (msx::g_host.zoomPercent < 150) msx::g_host.zoomPercent++;
    }
    if (ks.fn && M5Cardputer.Keyboard.isKeyPressed(CARDPUTER_ZOOM_MINUS)) {
        if (!msx::g_host.fullscreen) msx::g_host.fullscreen = true;
        if (msx::g_host.zoomPercent > 100) msx::g_host.zoomPercent--;
    }

    bool hasPrintable = false;
    for (char ch : ks.word) {
        if (ch) {
            hasPrintable = true;
            break;
        }
    }

    const bool keyboardMode = ks.fn &&
        (hasPrintable || ks.shift || ks.ctrl || ks.tab || ks.enter || ks.del || ks.space);

    // --- Collect direction + button state ---------------------------------
    unsigned int joy = 0;

    if (!keyboardMode) {
        if (M5Cardputer.Keyboard.isKeyPressed(CARDPUTER_LEFT_1)  ||
            M5Cardputer.Keyboard.isKeyPressed(CARDPUTER_LEFT_2))   joy |= BTN_LEFT;
        if (M5Cardputer.Keyboard.isKeyPressed(CARDPUTER_RIGHT_1) ||
            M5Cardputer.Keyboard.isKeyPressed(CARDPUTER_RIGHT_2))  joy |= BTN_RIGHT;
        if (M5Cardputer.Keyboard.isKeyPressed(CARDPUTER_UP_1)    ||
            M5Cardputer.Keyboard.isKeyPressed(CARDPUTER_UP_2))     joy |= BTN_UP;
        if (M5Cardputer.Keyboard.isKeyPressed(CARDPUTER_DOWN_1)  ||
            M5Cardputer.Keyboard.isKeyPressed(CARDPUTER_DOWN_2)  ||
            M5Cardputer.Keyboard.isKeyPressed(CARDPUTER_DOWN_3))   joy |= BTN_DOWN;

        if (M5Cardputer.Keyboard.isKeyPressed(CARDPUTER_BTN_A_1) ||
            M5Cardputer.Keyboard.isKeyPressed(CARDPUTER_BTN_A_2))   joy |= BTN_FIREA;
        if (M5Cardputer.Keyboard.isKeyPressed(CARDPUTER_BTN_B))     joy |= BTN_FIREB;
        if (M5Cardputer.Keyboard.isKeyPressed('p'))               joy |= BTN_FIRER;
        if (M5Cardputer.Keyboard.isKeyPressed('o'))               joy |= BTN_FIREL;
        if (M5Cardputer.Keyboard.isKeyPressed('i'))               joy |= BTN_FIRE;
        if (M5Cardputer.Keyboard.isKeyPressed(CARDPUTER_BTN_START))  joy |= BTN_START;
        if (M5Cardputer.Keyboard.isKeyPressed(CARDPUTER_BTN_SELECT)) joy |= BTN_SELECT;
    }

    // I2C gamepad (M5Stack JoyV2 / Joystick v1.1)
    if (share::hasI2cPad()) {
        const uint32_t pad = share::pollI2cPad();
        if (pad & share::PAD_LEFT)   joy |= BTN_LEFT;
        if (pad & share::PAD_RIGHT)  joy |= BTN_RIGHT;
        if (pad & share::PAD_UP)     joy |= BTN_UP;
        if (pad & share::PAD_DOWN)   joy |= BTN_DOWN;
        if (pad & share::PAD_A)      joy |= BTN_FIREA;
        if (pad & share::PAD_B)      joy |= BTN_FIREB;
        if (pad & share::PAD_START)  joy |= BTN_START;
        if (pad & share::PAD_SELECT) joy |= BTN_SELECT;
    }

    // --- Update fMSX keyboard matrix --
    std::memset((void*)KeyState, 0xFF, sizeof(KeyState));

    if (joy & BTN_LEFT)                    KBD_SET(KBD_LEFT);
    if (joy & BTN_RIGHT)                   KBD_SET(KBD_RIGHT);
    if (joy & BTN_UP)                      KBD_SET(KBD_UP);
    if (joy & BTN_DOWN)                    KBD_SET(KBD_DOWN);
    if (joy & (BTN_FIREA | BTN_START))     KBD_SET(KBD_SPACE);
    if (joy & (BTN_FIREB | BTN_SELECT))    KBD_SET(KBD_ENTER);

    if (keyboardMode) {
        msx_apply_keyboard_state(ks);
    } else if (ks.shift) {
        // Keep shifted harmonized shortcuts like FN+SHIFT out of gameplay
        // but allow plain SHIFT to reach games that inspect it directly
        KBD_SET(KBD_SHIFT);
    }

    return joy;
}

} // namespace

extern "C" unsigned int Joystick(void)
{
    const unsigned int joy = poll_input();
    unsigned int out = 0;

    if (joy & BTN_LEFT)                     out |= JST_LEFT;
    if (joy & BTN_RIGHT)                    out |= JST_RIGHT;
    if (joy & BTN_UP)                       out |= JST_UP;
    if (joy & BTN_DOWN)                     out |= JST_DOWN;
    if (joy & (BTN_FIREA | BTN_START))      out |= JST_FIREA;
    if (joy & (BTN_FIREB | BTN_SELECT))     out |= JST_FIREB;

    return out;
}

extern "C" void Keyboard(void)
{
    poll_input();
}

extern "C" unsigned int Mouse(byte)
{
    return 0;
}
