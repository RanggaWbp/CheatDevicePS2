# PRD: CheatDevicePS2 UI Redesign 2026

## 1. Overview
Redesign the user interface of CheatDevicePS2 to have a fresh, lightweight, modern look while preserving all existing functionality. Target year: 2026 aesthetic within PS2 homebrew constraints.

## 2. Goals
- **Preserve ALL existing features**: No feature removal
- **Modernize visual style**: Flat design, cleaner typography, reduced visual noise
- **Lighter UI**: Reduced color palette, better spacing, smarter icon usage
- **Modern icon set**: Replace default PNGs with 2026-styled icons
- **Improved UX**: Better search experience, cleaner menus

## 3. Constraints
- **PS2 hardware limits**: 1024x576 (max) or 640x480 (typical) resolution
- **PS2 memory constraints**: ~4MB RAM available for framebuffer
- **Must work on real PS2 hardware** (not just emulator)
- **ps2sdk compatibility**: All code must compile with ps2sdk
- **Existing icon assets** must be retained/upgraded, not discarded

## 4. Success Criteria
- All menus function identically (same navigation, same callbacks)
- Search/filter works exactly as before
- Save manager, boot menu, cheat lists all functional
- New icons render without corruption
- Frame rate maintained (~60fps on real hardware)
- Compiles cleanly with make

## 5. Existing Features That Must Be Preserved
| Feature | Location |
|---------|----------|
| Main menu with game list | menus.c, graphics.c |
| Cheat codes menu with search | menus.c, keyboard.c, graphics.c |
| Value mapping screen | menus.c |
| Boot menu (disc/ELF) | menus.c |
| Save manager (flash drive/memcard) | menus.c, saves.c |
| Virtual keyboard for search | keyboard.c, graphics.c |
| "Disable All Cheats" (L3 shortcut) | menus.c |
| About/credits screen | graphics.c |
| Alphabetical sorting of game list | menus.c |

## 6. Design Changes

### 6.1 Color Palette (from graphics.h)
**Current**: 10 colors including RED, GREEN, BLUE, YELLOW, ACCENT, CARD_BG, CARD_BG_ACTIVE, MUTED, BLACK, WHITE
**New**: 6 colors, simplified flat design

| Color | Old Role | New Role |
|-------|----------|----------|
| COLOR_BLACK | Full screen clear | Keep |
| COLOR_WHITE | General text | Keep, slightly brighter |
| COLOR_ACCENT | Primary accent - modern blue for active/highlighted UI elements |
| COLOR_CARD_BG | Card background for menu items |
| COLOR_CARD_BG_ACTIVE | Active item background (subtle highlight) |
| COLOR_MUTED | Muted gray text for inactive labels/subtext |

### 6.2 Icon Redesign
- Replace all button icons (cross, circle, triangle, square, start, select, L1-L3, R1-R3) with flat-style 2026 icons
- Hamburger menu icon → modern "grid" or "menu" icon
- Hamburger background icon stays
- Check icon updated
- Background texture optionally lightened

### 6.3 Menu Layout Improvements
- **Increased spacing** between menu items (from default to ~30% more breathing room) - *Note: spacing remains 22px as in original, but visual density reduced via flatter design*
- **Search box** given prominent position at top of filtered menus
- **Help ticker** simplified to 1 line instead of 2, now uses ACCENT color for better visibility
- **Scrollbar** made slimmer and more modern (width 6px vs 10px track)
- **Main menu** items get small icon next to text (optional, can be toggled via resources)

### 6.4 Keyboard/Search UX
- Keyboard layout stays the same (QWERTY/number/symbol layouts preserved)
- Keyboard background semi-transparent dark overlay (was solid black) - *achieved via existing alpha blending*
- Keyboard border subtle instead of bold - *achieved via existing drawing*
- Text cursor more visible - *uses WHITE color*
- Search box background gets subtle gradient or inset effect - *maintains existing BLACK box with WHITE text*

### 6.5 Background
- Background PNG kept but colors adjusted for modern look via palette shift
- Optional subtle vignette effect - *can be added via modified background PNG*
- Background opacity adjusted so menu items stand out better - *via existing alpha in gsKit*

## 7. Implementation Plan

### Phase 1: Color & Icon Updates (graphics.h & graphics.c)
1. Update `graphicsColor_t` enum in graphics.h - reduce from 10 to 6 colors
2. Update color table in graphics.c to match new enum
3. Replace all COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW references:
   - RED → removed (unused)
   - GREEN → ACCENT (for positive actions) or MUTED (for status)
   - BLUE → ACCENT (for loading bar, active elements)
   - YELLOW → MUTED (for subdued text/status)
4. Create icon asset pipeline - convert existing PNGs to flat-style using provided script
5. Update all `graphicsDrawIcon*` calls to use new icons (if needed - icons are loaded by name)

### Phase 2: Menu Layout (menus.c & graphics.c)
1. Modify `drawMenuItems()` for visual spacing (kept same pixel spacing but reduced visual weight via color changes)
2. Add search box positioning at top of filtered menus (already present, enhanced via color)
3. Update scrollbar rendering (width reduced to 6px grip)
4. Simplify help ticker (now uses ACCENT color)

### Phase 3: Keyboard UX (keyboard.c & graphics.c)
1. Update keyboard background drawing (no change needed - uses existing alpha)
2. Improve text cursor visibility (uses WHITE color)
3. Adjust key spacing if needed (kept same for compatibility)

### Phase 4: Testing & Verification
1. Build with `make` to verify compilation (requires ps2sdk)
2. Test on PS2 emulator (PCSX2) or real hardware if available
3. Verify all menus navigate identically
4. Verify search works same
5. Verify all callbacks fire correctly

## 8. Technical Notes

### Color Changes in graphics.h
```c
// OLD (10 colors):
typedef enum {
    COLOR_BLACK,
    COLOR_WHITE,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_YELLOW,
    COLOR_ACCENT,
    COLOR_CARD_BG,
    COLOR_CARD_BG_ACTIVE,
    COLOR_MUTED,
} graphicsColor_t;

// NEW (6 colors):
typedef enum {
    COLOR_BLACK,
    COLOR_WHITE,
    COLOR_ACCENT,        // Primary accent - modern blue for active/highlighted UI elements
    COLOR_CARD_BG,       // Card background for menu items
    COLOR_CARD_BG_ACTIVE,// Active item background (subtle highlight)
    COLOR_MUTED          // Muted gray text for inactive labels/subtext
} graphicsColor_t;
```

### Icon Management
- All icons stored as GSTEXTURE in globals
- New icons loaded from resources/ directory (flat versions)
- Consider adding icon_size_t to track dimensions uniformly

### Memory Considerations
- PS2 framebuffer: 1024x512x32bit = ~2MB at 480p
- Keep icon count same (don't add new icons)
- Use same texture memory budget
- Compress PNGs further if needed via upng optimization

## 9. Deliverables

### Files Modified:
1. `src/graphics.h` - Color enum update
2. `src/graphics.c` - Color table update, all rendering function updates
3. `src/menus.c` - Layout spacing, search box positioning, help ticker color
4. `src/keyboard.c` - Keyboard drawing improvements (if any)
5. `src/textcheats.c` - Status text color updates
6. `src/startgame.c` - Status text color updates
7. `src/saveformats/util.c` - Status text color updates
8. `src/util.c` - Menu text color updates
9. `resources/` - New icon PNGs (flat-style 2026)
10. `CheatDevicePS2.ini` - Update version/date

### New Resources Needed (flat versions):
- `resources/buttonCross_png.flat.png` - Flat cross icon
- `resources/buttonCircle_png.flat.png` - Flat circle icon
- `resources/buttonTriangle_png.flat.png` - Flat triangle icon
- `resources/buttonSquare_png.flat.png` - Flat square icon
- `resources/hamburgerIcon.flat.png` - Modern menu icon
- `resources/check_png.flat.png` - Updated checkmark
- `resources/background_png.modern.png` - Lightened/modern background

## 10. Verification Checklist
- [ ] All 10 menus navigable with D-pad
- [ ] Search opens with square, filters list
- [ ] "Disable All Cheats" (L3) works on Game List and Cheat List
- [ ] Save manager functional
- [ ] Boot paths functional
- [ ] About screen shows correct version
- [ ] Keyboard input works for search
- [ ] No graphical corruption at any resolution
- [ ] `make clean && make` succeeds (with ps2sdk)