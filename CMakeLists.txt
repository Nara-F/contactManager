cmake_minimum_required(VERSION 3.10)
project(contactManager CXX)

find_package(SQLite3 REQUIRED)

add_library(ContactCore STATIC
	src/App.cpp
	src/DataManager.cpp
	src/FileManager.cpp
	src/Service.cpp
	src/repository/SqliteContactRepository.cpp
	src/UserInterface.cpp
)

target_compile_features(ContactCore PUBLIC cxx_std_17)
target_include_directories(ContactCore PUBLIC include)
target_link_libraries(ContactCore PUBLIC SQLite::SQLite3)
 
add_executable(ContactManager
	src/main.cpp
)

target_link_libraries(ContactManager PRIVATE ContactCore)

add_custom_command(TARGET ContactManager POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/src/contactList
		$<TARGET_FILE_DIR:ContactManager>/contactList
)
