cmake_minimum_required(VERSION 3.16)

project(HDNap VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 COMPONENTS Quick REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Charts)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Concurrent)


find_package(pylon 7.1.0 REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)

include_directories(headers/)

include_directories(/opt/pylon/include/)
link_directories(/opt/pylon/lib/)

qt_add_executable(appHDNap
    headers/global.h
    headers/camera.h
    headers/devicepropertymodel.h
    headers/acquirecamera.h
    headers/imagebuffer.h
    headers/liveimageconsumer.h
    headers/datacard.h
    headers/acquiredatacard.h
    headers/livedataconsumer.h
    headers/hitbuffer.h
    headers/kdcmotor.h
    headers/mirrorcontrol.h
#    headers/controller.h
headers/filevalidator.h src/filevalidator.cpp

    src/hitbuffer.cpp
    src/main.cpp
    src/camera.cpp
    src/devicepropertymodel.cpp
    src/acquirecamera.cpp
    src/imagebuffer.cpp
    src/liveimageconsumer.cpp
    src/datacard.cpp
    src/acquiredatacard.cpp
    src/livedataconsumer.cpp
    src/kdcmotor.cpp
    src/mirrorcontrol.cpp
#    src/controller.cpp
)

qt_add_qml_module(appHDNap
    URI HDNap
    VERSION 1.0
    QML_FILES
    ui/main.qml
    ui/DboxSlider.qml
    ui/CameraControl.qml
    ui/DatacardControl.qml
    ui/ControlPanelWindow.qml
    ui/ViewWindow.qml
    ui/FileBox.qml
    ui/ZChartView.qml
    ui/MirrorControl.qml
)

set_target_properties(appHDNap PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appHDNap
    PRIVATE Qt6::Quick
    Qt6::Charts
    Qt6::Core
    Qt6::Concurrent
    Eigen3::Eigen
    pylon::pylon
    AqMD3
)

install(TARGETS appHDNap
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
