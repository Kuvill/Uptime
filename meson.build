project( 'Uptimer', 'cpp' )

# i don't want to compile gtk blueprint, so i need to set up CI

incdir = include_directories('./inc')

# demon_sources = files( './src/demon/' ) 
# gui_sources = files( './src/gui/' ) 
demon_sources = [
  './main.cpp',
  './src/logger.cpp',
  './src/demon/db.cpp',
  './src/demon/server.cpp',
  './src/demon/get_uptime.cpp',
  './src/demon/ram_storage.cpp',
]

gui_sources = [
  './src/gui/main.cpp'
]

gtkdeps = [
  dependency( 'gtk4' ),
  dependency( 'libadwaita-1' ),
]

dbdeps = [
    dependency( 'sqlite3' )
]

#if optimization == ''
  #optimization = '2'
#endif
#
## Guess there is better way to do this
#if optimization == '0'
  #cflags = ['-O0']
#elif optimization == '1'
  #cflags = ['-O1']
#elif optimization == '2'
  #cflags = ['-O2']
#elif optimization == '3'
  #cflags = ['-O3']
#endif

ui_file = './res/gui/main.ui'
custom_target(
  command: ['glib-compile-resources', 'prefetch.xml',
    '--sourcedir=./res/gui/', '--target=ui.gresource'],
  output: 'ui.gresource'
)

executable( 'Uptimer', demon_sources, dependencies : dbdeps, include_directories: incdir )
executable( 'Gui', gui_sources, dependencies : gtkdeps, include_directories: incdir )
