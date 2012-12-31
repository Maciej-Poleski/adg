set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.52 REQUIRED serialization)

include_directories(${Boost_INCLUDE_DIRS})

set(SOURCES
    MasterToSlaveRequestNewDiscussion.cxx
    MasterToSlaveReplyNewDiscussion.cxx
)

add_library(ConnMS ${SOURCES})

set_target_properties(ConnMS PROPERTIES COMPILE_FLAGS "-std=c++11")

target_link_libraries(ConnMS ${Boost_LIBRARIES} shared)
