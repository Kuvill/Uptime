# Uptimer

## How do you spend your time?

### How to build

git clone --depth=1 https://github.com/Kuvill/Uptime.git

cd Uptime;
meson setup build/ --buildtype=release;
cd build;
ninja build;
meson install;

That's all!

#### Dependencies

Demon:
- sqlite3

GUI:
- gtk4
- adwaita-1
