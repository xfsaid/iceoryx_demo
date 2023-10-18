cmake_minimum_required(VERSION 3.16)
project(ice_demo)

# ########################################################################################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread -fPIC -fpermissive")

# ########################################################################################
find_package(iceoryx_posh CONFIG REQUIRED)
find_package(iceoryx_hoofs CONFIG REQUIRED)

get_target_property(ICEORYX_CXX_STANDARD iceoryx_posh::iceoryx_posh CXX_STANDARD)
include(IceoryxPlatform)

# ########################################################################################
include_directories(/usr/include/)
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

# ########################################################################################
add_executable(cb_sub cb_sub_demo.cpp)
target_link_libraries(cb_sub iceoryx_posh::iceoryx_posh)

add_executable(cb_pub cb_pub_demo.cpp)
target_link_libraries(cb_pub iceoryx_posh::iceoryx_posh)
