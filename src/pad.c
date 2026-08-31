#include "pad.h"

#include <tamtypes.h>
#include <libpad.h>

typedef struct padState {
    u16 padPressed;
    u16 padHeld;
    u16 padOld;
    u16 initialDelay;       // Number of calls to padPoll() needed before considering a button to be held.
    u16 timeHeld;
    u16 timeHeldDelay;      // While a button is held, require a number of calls to padPoll() to be made before setting padHeld.
    struct padButtonStatus padStatus;
} padState_t;

static padState_t padState;
static char padBuff[256] __attribute__ ((aligned(64)));

void padInitialize()
{
    padInit(0);
    padPortOpen(0, 0, padBuff);

    // Tunggu pad siap SEBELUM minta ganti mode — kalau langsung dipanggil
    // setelah padPortOpen() tanpa nunggu ini, padSetMainMode() bisa gagal
    // diam-diam dan mode tetap di default.
    //
    // Dibatasi dengan PAD_STATE_WAIT_MAX_TRIES: controller Bluetooth bisa
    // butuh waktu jauh lebih lama dari kabel untuk keluar dari state
    // DISCONN/FINDPAD saat reconnect. Tanpa batas ini, boot Cheat Device
    // akan macet total selama itu (bukan cuma pad-nya yang belum siap).
    int state = padGetState(0, 0);
    int tries = 0;
    while((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1) && tries++ < PAD_STATE_WAIT_MAX_TRIES)
        state = padGetState(0, 0);

    // Coba mode analog supaya L3/R3 kebaca.
    padSetMainMode(0, 0, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

    // Tunggu status stabil lagi setelah ganti mode, baru padInfoMode()
    // bisa dipercaya.
    state = padGetState(0, 0);
    tries = 0;
    while((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1) && tries++ < PAD_STATE_WAIT_MAX_TRIES)
        state = padGetState(0, 0);

    // Kalau controller tidak benar-benar masuk mode DualShock (mis. clone/
    // wireless pad yang tidak support), fallback ke digital supaya tombol
    // lain tetap normal.
    if(padInfoMode(0, 0, PAD_MODECURID, 0) != PAD_TYPE_DUALSHOCK)
    {
        padSetMainMode(0, 0, PAD_MMODE_DIGITAL, PAD_MMODE_LOCK);
    }

    padState.padOld = 0xFFFF;
    padState.timeHeld = 0;
    padState.initialDelay = 18;
}

void padPoll(delayTime_t delayTime)
{
    padState.padHeld = 0;
    int state = padGetState(0, 0);
    int tries = 0;
    while((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1) && tries++ < PAD_STATE_WAIT_MAX_TRIES)
        state = padGetState(0, 0);

    // Controller belum stabil (mis. adaptor Bluetooth masih reconnect/
    // re-pairing) setelah dicoba sebanyak PAD_STATE_WAIT_MAX_TRIES kali.
    // JANGAN terus menahan frame ini — biarkan main loop lanjut ke
    // graphicsRender() supaya aplikasi tetap hidup & responsif, dan
    // padPoll() akan dipanggil lagi di frame berikutnya untuk dicoba ulang.
    // Sebelumnya loop ini tanpa batas, sehingga BT yang butuh waktu lebih
    // lama dari kabel untuk reconnect membuat SELURUH aplikasi (bukan cuma
    // input-nya) tampak macet total — persis gejala "tidak bisa reconnect".
    if(state != PAD_STATE_STABLE && state != PAD_STATE_FINDCTP1)
    {
        padState.padPressed = 0;
        padState.timeHeld = 0;
        return;
    }

    padRead(0, 0, &padState.padStatus);
    padState.padPressed = (0xFFFF ^ padState.padStatus.btns) & ~padState.padOld;
    padState.padOld = 0xFFFF ^ padState.padStatus.btns;

    // padState->padRapid will have an initial delay when a button is held
    if((0xFFFF ^ padState.padStatus.btns) && (0xFFFF ^ padState.padStatus.btns) == padState.padOld)
    {
        if(padState.timeHeld++ > padState.initialDelay && padState.timeHeld % delayTime == 0) // don't go too fast!
            padState.padHeld = (0xFFFF ^ padState.padStatus.btns);
        else
            padState.padHeld = padState.padPressed;
    }
    else
        padState.timeHeld = 0;
}

inline u32 padPressed()
{
    return padState.padPressed;
}

inline u32 padHeld()
{
    return padState.padHeld;
}
