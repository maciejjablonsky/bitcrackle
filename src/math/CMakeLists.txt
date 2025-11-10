add_library(bitcrackle_math INTERFACE)
add_library(bit::math ALIAS bitcrackle_math)

target_sources(
    bitcrackle_math
    INTERFACE
        include/math/qnumber/round.h
        include/math/qnumber.h
        include/math/utilities.h
        include/math/bits.h
        include/math/statistics.h
)

target_include_directories(bitcrackle_math INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(bitcrackle_math INTERFACE cxx_std_23)
target_compile_options(
    bitcrackle_math
    INTERFACE /Wall /permissive- /Zc:preprocessor /analyze /wd5045
    #        INTERFACE
    #        /WX
)

find_package(mp-units REQUIRED)
find_package(gcem REQUIRED)
target_link_libraries(bitcrackle_math INTERFACE mp-units::mp-units gcem)

add_subdirectory(tests)
