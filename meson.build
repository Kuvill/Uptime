# i can use wayland module

project( 'Uptimer', 'cpp' )

### Sources

incdir = include_directories('.', './inc')
# incdir += include_directories('./inc')
# incdir += include_directories('./third_party/')

demon_sources = [
  './src/logger.cpp',
  './src/time.cpp',

  './src/demon/db.cpp',
  './src/demon/server.cpp',
  './src/demon/get_uptime.cpp',
  './src/demon/ram_storage.cpp',
]

gui_sources = [
  './src/logger.cpp',
  './src/time.cpp',

  './src/gui/column_view.cpp',
  './src/gui/record_item.cpp',
  './src/gui/lazy_load.cpp',
  './src/gui/db.cpp',
  './src/gui/context.cpp',
  './src/gui/client.cpp'
]

server_sources = [
  './src/logger.cpp',

  './src/server/server.cpp'
]

### dependings

gtkdeps = [
  dependency( 'gtk4' ),
  dependency( 'libadwaita-1' ),
]

dbdeps = [
    dependency( 'sqlite3' )
]

### Resources

ui_file = './res/gui/main.ui'
custom_target(
  command: ['glib-compile-resources', 'prefetch.xml',
    '--sourcedir=./res/gui/', '--target=ui.gresource'],
  output: 'ui.gresource'
)

### Compiler optpions

add_project_arguments('-std=c++23', language: 'cpp')

buildtype = get_option('buildtype')
if buildtype == 'debug'
  add_project_arguments([ '-DDEBUG' ], language: 'cpp')
endif

### Targets 

fs = import('fs')
install_subdir( 'res', install_dir: fs.expanduser('~/.local/share/uptimer/' ) )

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
