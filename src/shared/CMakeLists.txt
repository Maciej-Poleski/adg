set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.52 REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

set(SOURCES
    Request.cxx
    Address.cxx)

add_library(shared ${SOURCES})

set_target_properties(shared PROPERTIES COMPILE_FLAGS "-std=c++11")

target_link_libraries(shared ${Boost_LIBRARIES})
