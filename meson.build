# i can use wayland module
# btw i should install main.ui and create dir without installation?

project( 'Uptimer', 'cpp',
  default_options: ['cpp_std=c++23', 'buildtype=release', 'b_lto=true'],
  meson_version: '>=1.1'
)


### Project settings
buildtype = get_option('buildtype')
enable_profiling = get_option('enable_profiling') # default, debug, none
native = get_option('native') # true / false
logging = get_option('logging')

cppflags = []

if native
  cppflags += '-march=native'
endif

if (not enable_profiling) and (buildtype != 'debug')
  cppflags += '-fno-semantic-interposition'
endif

if not logging
  cppflags += '-DNOLOG'
endif

if buildtype == 'debug'
  add_project_arguments([ '-DDEBUG' ], language: 'cpp')
endif

# Apply setting

if buildtype != 'plain'
  add_project_arguments(cppflags, language: 'cpp')
endif

add_project_arguments('-std=c++23', language: 'cpp')



### Sources
incdir = include_directories('.', './inc')

demon_sources = [
  './src/logger.cpp',
  './src/time.cpp',
  './src/change_dir.cpp',
  './src/check_unique.cpp',
  './src/utils.cpp',
  './src/notification.cpp',
  './src/settings.cpp',

  './src/demon/db.cpp',
  './src/demon/server.cpp',
  './src/demon/better_upimte.cpp',
  './src/demon/get_uptime.cpp',
  './src/demon/ram_storage.cpp',
  './src/demon/epoll.cpp',
  './src/demon/plugin.cpp',

  './src/demon/de_dependent/sway.cpp',
  './src/demon/de_dependent/hyprland.cpp',
  './src/demon/de_dependent/hyprland_true.cpp',
]

gui_sources = [
  './src/logger.cpp',
  './src/time.cpp',
  './src/change_dir.cpp',
  './src/utils.cpp',
  './src/notification.cpp',
  './src/settings.cpp',

  './src/gui/record_item.cpp',
  './src/gui/lazy_load.cpp',
  './src/gui/db.cpp',
  './src/gui/state.cpp',
  './src/gui/client.cpp',
  './src/gui/context.cpp',

  './src/demon/db.cpp',
  './src/demon/ram_storage.cpp',
  './src/demon/plugin.cpp',

  './src/gui/builder.cpp',
  './src/gui/builder/column_view.cpp',
  './src/gui/builder/stack.cpp',
  './src/gui/builder/table_navigation.cpp',

  './src/gui/utils/table_menu.cpp'
]

server_sources = [
  './src/logger.cpp',

  './src/server/server.cpp'
]

### dependings

gtkdeps = [
  dependency( 'gtk4' ),
  dependency( 'libadwaita-1' ),
  dependency( 'libnotify' )
]

dbdeps = [
    dependency( 'sqlite3' ),
    dependency( 'libnotify' )
]

### Resources

ui_file = './res/gui/main.ui'
custom_target(
  command: ['glib-compile-resources', 'prefetch.xml',
    '--sourcedir=./res/gui/', '--target=ui.gresource'],
  output: 'ui.gresource'
)

### Targets 

fs = import('fs')

# data_dir = get_option('datadir') / 'uptimer/'
data_dir1 = fs.expanduser('~/.local/share/') # i have to pass datadir into my code to support this feautre
install_data( 'res/data/main.ui',  install_dir: data_dir1 / 'uptimer' )

executable( 'Uptimer', 
  ['main.cpp', demon_sources], include_directories: incdir,
  dependencies : dbdeps ,
  install: true )

executable( 'Gui', 
  ['./src/gui/main.cpp', gui_sources], include_directories: incdir,
  dependencies : [gtkdeps, dbdeps],
  install: true )

#executable( 'Server',
#  ['./src/server/main.cpp', server_sources], include_directories: incdir )

# executable( 'Tests',
#  ['tests/tests.cpp', demon_sources, gui_sources], include_directories: incdir,
#  dependencies: [gtkdeps, dbdeps] )
