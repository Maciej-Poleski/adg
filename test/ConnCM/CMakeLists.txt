set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.52 COMPONENTS unit_test_framework system)
if(${Boost_FOUND})

    include_directories(${Boost_INCLUDE_DIRS} "../../src/ConnCM")
    add_definitions(-DBOOST_TEST_DYN_LINK)

    set(SOURCES
        main.cxx
        test01.cxx)

    add_executable(ConnCMTest ${SOURCES})
    set_target_properties(ConnCMTest PROPERTIES COMPILE_FLAGS "-std=c++11")
    target_link_libraries(ConnCMTest ${Boost_LIBRARIES} ConnCM)

    add_test(ConnCM ConnCMTest)
endif(${Boost_FOUND})
