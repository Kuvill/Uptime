# Uptimer

## How do you spend your time?

### How to build

git clone --depth=1 https://github.com/Kuvill/Uptime.git

cd Uptime
meson setup build/
ninja -C build

Now you can execute build/Gui and build/Demon! (now only from project root path)
#### Dependencies

Demon:
> sqlite3

GUI:
> gtk4
> adwaita-1
