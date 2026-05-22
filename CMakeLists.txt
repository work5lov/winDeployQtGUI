cmake_minimum_required(VERSION 3.21)

project(winDeployQtGUI VERSION 0.2 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Целевая версия Qt: 6.8.3+
find_package(Qt6 REQUIRED COMPONENTS Core Quick)

qt_standard_project_setup()

set(PROJECT_SOURCES
        QtFolderScanner.h QtFolderScanner.cpp
        DeploymentManager.h DeploymentManager.cpp
        Constants.h
        main.cpp
        qml.qrc
)

qt_add_executable(winDeployQtGUI
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_link_libraries(winDeployQtGUI
    PRIVATE Qt6::Core Qt6::Quick
)

set_target_properties(winDeployQtGUI PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.winDeployQtGUI
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS winDeployQtGUI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

qt_import_qml_plugins(winDeployQtGUI)
qt_finalize_executable(winDeployQtGUI)

# ============================================================
# CPack — создание инсталлятора (NSIS)
# ============================================================
set(CPACK_PACKAGE_NAME "winDeployQtGUI")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "OpenSoft")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "GUI wrapper for windeployqt")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-windows")

# Генератор: NSIS (Windows) или ZIP (универсальный)
if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "winDeployQtGUI")
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    set(CPACK_NSIS_CREATE_ICONS_EXTRA
        "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\winDeployQtGUI.lnk' '$INSTDIR\\\\bin\\\\winDeployQtGUI.exe'")
else()
    set(CPACK_GENERATOR "ZIP")
endif()

include(CPack)

# ============================================================
# Тесты
# ============================================================
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
