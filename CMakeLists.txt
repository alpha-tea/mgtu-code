cmake_minimum_required(VERSION 3.5)

project(mgtu-code LANGUAGES C)

#set(CMAKE_C_FLAGS "-Wall -Wextra -O0 -S -masm=intel -std=c99")
#Для gcc, если нужно собрать для msvs - отключить.
set(CMAKE_C_FLAGS "-Wall -Wextra -O0 -std=c11")

find_package( Threads REQUIRED )

file(GLOB ALLHEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST} ${ALLHEADERS})
#target_link_libraries( mgtu-code Threads::Threads )
