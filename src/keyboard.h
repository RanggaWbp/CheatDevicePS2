/*
 * Virtual Keyboard (search)
 * On-screen keyboard for the game-list search feature, navigated with the
 * D-pad. This is separate from the game's existing displayTextEditMenu()
 * (used for renaming cheats/codes) — this one is purpose-built for live
 * filtering as the user types.
 * Supports lowercase, uppercase, numbers, and symbol layouts.
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <tamtypes.h>

#define KEYBOARD_BUFFER_SIZE 64

typedef enum {
    KB_LAYOUT_LOWER,
    KB_LAYOUT_UPPER,
    KB_LAYOUT_NUMBERS,
    KB_LAYOUT_SYMBOLS,
    KB_NUM_LAYOUTS
} kbLayout_t;

// Open the keyboard. initialText may be NULL for an empty buffer.
// onDone is called with the final text when the user confirms,
// or with NULL if the user cancels.
void keyboardOpen(const char *initialText, void (*onDone)(const char *text));
void keyboardClose();
int  keyboardIsOpen();

// Feed pad input to the keyboard while it's open. Returns 1 if it consumed the input.
int keyboardHandlePad(u32 padPressed, u32 padHeld);

// Current text in the buffer (live, updates as user types).
const char *keyboardGetBuffer();

// Render the keyboard overlay + search box. Call from menuRender() while open.
void keyboardDraw();

#endif // KEYBOARD_H
