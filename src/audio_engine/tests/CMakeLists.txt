find_package(Catch2)

add_executable(bitcrackle_audio_engine_test)
target_link_libraries(bitcrackle_audio_engine_test PRIVATE Catch2::Catch2WithMain)
if(MSVC)
    # to compile catch2 tests
    target_compile_options(bitcrackle_audio_engine_test PRIVATE /wd4868)
endif()

target_sources(bitcrackle_audio_engine_test PRIVATE ring_buffer_test.cpp)
target_link_libraries(bitcrackle_audio_engine_test PRIVATE bitcrackle::audio_engine)
