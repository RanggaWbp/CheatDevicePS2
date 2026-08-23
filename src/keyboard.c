#include <string.h>
#include <stdio.h>

#include "keyboard.h"
#include "pad.h"
#include "graphics.h"

/*
 * ============================================================
 * PS2 HOMEBREW VIRTUAL KEYBOARD
 * ============================================================
 *
 * Logical grid:
 *
 *  Q W E R T Y U I O P
 *  A S D F G H J K L DEL
 *  Z X C V B N M , . 123
 *  CAPS       SPACE        OK
 *
 * Bottom row:
 *   CAPS  = column 0
 *   SPACE = column 1, rendered as ONE wide button
 *   OK    = column 9
 *
 * Empty cells are not rendered.
 */

#define KB_COLS 10
#define KB_ROWS 4

/* ============================================================
 * LOWERCASE
 * ============================================================ */

static const char *layoutLower[KB_ROWS][KB_COLS] =
{
    {
        "q", "w", "e", "r", "t",
        "y", "u", "i", "o", "p"
    },

    {
        "a", "s", "d", "f", "g",
        "h", "j", "k", "l", "DEL"
    },

    {
        "z", "x", "c", "v", "b",
        "n", "m", ",", ".", "123"
    },

    {
        "CAPS", "SPACE", "", "", "",
        "", "", "", "", "OK"
    }
};

/* ============================================================
 * UPPERCASE
 * ============================================================ */

static const char *layoutUpper[KB_ROWS][KB_COLS] =
{
    {
        "Q", "W", "E", "R", "T",
        "Y", "U", "I", "O", "P"
    },

    {
        "A", "S", "D", "F", "G",
        "H", "J", "K", "L", "DEL"
    },

    {
        "Z", "X", "C", "V", "B",
        "N", "M", ",", ".", "123"
    },

    {
        "caps", "SPACE", "", "", "",
        "", "", "", "", "OK"
    }
};

/* ============================================================
 * NUMBERS
 * ============================================================ */

static const char *layoutNumbers[KB_ROWS][KB_COLS] =
{
    {
        "1", "2", "3", "4", "5",
        "6", "7", "8", "9", "0"
    },

    {
        "-", "/", ":", ";", "(",
        ")", "$", "&", "@", "DEL"
    },

    {
        "#", "%", "+", "=", "*",
        "\"", "'", ".", ",", "SYM"
    },

    {
        "ABC", "SPACE", "", "", "",
        "", "", "", "", "OK"
    }
};

/* ============================================================
 * SYMBOLS
 * ============================================================ */

static const char *layoutSymbols[KB_ROWS][KB_COLS] =
{
    {
        "[", "]", "{", "}", "#",
        "%", "^", "*", "+", "="
    },

    {
        "_", "\\", "|", "~", "<",
        ">", "!", "?", "'", "DEL"
    },

    {
        "-", "/", ":", ";", "(",
        ")", "$", "&", "@", "123"
    },

    {
        "ABC", "SPACE", "", "", "",
        "", "", "", "", "OK"
    }
};

/* ============================================================
 * LAYOUT TABLE
 * ============================================================ */

static const char *(*layouts[KB_NUM_LAYOUTS])[KB_COLS] =
{
    layoutLower,
    layoutUpper,
    layoutNumbers,
    layoutSymbols
};

/* ============================================================
 * KEYBOARD STATE
 * ============================================================ */

static kbLayout_t currentLayout = KB_LAYOUT_LOWER;

static int cursorRow = 0;
static int cursorCol = 0;

static int isOpen = 0;

static char buffer[KEYBOARD_BUFFER_SIZE];

static void (*doneCallback)(const char *text) = NULL;

/* ============================================================
 * OPEN
 * ============================================================ */

void keyboardOpen(
    const char *initialText,
    void (*onDone)(const char *text)
)
{
    if(initialText)
    {
        strncpy(
            buffer,
            initialText,
            KEYBOARD_BUFFER_SIZE - 1
        );
    }
    else
    {
        buffer[0] = '\0';
    }

    buffer[KEYBOARD_BUFFER_SIZE - 1] = '\0';

    doneCallback = onDone;

    currentLayout = KB_LAYOUT_LOWER;

    cursorRow = 0;
    cursorCol = 0;

    isOpen = 1;
}

/* ============================================================
 * CLOSE
 * ============================================================ */

void keyboardClose()
{
    isOpen = 0;
    doneCallback = NULL;
}

/* ============================================================
 * IS OPEN
 * ============================================================ */

int keyboardIsOpen()
{
    return isOpen;
}

/* ============================================================
 * GET BUFFER
 * ============================================================ */

const char *keyboardGetBuffer()
{
    return buffer;
}

/* ============================================================
 * APPEND CHARACTER
 * ============================================================ */

static void appendChar(const char *key)
{
    size_t len;
    size_t keyLen;

    if(!key)
        return;

    len = strlen(buffer);
    keyLen = strlen(key);

    if(len + keyLen >= KEYBOARD_BUFFER_SIZE - 1)
        return;

    strcat(buffer, key);
}

/* ============================================================
 * BACKSPACE
 * ============================================================ */

static void backspace()
{
    size_t len = strlen(buffer);

    if(len > 0)
        buffer[len - 1] = '\0';
}

/* ============================================================
 * VALID KEY
 * ============================================================ */

static int isValidKey(int row, int col)
{
    const char *key;

    if(row < 0 || row >= KB_ROWS)
        return 0;

    if(col < 0 || col >= KB_COLS)
        return 0;

    key = layouts[currentLayout][row][col];

    if(!key)
        return 0;

    if(key[0] == '\0')
        return 0;

    return 1;
}

/* ============================================================
 * ACTIVATE KEY
 * ============================================================ */

static void activateKey()
{
    const char *key;

    key = layouts[currentLayout][cursorRow][cursorCol];

    if(!key || key[0] == '\0')
        return;

    /* DELETE */
    if(strcmp(key, "DEL") == 0)
    {
        backspace();
    }

    /* SPACE */
    else if(strcmp(key, "SPACE") == 0)
    {
        appendChar(" ");
    }

    /* OK */
    else if(strcmp(key, "OK") == 0)
    {
        if(doneCallback)
            doneCallback(buffer);

        keyboardClose();
    }

    /* CAPS */
    else if(strcmp(key, "CAPS") == 0)
    {
        currentLayout = KB_LAYOUT_UPPER;
    }

    /* LOWERCASE */
    else if(strcmp(key, "caps") == 0)
    {
        currentLayout = KB_LAYOUT_LOWER;
    }

    /* NUMBERS */
    else if(strcmp(key, "123") == 0)
    {
        currentLayout = KB_LAYOUT_NUMBERS;
    }

    /* SYMBOLS */
    else if(strcmp(key, "SYM") == 0)
    {
        currentLayout = KB_LAYOUT_SYMBOLS;
    }

    /* ABC */
    else if(strcmp(key, "ABC") == 0)
    {
        currentLayout = KB_LAYOUT_LOWER;
    }

    /* NORMAL CHARACTER */
    else
    {
        appendChar(key);
    }

    /*
     * Make sure cursor never points to an empty cell
     * after changing layout.
     */
    if(!isValidKey(cursorRow, cursorCol))
    {
        cursorRow = 0;
        cursorCol = 0;
    }
}

/* ============================================================
 * MOVE CURSOR
 * ============================================================ */

static void moveCursor(int dRow, int dCol)
{
    int row = cursorRow;
    int col = cursorCol;

    int attempts = 0;
    int maxAttempts = KB_ROWS * KB_COLS;

    while(attempts < maxAttempts)
    {
        row += dRow;
        col += dCol;

        /* Horizontal wrap */
        if(col < 0)
            col = KB_COLS - 1;

        if(col >= KB_COLS)
            col = 0;

        /* Vertical wrap */
        if(row < 0)
            row = KB_ROWS - 1;

        if(row >= KB_ROWS)
            row = 0;

        attempts++;

        if(isValidKey(row, col))
        {
            cursorRow = row;
            cursorCol = col;
            return;
        }
    }
}

/* ============================================================
 * PAD INPUT
 * ============================================================ */

int keyboardHandlePad(
    u32 pad_pressed,
    u32 pad_held
)
{
    if(!isOpen)
        return 0;

    /*
     * Navigation uses PAD_PRESSED instead of PAD_HELD.
     *
     * This prevents the cursor from flying across the
     * keyboard when a direction is held.
     */

    if(pad_pressed & PAD_UP)
    {
        moveCursor(-1, 0);
    }
    else if(pad_pressed & PAD_DOWN)
    {
        moveCursor(1, 0);
    }
    else if(pad_pressed & PAD_LEFT)
    {
        moveCursor(0, -1);
    }
    else if(pad_pressed & PAD_RIGHT)
    {
        moveCursor(0, 1);
    }

    /* CROSS = Select */
    if(pad_pressed & PAD_CROSS)
    {
        activateKey();
    }

    /* TRIANGLE = Backspace */
    else if(pad_pressed & PAD_TRIANGLE)
    {
        backspace();
    }

    /* CIRCLE = Close */
    else if(pad_pressed & PAD_CIRCLE)
    {
        keyboardClose();
    }

    /* L1 = CAPS / lowercase */
    else if(pad_pressed & PAD_L1)
    {
        if(currentLayout == KB_LAYOUT_LOWER)
        {
            currentLayout = KB_LAYOUT_UPPER;
        }
        else if(currentLayout == KB_LAYOUT_UPPER)
        {
            currentLayout = KB_LAYOUT_LOWER;
        }
    }

    /* R1 = Numbers / Symbols */
    else if(pad_pressed & PAD_R1)
    {
        if(currentLayout == KB_LAYOUT_NUMBERS)
        {
            currentLayout = KB_LAYOUT_SYMBOLS;
        }
        else
        {
            currentLayout = KB_LAYOUT_NUMBERS;
        }
    }

    /*
     * Prevent compiler warning if pad_held isn't otherwise used.
     */
    (void)pad_held;

    /*
     * Consume keyboard input so normal menu navigation
     * doesn't receive the same controller event.
     */
    return 1;
}

/* ============================================================
 * DRAW KEYBOARD
 * ============================================================ */

void keyboardDraw()
{
    if(!isOpen)
        return;

    graphicsDrawKeyboard(
        (const char *(*)[KB_COLS])layouts[currentLayout],
        KB_ROWS,
        KB_COLS,
        cursorRow,
        cursorCol
    );

    graphicsDrawSearchBox(buffer);
}
