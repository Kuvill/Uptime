cmake_minimum_required( VERSION 3.29 )
project( uptimer LANGUAGES CXX )

include(ExternalProject)

ExternalProject_Add(
	GtkChart
	GIT_REPOSITORY https://github.com/lundmar/gtkchart.git
	GIT_TAG master
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX="./third_party/"
)

# find_package( sqlite3 REQUIRED )

set(CMAKE_CXX_FLAGS "-lsqlite3")

include_directories( . )
include_directories(./third_party/GtkChart/include)
# include_directories( ${SQLite3_INCLUDE_DIR} )

add_executable( ${PROJECT_NAME} )

target_sources( ${PROJECT_NAME} PRIVATE main.cpp ./src/demon/get_uptime.cpp ./src/demon/db.cpp ./src/demon/ram_storage.cpp ./src/demon/server.cpp ./src/logger.cpp )
target_compile_features( ${PROJECT_NAME} PRIVATE c_std_23 )

target_link_libraries(${PROJECT_NAME} GtkChart)
# target_link_libraries( ${sqlite3_LIBRARIES} )
