# Uptimer


## How do you spend your time?
Uptimer (work title) - tools for recording some PC usage information (like on-screen (focused) window uptimes, browser focused tab and media in future) and present it with GUI or TUI.

All data is stored locally, what makes Uptimer completely privacy


### How to build

git clone --depth=1 https://github.com/Kuvill/Uptime.git  
  
cd Uptime;  
meson setup build/ --buildtype=release;  
ninja -C build install;  

That's all!

<details>
<summary>Installation details</summary>
  Available targets: Uptimer, Gui 

* Copy binary into /usr/local/bin/
* Copy Gtk scheme into ~/.local/share/uptimer/res/

</details>

#### Dependencies

Demon:
- sqlite3

GUI:
- gtk4
- adwaita-1

### DE support

- [x] SwayWM
- [x] Hyprland
- [ ] X11 (In plan)
- [ ] Gnome (In plan)
- [ ] KDE (kwin scripting unusable. js just haven't functions from documentation. Native doesn't have documentation for use or even build)

- [ ] Windows (On request)
- [ ] MacOS (On request)

## Documentation

### Add your DE support

Implement DesktopEnv ( example declarations are in better_uptimer.hpp, defines - in de_dependent file )
include header file into better_uptimer.cpp, call registerate<{your class}>(); in registrateAll func;
write path to your source file in meason.build: demon_sources

### Architecture summary

**Sway** - Request current focused window every *n* seconds  
**Hyprland** - Request current focused window every *n* seconds (temporarily)

## Screenshots

![Gui early edition](./summer_condition.png)

#### P.S.
Those date picker buttons and labels are fictional, except for the duration model( e.g. years ), which shows info up to today

