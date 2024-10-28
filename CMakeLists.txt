cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PICO_ENABLE_CXX_EXCEPTIONS 1)

# Name of the project (will be the name of the plugin)
project(tkrzw-node)

# Build a shared library named after the project from the files in `src/`
set(SOURCE_FILES 
    ${CMAKE_SOURCE_DIR}/src/driver.cpp
    ${CMAKE_SOURCE_DIR}/src/config_parser.cpp
    ${CMAKE_SOURCE_DIR}/src/dbm_async_worker.cpp
)

if( NOT (EXISTS "${CMAKE_SOURCE_DIR}/lib/libtkrzw.a") )
    message(STATUS "Couldn't find Tkrzw library under ${CMAKE_SOURCE_DIR}/lib/! Let's build the library...")

    execute_process(COMMAND "chmod" "+x" "dependency-builder.sh"
			WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/"
			COMMAND_ECHO STDOUT)
			
	execute_process(COMMAND "./dependency-builder.sh"
			WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/"
			COMMAND_ECHO STDOUT)
else()
    message(STATUS "Found Tkrzw library under ${CMAKE_SOURCE_DIR}/lib/! proceeding...")
endif()

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${TKRZW_HEADERS} ${CMAKE_JS_SRC})

# Gives our library file a .node extension without any "lib" prefix
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# Essential include files to build a node addon,
# You should add this line in every CMake.js based project
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC} ${CMAKE_SOURCE_DIR}/lib/include)

target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib)

# Essential library files to link to a node addon
# You should add this line in every CMake.js based project
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} libtkrzw.a atomic pthread lz4)