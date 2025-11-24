find_package(fmt REQUIRED)

add_library(bit_wave src/reader.cpp src/writer.cpp)
add_library(bitcrackle::wave ALIAS bit_wave)
target_include_directories(bit_wave PUBLIC include)
target_link_libraries(bit_wave PRIVATE fmt::fmt)
