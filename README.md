ToDoList
========

Simple Windows console ToDo list application (single-file C++).

## Build (Windows)

**Recommended: using g++ (MinGW/MinGW-w64)**

```powershell
g++ -std=c++17 -O2 -o ToDoList.exe ToDoList.cpp
```

The built `ToDoList.exe` will be in the current directory.

**Alternative: using CMake**

```powershell
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
cmake --build build --config Release
```

Note: On Windows, CMake may default to MSVC; use `-DCMAKE_CXX_COMPILER=g++` to ensure g++ is used.

## Notes

- This program uses the Windows console API (`windows.h`) and is intended for Windows.
- The program writes tasks to `tasks.txt` and `temp.txt` in the working directory.

## GitHub Releases / CI

- A GitHub Actions workflow is included in `.github/workflows/windows-build.yml` to build and upload artifacts.
- To publish a release: see `RELEASE.md` for instructions.
- Prebuilt binaries are available on the GitHub Releases page.

## How to share

1. Push your code to GitHub: `git remote add origin <repo-url>` and `git push -u origin main`.
2. GitHub Actions will automatically build on every push.
3. Create a GitHub Release and attach the binary, or tag a commit to trigger the release workflow.
