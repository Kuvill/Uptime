cmake_minimum_required( VERSION 3.29 )
project( uptimer LANGUAGES CXX )

# find_package( sqlite3 REQUIRED )

set(CMAKE_CXX_FLAGS "-lsqlite3")

include_directories( . )
# include_directories( ${SQLite3_INCLUDE_DIR} )

add_executable( ${PROJECT_NAME} )

target_sources( ${PROJECT_NAME} PRIVATE main.cpp ./src/demon/get_uptime.cpp ./src/demon/db.cpp ./src/demon/ram_storage.cpp ./src/demon/server.cpp)
target_compile_features( ${PROJECT_NAME} PRIVATE c_std_23 )
# target_link_libraries( ${sqlite3_LIBRARIES} )
