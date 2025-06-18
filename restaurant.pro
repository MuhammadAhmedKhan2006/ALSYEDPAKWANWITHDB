QT += core gui widgets sql printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    employeesalary.cpp \
    inventory.cpp \
    ownerdashboard.cpp \
    salesmandashboard.cpp \
    mainwindow.cpp

HEADERS += \
    employeesalary.h \
    inventory.h \
    ownerdashboard.h \
    salesmandashboard.h \
    mainwindow.h

TARGET = AlSyedPakwan
TEMPLATE = app
