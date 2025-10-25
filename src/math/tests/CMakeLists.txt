find_package(Catch2)

add_executable(bitcrackle_math_test)
target_link_libraries(bitcrackle_math_test PRIVATE Catch2::Catch2WithMain)
if(MSVC)
    # to compile catch2 tests
    target_compile_options(bitcrackle_math_test PRIVATE /wd4868)
endif()

target_sources(
    bitcrackle_math_test
    PRIVATE statistics_test.cpp add_test.cpp subtract_test.cpp
)
target_link_libraries(bitcrackle_math_test PRIVATE bit::math)
