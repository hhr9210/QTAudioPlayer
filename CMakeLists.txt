cmake_minimum_required(VERSION 3.16)

project(mp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 1. 统一查找所需的模块 ---
find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets LinguistTools)

set(TS_FILES mp_zh_CN.ts)

# Core modules
set(CORE_SOURCES
        core/audioplayer.cpp
        core/audioplayer.h
        core/localmusicmanager.cpp
        core/localmusicmanager.h
        core/musicapimanager.cpp
        core/musicapimanager.h
        core/lrcparser.cpp
        core/lrcparser.h
)

# Pages
set(PAGES_SOURCES
        pages/discoverypage.cpp
        pages/discoverypage.h
        pages/localmusicpage.cpp
        pages/localmusicpage.h
        pages/mvpage.cpp
        pages/mvpage.h
        pages/favoritepage.cpp
        pages/favoritepage.h
        pages/playlistpage.cpp
        pages/playlistpage.h
        pages/dailyrecommendpage.cpp
        pages/dailyrecommendpage.h
        pages/rankingpage.cpp
        pages/rankingpage.h
        pages/playerdetailpage.cpp
        pages/playerdetailpage.h
        pages/searchresultpage.cpp
        pages/searchresultpage.h
)

# Widgets
set(WIDGETS_SOURCES
        widgets/headerwidget.cpp
        widgets/headerwidget.h
        widgets/footerwidget.cpp
        widgets/footerwidget.h
        widgets/menuwidget.cpp
        widgets/menuwidget.h
        widgets/scrollinglabel.cpp
        widgets/scrollinglabel.h
        widgets/vinylwidget.cpp
        widgets/vinylwidget.h
        widgets/playlistwidget.cpp
        widgets/playlistwidget.h
        widgets/lyricswidget.cpp
        widgets/lyricswidget.h
        widgets/videoplayerwidget.cpp
        widgets/videoplayerwidget.h
)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        ${CORE_SOURCES}
        ${PAGES_SOURCES}
        ${WIDGETS_SOURCES}
        ${TS_FILES}
)

# --- 2. 创建可执行程序 ---
qt_add_executable(mp
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

# 生成翻译文件
qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

# --- 3. 链接库 (核心修改：确保 Multimedia 被包含) ---
target_link_libraries(mp PRIVATE
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::Network
)

# 窗口属性设置
set_target_properties(mp PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS mp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 结束构建配置
qt_finalize_executable(mp)
