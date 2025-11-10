#add_subdirectory(peep)
add_subdirectory(math)
add_subdirectory(wave)

#find_package(fmt REQUIRED)
#find_package(boost REQUIRED)

#add_executable(bitcrackle main.cpp)
#
#target_sources(
#    bitcrackle
#    PRIVATE
#        FILE_SET CXX_MODULES FILES utilities.ixx
#        midi_device_model.h
#        midi_device_model.cpp
#        device_change_event_filter.h
#        device_change_event_filter.cpp
#        qml_url_interceptor.h
#        qml_url_interceptor.cpp
#        qml_hotreload.h
#        qml_hotreload.cpp
#)
#
#target_include_directories(bitcrackle PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
#
#target_compile_definitions(
#    bitcrackle
#    PUBLIC
#        $<$<CONFIG:Debug>:
#        BITCRACKLE_COMPONENTS_MODULE=\"Bitcrackle/Components\"
#        BITCRACKLE_COMPONENTS_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/qml/Bitcrackle\"
#        >
#)
#
#target_link_libraries(
#    bitcrackle
#    PRIVATE
#        Qt6::Core
#        Qt6::Gui
#        Qt6::Qml
#        Qt6::Quick
#        winmm
#        fmt::fmt
#        boost::boost
#        bit::math
#)
#
#target_compile_definitions(bitcrackle PRIVATE UNICODE _UNICODE)
