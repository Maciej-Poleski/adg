set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.52 REQUIRED system)

include_directories(${Boost_INCLUDE_DIRS})

set(SOURCES
    Database.cxx
    Console.cxx
    main.cxx)

add_executable(client ${SOURCES})

set_target_properties(client PROPERTIES COMPILE_FLAGS "-std=c++11")

target_link_libraries(client ${Boost_LIBRARIES} ConnCM ConnCS shared)

install(TARGETS client RUNTIME DESTINATION client/bin)
