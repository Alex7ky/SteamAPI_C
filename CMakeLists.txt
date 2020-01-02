cmake_minimum_required(VERSION 3.10.2)

project(steam_api C)

add_definitions(-Os -Wall -Werror --std=gnu99 -Wmissing-declarations)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build/modules")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(SOURCES
	main.c
	src/inventory.c
	src/login.c
	src/market.c
	src/steam.c
	inc/login.h
	inc/inventory.h
	inc/market.h
	inc/steamdef.h
	inc/steamglob.h
	inc/steam.h
)

add_executable(steam_api ${SOURCES})

##########################################################
find_package(Curl REQUIRED)
if(NOT CURL_FOUND)
	message(SEND_ERROR "Failed to find CURL")
	return()
else()
	include_directories(${CURL_INCLUDE_DIRS})
	target_link_libraries(steam_api ${CURL_LIBRARIES})
endif()
##########################################################
find_package(SSL REQUIRED)
if(NOT SSL_FOUND)
	message(SEND_ERROR "Failed to find OpenSSL")
	return()
else()
	include_directories(${SSL_INCLUDE_DIR})
	target_link_libraries(steam_api ${SSL_LIBRARIES})
endif()
##########################################################
find_package(JSON-C REQUIRED)
if(NOT JSON-C_FOUND)
	message(SEND_ERROR "Failed to find Json-c.")
	return()
else()
	include_directories(${JSON-C_INCLUDE_DIR})
	target_link_libraries(steam_api ${JSON-C_LIBRARIES})
endif()
##########################################################
