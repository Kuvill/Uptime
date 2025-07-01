cmake_minimum_required(VERSION 3.10)

project(MyProject)

set(CMAKE_CXX_STANDARD 23)

include_directories(inc/)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk4)
pkg_check_modules(SQLITE REQUIRED sqlite3)
pkg_check_modules(ADWAITA REQUIRED libadwaita-1)

include_directories(${GTK_INCLUDE_DIRS} ${SQLITE_INCLUDE_DIRS} ${ADWAITA_INCLUDE_DIRS})

add_executable(Gui ./src/time.cpp ./src/logger.cpp ./src/gui/db.cpp ./src/gui/main.cpp ./src/gui/client.cpp ./src/gui/context.cpp ./src/gui/lazy_load.cpp ./src/gui/column_view.cpp ./src/gui/record_item.cpp)

add_executable(Uptimer main.cpp ./src/logger.cpp ./src/time.cpp ./src/demon/db.cpp ./src/demon/server.cpp ./src/demon/get_uptime.cpp ./src/demon/ram_storage.cpp )


target_link_libraries(Gui ${GTK_LIBRARIES} ${SQLITE_LIBRARIES} ${ADWAITA_LIBRARIES})
target_link_libraries(Uptimer ${GTK_LIBRARIES} ${SQLITE_LIBRARIES} ${ADWAITA_LIBRARIES})
