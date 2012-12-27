set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.52 REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

set(SOURCES Request.cxx)

add_library(details ${SOURCES})

set_target_properties(details PROPERTIES COMPILE_FLAGS "-std=c++11")

target_link_libraries(details ${Boost_LIBRARIES})
