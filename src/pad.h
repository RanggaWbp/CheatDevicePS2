#ifndef PAD_H
#define PAD_H

#include <tamtypes.h>
#include <libpad.h>

typedef enum delayTime {
    DELAYTIME_FAST = 2,
    DELAYTIME_SLOW = 6
} delayTime_t;

// Batas maksimum percobaan padGetState() sebelum menyerah untuk frame ini.
// Ini bukan satuan waktu (busy-loop, bukan sleep), jadi dipilih cukup besar
// agar controller kabel/BT yang normal tetap selalu lolos dalam batas ini,
// tapi tidak sampai membuat aplikasi macet total kalau controller memang
// sedang tidak tersambung.
#define PAD_STATE_WAIT_MAX_TRIES 2000000

// Initialize pad
void padInitialize();

// Poll controller for button status
void padPoll(delayTime_t delayTime);

// Get buttons pressed momentarily
inline u32 padPressed();

// Get buttons held down
inline u32 padHeld();

#endif // PAD_H
