cmake_minimum_required(VERSION 3.13)

project(Win32Redirector)

if(WIN32)
	if(MSVC)
		add_compile_options(-utf-8 )
		message(FATAL_ERROR "NOT SUPPORT MSVC")
	else()
		add_link_options(-static -static-libgcc -static-libstdc++)
	endif()
else()
message(FATAL_ERROR "ONLY SUPPORT WINDOWS")
endif()

#C++标准
set(CMAKE_CXX_STANDARD 20)


file(GLOB MAIN_CXX_C_FILES *.cpp *.c )
add_executable(Win32Redirector ${MAIN_CXX_C_FILES})
