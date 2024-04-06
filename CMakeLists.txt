# CMakeLists.txt --- CMake project settings
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 3.0)

# project name and languages
project(RadioListView CXX RC)

##############################################################################

# RadioListView.exe
add_executable(RadioListView WIN32 main.cpp main_res.rc)
target_compile_definitions(RadioListView PRIVATE UNICODE _UNICODE)
target_link_libraries(RadioListView PRIVATE comctl32 shlwapi)

##############################################################################
