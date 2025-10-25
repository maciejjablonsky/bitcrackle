cmake_minimum_required(VERSION 3.15)
project(bitcrackle CXX)

set(CMAKE_LINK_LIBRARY_ONLY_TARGETS ON)

find_package(Qt6 COMPONENTS Core Gui Qml Quick REQUIRED)
qt_policy(SET QTP0001 NEW)
#set(CMAKE_AUTOMOC ON)

add_subdirectory(src)

#set_source_files_properties(
#    qml/Bitcrackle/Sizing.qml
#    PROPERTIES QT_QML_SINGLETON_TYPE TRUE
#)
#set_source_files_properties(
#    qml/Bitcrackle/Colors.qml
#    PROPERTIES QT_QML_SINGLETON_TYPE TRUE
#)
#
#set_target_properties(bitcrackle PROPERTIES AUTOMOC ON)
#qt_add_qml_module(bitcrackle
#    URI Bitcrackle
#    QML_FILES
#        qml/Bitcrackle/Main.qml
#        qml/Bitcrackle/Sizing.qml
#        qml/Bitcrackle/Colors.qml
#    RESOURCES
#        qml/Bitcrackle/waves.png
#        qml/Bitcrackle/robot.png
#)

#install(TARGETS bitcrackle DESTINATION "."
#        RUNTIME DESTINATION bin
#        ARCHIVE DESTINATION lib
#        LIBRARY DESTINATION lib
#        )

#add_subdirectory(tests)
