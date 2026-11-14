# SCAMPS
Smart Clinic Appointment & Patient Management System

## Desktop dashboard

The desktop app uses Qt 6 Widgets and C++17. Its dashboard is the startup screen and includes patient and appointment summary cards, custom-painted doughnut charts, age distribution bars, a daily activity summary, and today's appointments. Sidebar pages expose the patient, appointment, waiting-queue, search/sort, and report workflows using the existing `ClinicManager`, `ClinicDatabase`, and `SearchEngine` classes.

The dashboard starts with ten sample patients and fourteen sample appointments so the charts are populated on first launch. These records are in-memory demo data; the repository's file persistence utility is not implemented or connected yet.

## Build

Open this folder in Qt Creator with a Qt 6 Widgets desktop kit (C++17), or build with CMake:

```sh
cmake -S . -B build
cmake --build build
```
