# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
# based at <https://iamsorush.com/posts/cpp-cmake-build/>

cmake_minimum_required (VERSION 3.24)
set(CMAKE_VERBOSE_MAKEFILE TRUE)

project("MovieCatalog"
	DESCRIPTION "Movies Catalog using wxWidgets-3.2.4 and SQLite3"
	VERSION 3.0.1.0
	LANGUAGES CXX
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(MSVC)
	add_executable(${PROJECT_NAME} WIN32)
else()
	add_executable(${PROJECT_NAME})
endif()

if(UNIX)
	# see here: https://github.com/PointCloudLibrary/pcl/issues/3680
	# when this is fixed, we can remove the following 3 lines.
	if(NOT DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
		set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
	endif()
	find_package(GTK3 REQUIRED gtk)
endif()

find_package(wxWidgets REQUIRED) # COMPONENTS 
find_package(SQLite3 REQUIRED)
# Include the wxWidgets use file to initialize various settings.
include(${wxWidgets_USE_FILE})

if(UNIX)
	if(GTK3_FOUND)
		message(STATUS "GTK3 found, version " ${GTK3_VERSION})
	else()
		message(STATUS "GTK3 NOT found !!")
	endif()
	include_directories(${GTK3_INCLUDE_DIRS})
	target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}" "/usr/include/harfbuzz/" ${GTK3_INCLUDE_DIRS})
endif()

# Include sub-projects.
add_subdirectory("lkControls")
add_subdirectory("lkSQLite3")
add_subdirectory("MoviesCatalogs")

if(UNIX)
	target_link_libraries(${PROJECT_NAME} PRIVATE ${wxWidgets_LIBRARIES} ${SQLite3_LIBRARIES} ${GTK3_LIBRARIES})
else()
	target_link_libraries(${PROJECT_NAME} PRIVATE ${wxWidgets_LIBRARIES} ${SQLite3_LIBRARIES})
endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
