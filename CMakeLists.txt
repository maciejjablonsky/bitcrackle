cmake_minimum_required(VERSION 3.15)
project(bitcrackle CXX)

add_executable(bitcrackle src/main.cpp)

install(TARGETS bitcrackle DESTINATION "."
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        )
