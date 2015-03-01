cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)
cmake_policy(VERSION 2.8.11)

project(glcapsviewer)
set(NAME glcapsviewer)

set(CMAKE_INCLUDE_CURRENT_DIR OFF)
set(CMAKE_AUTOMOC ON)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-std=c++11)
add_definitions(-std=c++0x)

include_directories("." "GeneratedFiles")

file(GLOB SOURCE *.cpp)
file(GLOB HEADER *.h)

#Qt
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Widgets REQUIRED)
file (GLOB Resources *.qrc)
QT5_ADD_RESOURCES(ResourceSources ${Resources})

#Gl
find_package(OpenGL REQUIRED)

#Glew
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})
link_directories(${GLEW_LIBRARIES})

#Glfw
find_path(GLFW_INCLUDE_DIR GLFW/glfw3.h)
find_library(GLFW_LIBRARY NAMES glfw3 glfw)

set(GLFW_INCLUDE_DIRS ${GLFW_INCLUDE_DIR})
set(GLFW_LIBRARIES ${GLFW_LIBRARY})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(GLFW
	REQUIRED_VARS GLFW_INCLUDE_DIR GLFW_LIBRARY)

if(GLFW_FOUND AND NOT TARGET GLFW::GLFW)
	add_library(GLFW::GLFW UNKNOWN IMPORTED)
	set_target_properties(GLFW::GLFW PROPERTIES
		IMPORTED_LOCATION "${GLFW_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES "${GLFW_INCLUDE_DIRS}")
endif()

mark_as_advanced(GLFW_INCLUDE_DIR GLFW_LIBRARY)

#Boost
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
add_definitions( -DBOOST_ALL_DYN_LINK )

# Compiler specific stuff
IF(MSVC)
    SET(CMAKE_CXX_FLAGS "/EHsc")
ENDIF(MSVC)

# Unicode
add_definitions(-DUNICODE -D_UNICODE)


add_executable(${NAME} WIN32
${SOURCE}
${HEADER}
${ResourceSources})

target_link_libraries(${NAME} Qt5::Core)	
target_link_libraries(${NAME} Qt5::Gui)	
target_link_libraries(${NAME} Qt5::Network)	
target_link_libraries(${NAME} Qt5::Widgets)
target_link_libraries(${NAME} ${GLEW_LIBRARIES})
target_link_libraries(${NAME} ${OPENGL_LIBRARIES})
target_link_libraries(${NAME} ${Boost_LIBRARIES})
target_link_libraries(${NAME} glfw ${GLFW_LIBRARY})

