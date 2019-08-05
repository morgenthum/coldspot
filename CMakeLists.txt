cmake_minimum_required(VERSION 2.8.4)

# Project
project(coldspot)

# ColdSpot JVM
file(GLOB_RECURSE SRC_JVM "src/jvm/*.c*")
add_library(jvm SHARED ${SRC_JVM})
set_target_properties(jvm PROPERTIES COMPILE_FLAGS "-std=c++1y")
if (APPLE)
    set_target_properties(jvm PROPERTIES LINK_FLAGS "-compatibility_version 1.0.0")
    add_custom_command(TARGET jvm POST_BUILD
            COMMAND ${CMAKE_INSTALL_NAME_TOOL} -id "@rpath/libjvm.dylib"
            $<TARGET_FILE:jvm>)
endif ()
target_include_directories(jvm PUBLIC
        "src"
        "/usr/local/include"
        "/usr/local/lib/libffi-3.2/include"
        "/usr/local/lib/libzip/include")
find_library(FFI_LIBRARY ffi)
find_library(ZIP_LIBRARY zip)
target_link_libraries(jvm ${FFI_LIBRARY} ${ZIP_LIBRARY})

# Wrapper-executable for GNU Classpath
file(GLOB_RECURSE SRC_JAVA "src/java/*.c*")
add_executable(java ${SRC_JAVA})
set_target_properties(java PROPERTIES COMPILE_FLAGS "-std=c++1y")
target_include_directories(java PUBLIC "src")
target_link_libraries(java jvm)

# Tests
file(GLOB_RECURSE SRC_TESTS "src-test/*.c*")
add_executable(tests ${SRC_TESTS})
set_target_properties(tests PROPERTIES COMPILE_FLAGS "-std=gnu++11")
if (CYGWIN)
    #set_target_properties(tests PROPERTIES LINK_FLAGS "-static-libstdc++ -static-libgcc")
endif ()
target_link_libraries(tests jvm "/usr/local/lib/libgtest.a" "/usr/local/lib/libgtest_main.a")

# OS X: install_name_tool -id @rpath/libjvm.dylib libjvm.dylib