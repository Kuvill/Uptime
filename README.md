# Uptimer


## How do you spend your time?
The set of applications record focused window uptime into db and provide tools to view processed results via gui or cli


### How to build

git clone --depth=1 https://github.com/Kuvill/Uptime.git

cd Uptime;
meson setup build/ --buildtype=release;
cd build;
ninja;
meson install;

That's all!

<details>
<summary>Installation details</summary>

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
- [ ] Gnome (In plan)
- [ ] KDE (kwin scripting unusable. js just haven't functions from documentation. Native doesn't have documentation for use or even build)

## Documentation

### Add your DE support

Implement DesktopEnv ( example declarations are in better_uptimer.hpp, defines - in de_dependent file )
include header file into better_uptimer.cpp, call registerate<{your class}>(); in registrateAll func;
write path to your source file in meason.build: demon_sources
