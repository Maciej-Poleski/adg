set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.52 REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

set(SOURCES ClientToMasterRequest.cxx)

add_library(ConnCM ${SOURCES})

set_target_properties(ConnCM PROPERTIES COMPILE_FLAGS "-std=c++11")

target_link_libraries(ConnCM ${Boost_LIBRARIES} details)
