add_library(bitcrackle_audio_engine STATIC)
add_library(bitcrackle::audio_engine ALIAS bitcrackle_audio_engine)
target_include_directories(bitcrackle_audio_engine PUBLIC include PRIVATE src)
target_sources(
    bitcrackle_audio_engine
    PRIVATE include/audio_engine/engine.h include/audio_engine/ring_buffer.h src/engine.cpp
)

add_subdirectory(tests)
