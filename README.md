[![build & test (clang, gcc, MSVC)](https://github.com/kamchatka-volcano/figcone_ini/actions/workflows/build_and_test.yml/badge.svg?branch=master)](https://github.com/kamchatka-volcano/figcone_ini/actions/workflows/build_and_test.yml)

**figcone_ini** - is a [`inifile-cpp`](https://github.com/Rookfighter/inifile-cpp) configuration parser adapter for [`figcone`](https://github.com/kamchatka-volcano/figcone) library. 


## Installation
Download and link the library from your project's CMakeLists.txt:
```
cmake_minimum_required(VERSION 3.14)

include(FetchContent)
FetchContent_Declare(figcone_ini
    GIT_REPOSITORY "https://github.com/kamchatka-volcano/figcone_ini.git"
    GIT_TAG "origin/master"
)
#uncomment if you need to install figcone_ini with your target
#set(INSTALL_FIGCONE_INI ON)
FetchContent_MakeAvailable(figcone_ini)

add_executable(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE figcone::figcone_ini)
```

For the system-wide installation use these commands:
```
git clone https://github.com/kamchatka-volcano/figcone_ini.git
cd figcone_ini
cmake -S . -B build
cmake --build build
cmake --install build
```

Afterwards, you can use find_package() command to make the installed library available inside your project:
```
find_package(figcone_ini 0.9.0 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE figcone::figcone_ini)
```

## Running tests
```
cd figcone_ini
cmake -S . -B build -DENABLE_TESTS=ON
cmake --build build
cd build/tests && ctest
```

## License
**figcone_ini** is licensed under the [MS-PL license](/LICENSE.md)  
