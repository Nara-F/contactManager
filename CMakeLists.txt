cmake_minimum_required(VERSION 3.10)
project(contactManager CXX)

add_library(ContactCore STATIC
	src/App.cpp
	src/DataManager.cpp
	src/FileManager.cpp
	src/Service.cpp
	src/UserInterface.cpp
)

target_compile_features(ContactCore PUBLIC cxx_std_17)
target_include_directories(ContactCore PUBLIC include)
 
add_executable(ContactManager
	src/main.cpp
)

target_link_libraries(ContactManager PRIVATE ContactCore)

add_custom_command(TARGET ContactManager POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/src/contactList
		$<TARGET_FILE_DIR:ContactManager>/contactList
)
