set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.52 REQUIRED system)

find_package(OpenSSL REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

set(SOURCES
    ClientRequest.cxx
    Database.cxx
    Console.cxx
    main.cxx)

add_executable(master ${SOURCES})

set_target_properties(master PROPERTIES COMPILE_FLAGS "-std=c++11")

target_link_libraries(master ${Boost_LIBRARIES} ConnCM ConnMS shared ${OPENSSL_LIBRARIES})

install(TARGETS master RUNTIME DESTINATION master/bin)
