#pragma GCC optimize ("Ofast")

#include "gx4000_input.h"
#include "gx4000_display.h"
#include "share/input.h"
#include <M5Cardputer.h>
#include <string.h>

extern "C" {
#include "arnold/cpc.h"
}

// ─────────────────────────────────────────────────────────────────────────────
// Key layout (aligned with the shared Cardputer mapping used by other cores):
//
//   D-Pad  : shared directional keys
//   Fire 1 : shared A buttons (J / L)
//   Fire 2 : shared B button (K)
//   Quit   : G0 held ≥ 1 s
//
// CPC Joystick bit mask (active HIGH = pressed):
//   bit 0 = UP          bit 1 = DOWN
//   bit 2 = LEFT         bit 3 = RIGHT
//   bit 4 = FIRE1        bit 5 = FIRE2
// ─────────────────────────────────────────────────────────────────────────────

volatile uint8_t gx4000_joy_state = 0;

static uint32_t s_go_press_ms = 0;  // millis() when G0 first seen pressed
static uint32_t s_lastPollMs = 0;
static bool s_lastQuit = false;
static constexpr uint32_t kPollPeriodMs = 8;

static void gx4000_set_cpc_joy_key(int key_id, bool pressed)
{
    if (pressed) {
        CPC_SetKey(key_id);
    } else {
        CPC_ClearKey(key_id);
    }
}

void gx4000_input_init(void)
{
    gx4000_joy_state = 0;
    s_go_press_ms = 0;
    s_lastPollMs = 0;
    s_lastQuit = false;
}

bool gx4000_input_poll(void)
{
    if (share::shouldPollInput() == false) {
        return s_lastQuit;
    }

    uint32_t now = (uint32_t)millis();
    if (s_lastPollMs != 0 && (now - s_lastPollMs) < kPollPeriodMs) {
        return s_lastQuit;
    }
    s_lastPollMs = now;

    M5Cardputer.update();
    auto &kb = M5Cardputer.Keyboard;
    const Keyboard_Class::KeysState ks = kb.keysState();

    share::checkCommonInput(ks);

    // Cycle screen mode (square/full + zoom steps) with '\\'
    if (kb.isChange() && kb.isKeyPressed(CARDPUTER_SCREEN_TOGGLE)) {
        gx4000_display_cycle_view();
        gx4000_joy_state = 0;
        return false;
    }

    // Fine zoom with FN + directional shortcuts
    if (ks.fn && kb.isKeyPressed(CARDPUTER_ZOOM_PLUS)) {
        gx4000_display_zoom_in();
        gx4000_joy_state = 0;
        return false;
    }

    if (ks.fn && kb.isKeyPressed(CARDPUTER_ZOOM_MINUS)) {
        gx4000_display_zoom_out();
        gx4000_joy_state = 0;
        return false;
    }

    uint8_t joy = 0;

    // D-pad
    if (kb.isKeyPressed(CARDPUTER_UP_1) ||
        kb.isKeyPressed(CARDPUTER_UP_2))        joy |= (1 << 0);  // UP
    if (kb.isKeyPressed(CARDPUTER_DOWN_1) ||
        kb.isKeyPressed(CARDPUTER_DOWN_2) ||
        kb.isKeyPressed(CARDPUTER_DOWN_3))      joy |= (1 << 1);  // DOWN
    if (kb.isKeyPressed(CARDPUTER_LEFT_1) ||
        kb.isKeyPressed(CARDPUTER_LEFT_2))      joy |= (1 << 2);  // LEFT
    if (kb.isKeyPressed(CARDPUTER_RIGHT_1) ||
        kb.isKeyPressed(CARDPUTER_RIGHT_2))     joy |= (1 << 3);  // RIGHT

    // Fire buttons
    if (kb.isKeyPressed(CARDPUTER_BTN_A_1) ||
        kb.isKeyPressed(CARDPUTER_BTN_A_2))     joy |= (1 << 4);  // FIRE1
    if (kb.isKeyPressed(CARDPUTER_BTN_B))       joy |= (1 << 5);  // FIRE2

    if (share::hasI2cPad()) {
        const uint32_t pad = share::pollI2cPad();
        if (pad & share::PAD_UP)    joy |= (1 << 0);
        if (pad & share::PAD_DOWN)  joy |= (1 << 1);
        if (pad & share::PAD_LEFT)  joy |= (1 << 2);
        if (pad & share::PAD_RIGHT) joy |= (1 << 3);
        if (pad & share::PAD_A)     joy |= (1 << 4);
        if (pad & share::PAD_B)     joy |= (1 << 5);
    }

    gx4000_joy_state = joy;

    gx4000_set_cpc_joy_key(CPC_JOY0_UP,    (joy & (1 << 0)) != 0);
    gx4000_set_cpc_joy_key(CPC_JOY0_DOWN,  (joy & (1 << 1)) != 0);
    gx4000_set_cpc_joy_key(CPC_JOY0_LEFT,  (joy & (1 << 2)) != 0);
    gx4000_set_cpc_joy_key(CPC_JOY0_RIGHT, (joy & (1 << 3)) != 0);
    gx4000_set_cpc_joy_key(CPC_JOY0_FIRE1, (joy & (1 << 4)) != 0);
    gx4000_set_cpc_joy_key(CPC_JOY0_FIRE2, (joy & (1 << 5)) != 0);

    // Quit: G0 / Opt key held ≥ 1 s
    bool go_down = kb.isKeyPressed(0x00) || kb.isKeyPressed('`');
    if (go_down) {
        if (s_go_press_ms == 0)
            s_go_press_ms = now ? now : 1;
        else if ((now - s_go_press_ms) >= 1000) {
            s_lastQuit = true;
            return true;   // quit
        }
    } else {
        s_go_press_ms = 0;
    }

    s_lastQuit = false;
    return false;
}
