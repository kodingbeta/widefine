cmake_minimum_required(VERSION 2.6)

include(SetUpConfigurations.cmake)

project(widefine)

if(APPLE)
add_definitions("-DOS_MACOSX=1")
endif()

if(UNIX)
add_definitions("-D_mkgmtime=timegm")
endif()

if(WIN32)
add_definitions("-DOS_WIN=1")
endif()

add_definitions("-DCDM_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/../lib/cdm/\"")

subdirs(${CMAKE_CURRENT_SOURCE_DIR}/../lib/wvdecrypter)
subdirs(${CMAKE_CURRENT_SOURCE_DIR}/../lib/p8-platform)
subdirs(${CMAKE_CURRENT_SOURCE_DIR}/../lib/expat)

set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/../lib/inputstream.adaptive/common/AdaptiveTree.h
	${CMAKE_CURRENT_SOURCE_DIR}/../lib/inputstream.adaptive/common/AdaptiveStream.h
	${CMAKE_CURRENT_SOURCE_DIR}/../lib/inputstream.adaptive/parser/DASHTree.h
	${CMAKE_CURRENT_SOURCE_DIR}/../lib/inputstream.adaptive/parser/PRProtectionParser.h
	${CMAKE_CURRENT_SOURCE_DIR}/../lib/inputstream.adaptive/helpers.h
)

set(SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/../lib/inputstream.adaptive/common/AdaptiveTree.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../lib/inputstream.adaptive/common/AdaptiveStream.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../lib/inputstream.adaptive/parser/DASHTree.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../lib/inputstream.adaptive/parser/PRProtectionParser.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../lib/inputstream.adaptive/log.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../lib/inputstream.adaptive/helpers.cpp
	main.cpp
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../lib/libbento4/Core
	${CMAKE_CURRENT_SOURCE_DIR}/../lib/expat/lib
)

if(NOT WIN32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
endif()

add_executable(widefine ${HEADERS} ${SOURCES})

target_link_libraries(widefine bento4 ssd_wv p8-platform expat)
