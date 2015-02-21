cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)
cmake_policy(VERSION 2.8.11)

project(glcapsviewer)
set(NAME glcapsviewer)

set(CMAKE_INCLUDE_CURRENT_DIR OFF)
set(CMAKE_AUTOMOC ON)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-std=c++11)

include_directories("." "GeneratedFiles")

file(GLOB SOURCE *.cpp)
file(GLOB HEADER *.h)

#Required packages

IF(UNIX)
	find_package(PkgConfig REQUIRED)
	find_package (Threads)
	find_package(X11 REQUIRED)
	pkg_search_module(GLFW REQUIRED glfw3)
ENDIF()

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
include_directories(${GLFW_INCLUDE_DIR})
link_directories(${GLFW_LIBRARIES})

#Boost
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
add_definitions( -DBOOST_ALL_DYN_LINK )

#RapidXml
include_directories(${RAPIDXML_INCLUDE_DIR})

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

