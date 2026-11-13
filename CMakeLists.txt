cmake_minimum_required(VERSION 3.16)
project(SCAMPS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SOURCES
    main.cpp
    model/PatientRecord.cpp
    model/GeneralPatient.cpp
    model/EmergencyPatient.cpp
    model/FollowUpPatient.cpp
    model/Appointment.cpp
    model/ClinicDatabase.cpp
    controller/ClinicManager.cpp
    controller/SearchEngine.cpp
    gui/MainWindow.cpp
)

set(HEADERS
    model/PatientRecord.h
    model/GeneralPatient.h
    model/EmergencyPatient.h
    model/FollowUpPatient.h
    model/Appointment.h
    model/ClinicDatabase.h
    controller/ClinicManager.h
    controller/SearchEngine.h
    gui/MainWindow.h
    utils/ClinicException.h
)

add_executable(SCAMPS ${SOURCES} ${HEADERS})

# Tells the compiler where to find headers inside subfolders
target_include_directories(SCAMPS PRIVATE
    .
    model
    controller
    gui
    utils
)

target_link_libraries(SCAMPS PRIVATE Qt6::Widgets)