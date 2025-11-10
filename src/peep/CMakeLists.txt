add_library(bitcrackle_peep STATIC)
add_library(bit::peep ALIAS bitcrackle_peep)

target_sources(bitcrackle_peep PRIVATE include/peep/peeper.h src/peeper.cpp)

target_include_directories(bitcrackle_peep PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(
    bitcrackle_peep
    PRIVATE Qt6::Core Qt6::Gui
    #            Qt6::Qml
    #            Qt6::Quick
)
