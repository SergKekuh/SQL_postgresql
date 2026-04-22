cmake_minimum_required(VERSION 3.16)
project(UTSK_Intelligent_Sales VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build unit tests" OFF)
option(BUILD_GUI "Build Qt GUI version" OFF)

find_package(nlohmann_json REQUIRED)
find_package(libpqxx REQUIRED)

add_executable(utsk_console
    src/main_console.cpp
    src/core/Database.cpp
    src/core/Config.cpp
    src/core/Logger.cpp
    src/models/Client.cpp
    src/models/Product.cpp
    src/models/Document.cpp
    src/models/SaleLine.cpp
    src/services/ClientService.cpp
    src/services/ProductService.cpp
    src/services/AnalyticsService.cpp
    src/ui/ConsoleUI.cpp
    src/ui/ConsoleMenu.cpp
    src/ui/TablePrinter.cpp
)

target_include_directories(utsk_console PRIVATE include)
target_link_libraries(utsk_console pqxx nlohmann_json::nlohmann_json)

if(BUILD_GUI)
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql Charts)
    qt_standard_project_setup()
    
    add_executable(utsk_gui
        src/main_gui.cpp
        src/core/Database.cpp
        src/core/Config.cpp
        src/core/Logger.cpp
        src/models/Client.cpp
        src/models/Product.cpp
        src/services/ClientService.cpp
        src/services/RecommendationService.cpp
        src/ui/MainWindow.cpp
        src/ui/ClientDialog.cpp
        src/ui/DashboardWidget.cpp
        src/ui/RecommendationsWidget.cpp
        src/ui/ChartsWidget.cpp
    )
    
    target_include_directories(utsk_gui PRIVATE include)
    target_link_libraries(utsk_gui Qt6::Core Qt6::Widgets Qt6::Sql Qt6::Charts pqxx nlohmann_json::nlohmann_json)
endif()

if(BUILD_TESTS)
    find_package(GTest REQUIRED)
    enable_testing()
    add_subdirectory(tests)
endif()

message(STATUS "UTSK Intelligent Sales v1.0.0")
