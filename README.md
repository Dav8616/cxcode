# CX Code  
### A well-featured code editor for the TI-Nspire CX / CX II — powered by Ndless + SDL

[![Build](https://img.shields.io/badge/build-make-blue?style=flat-square)]()
![Platform](https://img.shields.io/badge/platform-TI--Nspire%20CX%20%2F%20CX%20II-lightgrey?style=flat-square)
![License](https://img.shields.io/badge/license-GNU-green?style=flat-square)
![Status](https://img.shields.io/badge/status-alpha-orange?style=flat-square)

CXCode brings a VS-Code-style editing UI to your graphing calculator, because clearly the Nspire wasn’t suffering enough already.

---

##  Features

- **Syntax highlighting** for keywords, strings, comments, and numbers     (Planned, not implemented yet)
- **Smooth mouse-style cursor** with proper touchpad tracking  
- **Undo/Redo stack**
- **Copy/Paste via internal clipboard**  
- **File Open / Save / New** with status feedback  
- **Menu bar UI** (File / Edit / View / Help)  
- **Configurable themes** with RGB fields  
- **Theme file loader & saver** (`.theme.tns`)  
- **Built-in Theme Editor** with R/G/B sliders     (Planned, slightly implemented)
- **Line numbers toggle**  
- **Scrolling, cursor clamping, and viewport handling**  

---

## Building

Requires:

- Ndless SDK  
- libndls  
- SDL for TI-Nspire (NspireSDL (NOT nSDL)  
- arm-none-eabi toolchain (bundled with Ndless SDK)

Compile with:

```sh
make
```

This produces:

- cxcode.elf   # raw executable
- cxcode.tns   # Ndless executable

Copy cxcode.tns to your calculator’s Ndless documents folder and run it after installing ndless.

---

# Controls
Editor:
- Action → Keys
- Move text cursor  →  Arrow keys, click on line
- Move mouse cursor → Touchpad
- Click → Press on touchpad (:
- New line → Enter
- Delete → Backspace
- Undo → Ctrl + Z
- Redo → Ctrl + Y
- Copy / Paste →	Ctrl + C / Ctrl + V

Menu Bar:
- Action →	Keys
- Toggle  →  menu bar	Esc
- Move menus  → 	Left/Right
- Select item  → 	Up/Down
- Activate →	Enter

Theme Editor:
- Modify R/G/B → Left/Right
- Switch R/G/B → Tab
- Save theme →	Enter
- Exit →	Esc
---

# Theme Format

Themes are plain-text .theme.tns files using:

key=R,G,B

Example:

bg=0,0,0
text=255,255,255
cursor=255,255,0
menu_bg=24,24,24
comment=106,153,85

Load/save handled by:

    theme_load()

    theme_save()

To edit a theme at present:
1. Save the current theme by selecting the **save theme** option under the view menu,
2. Select the **open** option under the file menu.
3. enter ``ndless/cxcode.theme.tns`` in the text box and press enter.
4. Edit the RGB 0-255 values next to each option.
# DO NOT CHANGE THE VARIABLE NAMES. IF YOU DO, YOUR THEME WILL BE UNUSABLE AS THE INTERPRETER WILL NOT UNDERSTAND IT.

The mouse cursor icon is currently uneditable except through editing the source code.

Default theme:
-    Background - 0 0 0
-    Text - 255 255 255
-    Line Numbers - 128 128 128
-    Cursor - 255 255 0
-    Title Bar - 32 32 32
-    Status Bar - 32 32 32
-    Menu BG - 24 24 24
-    Menu Border - 64 64 64
-    Menu Selected - 0 100 200
-    Keyword - 86 156 214 (For syntax highlighting later)
-    String - 206 145 120 (For syntax highlighting later)
-    Comment - 106 153 85 (For syntax highlighting later)
-    Number - 181 206 168 (For syntax highlighting later)
    
---

# Dependencies

    Ndless

    libndls

    SDL (TI-Nspire port)

    C/C++ standard libs

Everything is expected to be inside your existing Ndless SDK installation.

---

# Status Messages

The bottom bar shows quick feedback for actions like:

-    Saving / loading

-    Errors

-    Undo/redo status

-    Menu actions

---

# Known Quirks

-    Mouse tracking depends on raw touchpad reports (Ndless).

-    Clicking outside a text box doesn't close it.

-    Syntax highlighting is not yet implemented.

-    Some things say that they can do things (ie. .tns auto append on the open file dialogue), but don't actually do anything. This is because I wasn't planning on releasing cxcode so soon, and they are planned but not implimented. 
---

# Roadmap

    Tabs (multiple files)

    Plugin-ish formatting helpers

    Mini-map (wish me luck)

    USB keyboard support?

    Syntax highlighting, 
    
    Scrolling text window (Text that extends beyond the window is unviewable :/ ), 
    
    Javascript interpreter (Using Duktape), 
    
    In-app proper theme editor, 
    
    Uppercase (yeah I forgot uppercase).
---

# License

This program is free software, and is licensed under the GNU GPL V3 License.
You should have recieved a copy of the license with this program. If not, review it [here](https://www.gnu.org/licenses/gpl-3.0.en.html).


~ David Choate (Dav86)
Doing questionable things to graphing calculators since 2020

Copyright © 2025 David Choate
