cmake_minimum_required(VERSION 3.16)

project(SistemaTarefasQt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(SistemaTarefasQt
    src/main.cpp
    src/task.h
    src/task.cpp
    src/taskmanager.h
    src/taskmanager.cpp
    src/taskfilerepository.h
    src/taskfilerepository.cpp
    src/mainwindow.h
    src/mainwindow.cpp
)

target_link_libraries(SistemaTarefasQt PRIVATE Qt6::Widgets)
