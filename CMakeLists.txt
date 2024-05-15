cmake_minimum_required(VERSION 3.5)

project(Password_Manager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        Resources.rc
        Resources.qrc
        dbsetup.h dbsetup.cpp
        helpers.h helpers.cpp
        edittagdialog.h edittagdialog.cpp edittagdialog.ui
        newpassworddialog.h newpassworddialog.cpp newpassworddialog.ui
        contextMenu.cpp
        regfunctions.h regfunctions.cpp
        QR/QrCode.cpp
        QR/QrCode.hpp
        QR/QrToPng.cpp
        QR/QrToPng.h
        QR/TinyPngOut.cpp
        QR/TinyPngOut.hpp
        TOTP/TOTPGEN.cpp
        AES256/AES256.cpp SHA1/SHA1.cpp
        newseeddialog.h newseeddialog.cpp newseeddialog.ui
        childlineedit.h childlineedit.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Password_Manager
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Password_Manager APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Password_Manager SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Password_Manager
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(Password_Manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/SQLite)
target_include_directories(Password_Manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/TOTP)
target_link_libraries(Password_Manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/SQLite/libsqlite3mc_static.a)
target_link_libraries(Password_Manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/TOTP/libcrypto.lib)
target_link_libraries(Password_Manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/TOTP/libssl.lib)
target_link_libraries(Password_Manager PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(Password_Manager PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Password_Manager)
endif()
