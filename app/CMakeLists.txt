
cmake_minimum_required(VERSION 3.4.1)

add_library( 
        native-lib

        SHARED
        src/main/cpp/dgif_lib.c
        src/main/cpp/gifalloc.c
        src/main/cpp/native-lib.cpp)

find_library(
        jnigraphics-lib
        jnigraphics )

find_library( 
        log-lib
        
        log)

target_link_libraries( 
        native-lib
        ${jnigraphics-lib}
        ${log-lib})