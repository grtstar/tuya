cmake_minimum_required(VERSION 3.0.2)
project(tuya_app)

if (NOT DEFINED ENV{COMMON_DIR})
	message(FATAL_ERROR "Please set COMMON_DIR first")
else()
	message(STATUS "COMMON_DIR is $ENV{COMMON_DIR}")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_CXX_STANDARD 11)  
add_link_options("LINKER:-rpath-link,$ENV{COMMON_DIR}/lib")

find_package(spdlog REQUIRED)
find_package(lcm REQUIRED)

include_directories(apps/include sdk/include apps/src)
include_directories(apps/src/bluez/includes)
include_directories(apps/src/bluez/includes/src/shared)
include_directories(apps/src/bluez/includes/lib)
include_directories(apps/src/wifi/include)
include_directories(nl/include/libnl3)

add_subdirectory(apps/src/bluez/libbluetooth)
add_subdirectory(apps/src/bluez/libshared)

file(GLOB WIFI_SOURCES "apps/src/wifi/wifi/*.c")

add_compile_definitions(OS_NET_LINUX_OS)
add_compile_definitions(TY_BT_MOD=1)
#add_compile_definitions(RK_DEVICEIO)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/sdk/lib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/nl/lib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/lib64/bluez)

add_executable(${PROJECT_NAME}  apps/src/tuya.cpp
                                apps/src/tuya_wifi.c
                                apps/src/tuya_ops.cpp
                                apps/src/tuya_ipc_media_demo.c
                                apps/src/tuya_ipc_sweeper_demo.c
                                apps/src/tuya_robot.cpp
                                apps/src/tuya_enums.cpp
                                apps/src/tuya_dp_function.cpp
                                apps/src/tuya_utils.cpp
                                apps/src/lz4/lz4.c
                                apps/src/lz4/lz4frame.c
                                apps/src/lz4/lz4hc.c
                                apps/src/lz4/xxhash.c
                                apps/src/tuya_os_adapt_bt.c
                                ${WIFI_SOURCES}
)

#用于删除某编译选项，例如 -O3,(关闭 -O3 优化后，崩溃时可提供更详细的调用栈信息)
# get_target_property(COMPILE_FLAGS ${PROJECT_NAME} COMPILE_OPTIONS )
# list(REMOVE_ITEM COMPILE_FLAGS -O3)
# set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_OPTIONS "${COMPILE_FLAGS}" )

# spdlog
target_link_libraries(${PROJECT_NAME} spdlog::spdlog_header_only)
# lcm
target_link_libraries(${PROJECT_NAME} lcm)
# tuya
target_link_libraries(${PROJECT_NAME} tuya_iot)
# deviceio
#target_link_libraries(${PROJECT_NAME} DeviceIo)
#target_link_libraries(${PROJECT_NAME} asound)
# bluez
target_link_libraries(${PROJECT_NAME} bluetooth shared)
# libnl
target_link_libraries(${PROJECT_NAME} libnl-3.a libnl-genl-3.a)
# wpa
target_link_libraries(${PROJECT_NAME} libwpa_client_64.a)
# curl
target_link_libraries(${PROJECT_NAME} curl)
# ssl
target_link_libraries(${PROJECT_NAME} ssl)
# rtmp
target_link_libraries(${PROJECT_NAME} rtmp)
# crypto
target_link_libraries(${PROJECT_NAME} crypto)
