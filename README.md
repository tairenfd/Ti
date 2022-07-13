Ti - A Kilo fork
====================================

This is solely being used as a learning tool and isn't intended to be used in lieu of a well established
text editor.

Keeping true with the original Kilo project, the SLOC for this project aims to stay under 1024 lines.
Currently sitting at ~1100 lines according to sloc, but theres plenty of refactoring that can be done.

See original kilo source code at [github/antirez](https://github.com/antirez/kilo "Kilo Text Editor")

See [Paige Ruten's](https://github.com/paigeruten "paigeruten") 'BYOE' documents [here](https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html "Build Your Own Editor")

Any advice is greatly appreciated!

cloc --diff  ti.c compared with [kilo-src by paigeruten](https://github.com/snaptoken/kilo-src/blob/master/kilo.c)
![image](https://user-images.githubusercontent.com/71906074/178766002-fd8a31a8-7931-4429-b1c1-42996605b34a.png)

cloc --diff  ti.c compared with [kilo by antirez](https://github.com/antirez/kilo/blob/master/kilo.c)
![image](https://user-images.githubusercontent.com/71906074/178768579-571798d0-8f82-474f-b34c-c49e28a734b5.png)

Asciicast
=========

[![asciicast](https://asciinema.org/a/mQMB04FYcA8uVQxpkHmbkgl4L.svg)](https://asciinema.org/a/mQMB04FYcA8uVQxpkHmbkgl4L)

Features
========

- Simple Syntax Highlighting for:
    - C
    - Python
    - Go
    - Rust
    - Javascript
    - Bash (Needs a lot of improvement)
    - TODO...
- Search functionality
- Modal editor, 2 modes - NORMAL/INSERT
- Simple editor-command line
- Set theme color

Keybinds
========

#### General bindings

- **Ctrl + q** : Quit, if unsaved changes, a prompt will appear and require you to press <ENTER> 

- **Ctrl + s** : Save file

- **ESC** : Enter *Normal mode*
- **i** : Enter *Insert mode*

- **HOME** : Go to start of row, if already at start, go to previous row
- **END** : Go to end of row, if already at end, go to next row

- **Page Up** : Go up one page
- **Page Down** : Go down one page

#### Normal mode

- **/** : search
    - *'Down arrow (↓) / Right arrow (→)'* - Next Search
    - *'Up arrow (↑) / Left arrow (←)'* - Previous search result
    - *'ESC'* - Cancel search
    - *'ENTER'* - Go to current selection

- **h, j, k, l** : left, down, up, right movement keys

- **w** : next word
- **W** : previous word

- **x** : delete character under cursor
- **dd** : delete current row
- **dw** : delete from current char to end of word
- **dx** : delete current word
- **ENTER** : insert row

- **:** : open editor command line
    - *'w'* or *'write'* - Save file
    - *'q'* or *'quit'* - Quit, will prompt if unsaved changes
    - *'!q'* or *'!quit'* - Force quit
    - *'wq'* or *'done'* - Save and quit
    - *'themes'* - show available themes
    - *'set theme <color>'* - set theme
    - *'h'* or *'help'* - Help menu, currently just directs user to README

#### Insert mode

- All general keybindings should work, as well as normal typing to screen

SETTINGS
========
- TI_TAB_STOP = Tab render size
- in editor-command: set theme <color> = Editor's "theme"
    - Black
    - Red
    - Green
    - Yellow
    - Blue
    - Magenta
    - Cyan
    - White / Default

TODO
====

- Syntax highlighting:
    - Zsh
    - Make
    - Git
    - HTML
    - etc...
- Scroll buffer
- Refactoring of deletion functionalities
- Refactor binds
- Refactor suntax(?)
- Fix search function (same row functionality)
- Undo / Redo functionality
- Rewrite C -> Rust?

KNOWN ISSUES
============
- Search function only finds the first match in a row
- Del current word will go to end of next word when
deleting a string of spaces/tabs

Contributers / Credit To
========================

- tairenfd
- antirez
- paigeruten
- Python and Go syntax highlighting keywords/types from 
[dvwallin's](https://github.com/dvwallin) 
[openemacs project](https://github.com/dvwallin/openemacs), another fork of Kilo!
