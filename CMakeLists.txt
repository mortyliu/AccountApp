cmake_minimum_required(VERSION 3.16)

project(AccountApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(Qt6_DIR "D:/Software/QT/6.11.0/mingw_64/lib/cmake/Qt6")
set(QT_FORCE_ANY_CMAKE_VERSION_FOR_USING_QT TRUE)
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts Sql Svg)
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
    src/view/CategoryGridWidget.cpp
    src/view/AccountView.cpp
    src/view/TransactionView.cpp
    src/view/StatisticsView.cpp
    src/view/TransferView.cpp
    src/view/IconSelectDialog.cpp
    src/ui/MainWindow.cpp
    src/utils/CsvExporter.cpp
)

set(HEADERS
    src/model/DatabaseManager.h
    src/model/CategoryModel.h
    src/model/AccountModel.h
    src/model/TransactionModel.h
    src/model/Constants.h
    src/controller/CategoryController.h
    src/controller/AccountController.h
    src/controller/TransactionController.h
    src/controller/StatisticsController.h
    src/view/CategoryView.h
    src/view/CategoryGridWidget.h
    src/view/AccountView.h
    src/view/TransactionView.h
    src/view/StatisticsView.h
    src/view/TransferView.h
    src/view/IconSelectDialog.h
    src/ui/MainWindow.h
    src/utils/CsvExporter.h
)

qt_add_executable(AccountApp
    ${SOURCES}
    ${HEADERS}
)

qt_add_resources(AccountApp resources
    FILES
    resources/style.qss
    resources/icons/app.svg
    resources/icons/app.png
    resources/icons/branch-closed.svg
    resources/icons/branch-open.svg
    resources/icons/cat_food.svg
    resources/icons/cat_transport.svg
    resources/icons/cat_shopping.svg
    resources/icons/cat_entertainment.svg
    resources/icons/cat_medical.svg
    resources/icons/cat_education.svg
    resources/icons/cat_housing.svg
    resources/icons/cat_daily.svg
    resources/icons/cat_other_expense.svg
    resources/icons/cat_salary.svg
    resources/icons/cat_bonus.svg
    resources/icons/cat_investment.svg
    resources/icons/cat_other_income.svg
    resources/icons/cat_breakfast.svg
    resources/icons/cat_lunch.svg
    resources/icons/cat_dinner.svg
    resources/icons/cat_snack.svg
    resources/icons/cat_subway.svg
    resources/icons/cat_taxi.svg
    resources/icons/cat_gasoline.svg
    resources/icons/cat_clothing.svg
    resources/icons/cat_digital.svg
    resources/icons/cat_daily_goods.svg
    resources/icons/cat_game.svg
    resources/icons/cat_movie.svg
    resources/icons/cat_travel.svg
    resources/icons/cat_clinic.svg
    resources/icons/cat_medicine.svg
    resources/icons/cat_book.svg
    resources/icons/cat_training.svg
    resources/icons/cat_rent.svg
    resources/icons/cat_property.svg
    resources/icons/cat_utilities.svg
    resources/icons/cat_phone_bill.svg
    resources/icons/cat_haircut.svg
    resources/icons/transfer.svg
    resources/icons/food.svg
    resources/icons/shopping.svg
    resources/icons/transport.svg
    resources/icons/salary.svg
    resources/icons/redpack.svg
    resources/icons/medical.svg
    resources/icons/education.svg
    resources/icons/entertainment.svg
    resources/icons/housing.svg
    resources/icons/water.svg
    resources/icons/phone.svg
    resources/icons/finance.svg
    resources/icons/cash.svg
    resources/icons/bank.svg
    resources/icons/insurance.svg
    resources/icons/travel.svg
    resources/icons/clothes.svg
    resources/icons/beauty.svg
    resources/icons/other.svg
    PREFIX "/"
    BASE "resources"
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
    Qt6::Svg
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
