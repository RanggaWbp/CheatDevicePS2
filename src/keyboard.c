#include <string.h>
#include <stdio.h>

#include "keyboard.h"
#include "pad.h"
#include "graphics.h"

// Each layout is a grid of rows/cols. Regular keys hold their own label
// (e.g. "a", "5"). Special keys use fixed labels handled in activateKey():
// "BKSP"=backspace, "SPACE"=space, "OK"=confirm, "CAPS"/"caps"=shift,
// "123"=numbers layout, "SYM"=symbols layout, "ABC"=back to lowercase.
#define KB_COLS 10
#define KB_ROWS 4

static const char *layoutLower[KB_ROWS][KB_COLS] = {
    {"q","w","e","r","t","y","u","i","o","p"},
    {"a","s","d","f","g","h","j","k","l","BKSP"},
    {"z","x","c","v","b","n","m",",",".","123"},
    {"CAPS","SPACE","SPACE","SPACE","SPACE","SPACE","SPACE","SPACE","SPACE","OK"}
};

static const char *layoutUpper[KB_ROWS][KB_COLS] = {
    {"Q","W","E","R","T","Y","U","I","O","P"},
    {"A","S","D","F","G","H","J","K","L","BKSP"},
    {"Z","X","C","V","B","N","M",",",".","123"},
    {"caps","SPACE","SPACE","SPACE","SPACE","SPACE","SPACE","SPACE","SPACE","OK"}
};

static const char *layoutNumbers[KB_ROWS][KB_COLS] = {
    {"1","2","3","4","5","6","7","8","9","0"},
    {"-","/",":",";","(",")","$","&","@","BKSP"},
    {"#","%","+","=","*","\"","'",".",",","SYM"},
    {"ABC","SPACE","SPACE","SPACE","SPACE","SPACE","SPACE","SPACE","SPACE","OK"}
};

static const char *layoutSymbols[KB_ROWS][KB_COLS] = {
    {"[","]","{","}","#","%","^","*","+","="},
    {"_","\\","|","~","<",">","!","?","'","BKSP"},
    {"-","/",":",";","(",")","$","&","@","123"},
    {"ABC","SPACE","SPACE","SPACE","SPACE","SPACE","SPACE","SPACE","SPACE","OK"}
};

static const char *(*layouts[KB_NUM_LAYOUTS])[KB_COLS] = {
    layoutLower, layoutUpper, layoutNumbers, layoutSymbols
};

static kbLayout_t currentLayout = KB_LAYOUT_LOWER;
static int cursorRow = 0;
static int cursorCol = 0;
static int isOpen = 0;
static char buffer[KEYBOARD_BUFFER_SIZE];
static void (*doneCallback)(const char *text) = NULL;

void keyboardOpen(const char *initialText, void (*onDone)(const char *text))
{
    if(initialText)
        strncpy(buffer, initialText, KEYBOARD_BUFFER_SIZE - 1);
    else
        buffer[0] = '\0';
    buffer[KEYBOARD_BUFFER_SIZE - 1] = '\0';

    doneCallback = onDone;
    currentLayout = KB_LAYOUT_LOWER;
    cursorRow = 0;
    cursorCol = 0;
    isOpen = 1;
}

void keyboardClose()
{
    isOpen = 0;
    doneCallback = NULL;
}

int keyboardIsOpen()
{
    return isOpen;
}

const char *keyboardGetBuffer()
{
    return buffer;
}

static void appendChar(const char *key)
{
    size_t len = strlen(buffer);
    size_t keyLen = strlen(key);

    if(len + keyLen >= KEYBOARD_BUFFER_SIZE - 1)
        return; // buffer full

    strcat(buffer, key);
}

static void backspace()
{
    size_t len = strlen(buffer);
    if(len > 0)
        buffer[len - 1] = '\0';
}

static void activateKey()
{
    const char *key = layouts[currentLayout][cursorRow][cursorCol];

    if(!key || key[0] == '\0')
        return;

    if(strcmp(key, "BKSP") == 0)
        backspace();
    else if(strcmp(key, "SPACE") == 0)
        appendChar(" ");
    else if(strcmp(key, "OK") == 0)
    {
        if(doneCallback)
            doneCallback(buffer);
        keyboardClose();
    }
    else if(strcmp(key, "CAPS") == 0)
        currentLayout = KB_LAYOUT_UPPER;
    else if(strcmp(key, "caps") == 0)
        currentLayout = KB_LAYOUT_LOWER;
    else if(strcmp(key, "123") == 0)
        currentLayout = KB_LAYOUT_NUMBERS;
    else if(strcmp(key, "ABC") == 0)
        currentLayout = KB_LAYOUT_LOWER;
    else if(strcmp(key, "SYM") == 0)
        currentLayout = KB_LAYOUT_SYMBOLS;
    else
        appendChar(key);
}

// Move cursor, skipping empty placeholder cells (used for wide space bar).
static void moveCursor(int dRow, int dCol)
{
    int row = cursorRow;
    int col = cursorCol;
    int attempts = 0;

    do {
        row += dRow;
        col += dCol;

        if(row < 0) row = KB_ROWS - 1;
        if(row >= KB_ROWS) row = 0;
        if(col < 0) col = KB_COLS - 1;
        if(col >= KB_COLS) col = 0;

        attempts++;
    } while(layouts[currentLayout][row][col][0] == '\0' && attempts < KB_COLS * KB_ROWS);

    cursorRow = row;
    cursorCol = col;
}

int keyboardHandlePad(u32 pad_pressed, u32 pad_held)
{
    if(!isOpen)
        return 0;

    if(pad_held & PAD_UP)
        moveCursor(-1, 0);
    else if(pad_held & PAD_DOWN)
        moveCursor(1, 0);
    else if(pad_held & PAD_LEFT)
        moveCursor(0, -1);
    else if(pad_held & PAD_RIGHT)
        moveCursor(0, 1);

    if(pad_pressed & PAD_CROSS)
        activateKey();
    else if(pad_pressed & PAD_TRIANGLE)
        backspace();
    else if(pad_pressed & PAD_CIRCLE)
        keyboardClose(); // cancel, no callback
    else if(pad_pressed & PAD_L1)
        currentLayout = (currentLayout == KB_LAYOUT_LOWER) ? KB_LAYOUT_UPPER : KB_LAYOUT_LOWER;
    else if(pad_pressed & PAD_R1)
        currentLayout = (currentLayout == KB_LAYOUT_NUMBERS) ? KB_LAYOUT_SYMBOLS : KB_LAYOUT_NUMBERS;

    return 1; // input was consumed, don't let normal menu navigation run
}

void keyboardDraw()
{
    if(!isOpen)
        return;

    graphicsDrawKeyboard((const char *(*)[KB_COLS])layouts[currentLayout],
                          KB_ROWS, KB_COLS, cursorRow, cursorCol);
    graphicsDrawSearchBox(buffer);
}
