cmake_minimum_required(VERSION 3.0)

project(ObjectAPI)

add_executable(ObjectAPI
    main.cpp
    ObjResourceFactory.cpp
    ObjService.cpp
    ObjServiceSettingsFactory.cpp)

target_link_libraries(ObjectAPI restbed)

target_include_directories(ObjectAPI PUBLIC ${CMAKE_SOURCE_DIR}/include)

set_property(TARGET ObjectAPI PROPERTY CXX_STANDARD 17)