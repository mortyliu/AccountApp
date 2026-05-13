QT       += core gui widgets charts sql

TARGET   = AccountApp
TEMPLATE = app

SOURCES += src/main.cpp \
           src/model/DatabaseManager.cpp \
           src/model/CategoryModel.cpp \
           src/model/AccountModel.cpp \
           src/model/TransactionModel.cpp \
           src/controller/CategoryController.cpp \
           src/controller/AccountController.cpp \
           src/controller/TransactionController.cpp \
           src/controller/StatisticsController.cpp \
           src/view/CategoryView.cpp \
           src/view/AccountView.cpp \
           src/view/TransactionView.cpp \
           src/view/StatisticsView.cpp \
           src/view/TransferView.cpp \
           src/ui/MainWindow.cpp \
           src/utils/CsvExporter.cpp

HEADERS += src/model/DatabaseManager.h \
           src/model/CategoryModel.h \
           src/model/AccountModel.h \
           src/model/TransactionModel.h \
           src/controller/CategoryController.h \
           src/controller/AccountController.h \
           src/controller/TransactionController.h \
           src/controller/StatisticsController.h \
           src/view/CategoryView.h \
           src/view/AccountView.h \
           src/view/TransactionView.h \
           src/view/StatisticsView.h \
           src/view/TransferView.h \
           src/ui/MainWindow.h \
           src/utils/CsvExporter.h

RESOURCES += resources/resources.qrc

QMAKE_CXXFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
