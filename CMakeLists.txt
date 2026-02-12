cmake_minimum_required(VERSION 3.10)
project(contactManager CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ContactCore STATIC
	src/App.cpp
	src/DataManager.cpp
	src/FileManager.cpp
	src/Service.cpp
	src/UserInterface.cpp
)

target_include_directories(ContactCore PUBLIC include src)

add_executable(ContactManager
	src/main.cpp
)

target_link_libraries(ContactManager PRIVATE ContactCore)