cmake_minimum_required(VERSION 3.16)

project(AccountApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(Qt6_DIR "D:/Software/QT/6.11.0/mingw_64/lib/cmake/Qt6")
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts Sql)
qt_standard_project_setup()

set(SOURCES
    src/main.cpp
    src/model/DatabaseManager.cpp
    src/model/CategoryModel.cpp
    src/model/AccountModel.cpp
    src/model/TransactionModel.cpp
    src/controller/CategoryController.cpp
    src/controller/AccountController.cpp
    src/controller/TransactionController.cpp
    src/controller/StatisticsController.cpp
    src/view/CategoryView.cpp
    src/view/AccountView.cpp
    src/view/TransactionView.cpp
    src/view/StatisticsView.cpp
    src/view/TransferView.cpp
    src/ui/MainWindow.cpp
    src/utils/CsvExporter.cpp
)

set(HEADERS
    src/model/DatabaseManager.h
    src/model/CategoryModel.h
    src/model/AccountModel.h
    src/model/TransactionModel.h
    src/controller/CategoryController.h
    src/controller/AccountController.h
    src/controller/TransactionController.h
    src/controller/StatisticsController.h
    src/view/CategoryView.h
    src/view/AccountView.h
    src/view/TransactionView.h
    src/view/StatisticsView.h
    src/view/TransferView.h
    src/ui/MainWindow.h
    src/utils/CsvExporter.h
)

set(RESOURCES
    resources/resources.qrc
)

qt_add_executable(AccountApp
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

qt_add_resources(AccountApp "resources"
    PREFIX "/"
    FILES ${RESOURCES}
)

target_compile_options(AccountApp PRIVATE
    -finput-charset=UTF-8
    -fexec-charset=UTF-8
    -DUNICODE
    -D_UNICODE
)

if(WIN32)
    target_compile_definitions(AccountApp PRIVATE
        WIN32_LEAN_AND_MEAN
        _WIN32_WINNT=0x0601
    )
endif()

target_link_libraries(AccountApp PRIVATE
    Qt6::Widgets
    Qt6::Charts
    Qt6::Sql
)

if(WIN32)
    set_target_properties(AccountApp PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

install(TARGETS AccountApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
