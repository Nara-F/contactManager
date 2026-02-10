cmake_minimum_required(VERSION 3.10)
project(contactManager CXX)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(./src SRC_LIST)
add_executable(ContactManager ${SRC_LIST})

target_include_directories(ContactManager PRIVATE src include)