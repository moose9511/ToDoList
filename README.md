ToDoList
========

Simple Windows console ToDo list application (single-file C++).

## Quick Start (No Compilation Needed)

**Download pre-built executable:**

Go to [Releases](https://github.com/moose9511/ToDoList/releases) and download `ToDoList.exe`. Run it directly—no installation or compilation required.

After downloading ToDoList.exe, right-click it and:
1. Select "Properties"
2. Check "Unblock" at the bottom
3. Click "Apply" → "OK"
OR use PowerShell:
Unblock-File -Path C:\path\to\ToDoList.exe

## Build from Source (Optional)

**Using g++ (MinGW/MinGW-w64)**

```powershell
g++ -std=c++17 -O2 -o ToDoList.exe ToDoList.cpp
```

The built `ToDoList.exe` will be in the current directory.

**Using CMake**

```powershell
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
cmake --build build --config Release
```

## Notes

- This program uses the Windows console API (`windows.h`) and is intended for Windows.
- The program writes tasks to `tasks.txt` and `temp.txt` in the working directory.

## About the Project

- Source code is in `ToDoList.cpp` (single file, ~470 lines)
- CI/CD: GitHub Actions automatically builds on every push and creates releases
- Released under the MIT License
