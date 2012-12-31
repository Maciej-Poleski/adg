set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.52 REQUIRED serialization)

include_directories(${Boost_INCLUDE_DIRS})

set(SOURCES
    ClientToSlaveRequest.cxx
    ClientToSlaveReply.cxx
    Post.cxx
)

add_library(ConnCS ${SOURCES})

set_target_properties(ConnCS PROPERTIES COMPILE_FLAGS "-std=c++11")

target_link_libraries(ConnCS ${Boost_LIBRARIES} shared)
