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
[-] KDE (kwin scripting unusable. js just haven't functions from documentation. Native doesn't have documentation for use or even build)
[-] Gnome (In plan)

