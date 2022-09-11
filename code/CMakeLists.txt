cmake_minimum_required(VERSION 3.20)
project(llvm-ir CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/conan.cmake)

conan_cmake_configure(REQUIRES 
                        fmt/8.0.0
                        catch2/3.1.0
                        llvm-core/13.0.0
                      GENERATORS 
                        cmake_find_package 
                        cmake
                        cmake_paths
                      )

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings}
                    )
 
find_package(fmt REQUIRED)
find_package(Catch2 REQUIRED)

add_subdirectory(tests)
add_subdirectory(src)