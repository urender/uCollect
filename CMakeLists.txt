cmake_minimum_required(VERSION 3.13)

PROJECT(uCollect C)
#ADD_DEFINITIONS(-Os -ggdb -Wall -Werror --std=gnu99 -Wmissing-declarations)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

FIND_PATH(ubox_include_dir NAMES libubox/usock.h)
FIND_PATH(ubus_include_dir NAMES libubus.h)
INCLUDE_DIRECTORIES(${ubox_include_dir} ${ubus_include_dir})

FIND_LIBRARY(ubox NAMES ubox)
FIND_LIBRARY(uci NAMES uci)
FIND_LIBRARY(ubus NAMES ubus)
FIND_LIBRARY(sqlite3 NAMES sqlite3)

SET(LIBS ${ubox} ${ubus} ${uci} ${sqlite3})

ADD_EXECUTABLE(uCollect main.c ubus.c db.c device.c state.c health.c event.c config.c)
TARGET_LINK_LIBRARIES(uCollect ${LIBS})

INSTALL(TARGETS uCollect
	RUNTIME DESTINATION sbin
)
