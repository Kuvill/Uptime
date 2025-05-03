project( 'Uptimer', 'cpp' )

add_project_arguments('-std=c++23', language: 'cpp')

# i don't want to set compiler for gtk blueprint, so i need to set up CI

incdir = include_directories('./inc')
incdir = include_directories('.')

demon_sources = [
  './src/logger.cpp',
  './src/time.cpp',

  './main.cpp',
  './src/demon/db.cpp',
  './src/demon/server.cpp',
  './src/demon/get_uptime.cpp',
  './src/demon/ram_storage.cpp',
]

gui_sources = [
  './src/logger.cpp',
  './src/time.cpp',

  # ll. i have to unlock db_out from db
  './src/demon/db.cpp',
  './src/demon/ram_storage.cpp',

  './src/gui/main.cpp',
  './src/gui/column_view.cpp',
  './src/gui/record_item.cpp',
  './src/gui/lazy_load.cpp',
  './src/gui/db_out.cpp',
  './src/gui/context.cpp'
]

gtkdeps = [
  dependency( 'gtk4' ),
  dependency( 'libadwaita-1' ),
]

dbdeps = [
    dependency( 'sqlite3' )
]

ui_file = './res/gui/main.ui'
custom_target(
  command: ['glib-compile-resources', 'prefetch.xml',
    '--sourcedir=./res/gui/', '--target=ui.gresource'],
  output: 'ui.gresource'
)

buildtype = get_option('buildtype')
if buildtype == 'debug'
  add_project_arguments([ '-DDEBUG' ], language: 'cpp')
endif

fs = import('fs')
install_subdir( 'res', install_dir: fs.expanduser('~/.local/share/uptimer/' ) )

executable( 'Uptimer', 
  demon_sources, include_directories: incdir,
  dependencies : dbdeps ,
  install: true )

executable( 'Gui', 
  gui_sources, include_directories: incdir,
  dependencies : [gtkdeps, dbdeps],
  install: true )
