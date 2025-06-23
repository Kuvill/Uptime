# Uptimer

## How do you spend your time?

### DE support

- [x] SwayWM


### How to build

git clone --depth=1 https://github.com/Kuvill/Uptime.git

cd Uptime;
meson setup build/ --buildtype=release;
cd build;
ninja;
meson install;

That's all!

#### Dependencies

Demon:
- sqlite3

GUI:
- gtk4
- adwaita-1
