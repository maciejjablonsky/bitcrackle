cmake_minimum_required(VERSION 3.15)
project(bitcrackle CXX)


find_package(Qt6 COMPONENTS Core Gui Qml Quick REQUIRED)
qt_policy(SET QTP0001 NEW)
set(CMAKE_AUTOMOC ON)

find_package(fmt REQUIRED)
find_package(boost REQUIRED)

add_executable(bitcrackle 
	src/main.cpp
)

target_sources(bitcrackle
    PRIVATE
        src/midi_device_model.h
        src/midi_device_model.cpp
        src/device_change_event_filter.h
        src/device_change_event_filter.cpp
)


target_link_libraries(bitcrackle
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        winmm
        fmt::fmt
        boost::boost
)

target_compile_definitions(bitcrackle PRIVATE UNICODE _UNICODE)

set_source_files_properties(qml/Sizing.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
set_source_files_properties(qml/Colors.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(bitcrackle
    URI Bitcrackle
    QML_FILES
        qml/Main.qml
        qml/Sizing.qml
        qml/Colors.qml
)

install(TARGETS bitcrackle DESTINATION "."
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        )
