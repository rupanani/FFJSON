cmake_minimum_required (VERSION 3.5)
include(GNUInstallDirs)
project (FFJSON)
IF (DEFINED _DEBUG)
ADD_DEFINITIONS(-D_DEBUG=${_DEBUG})
ENDIF()
set (FFJSON_VERSION_MAJOR 1)
set (FFJSON_VERSION_MINOR 0)
set (Umbrella "ferryfair")
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
# Mac OS X specific code
SET(macOS ON)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
    "${PROJECT_SOURCE_DIR}/FFJSONConfig.h.in"
    "${PROJECT_BINARY_DIR}/FFJSONConfig.h"
    )
configure_file(
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.pc.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc"
    )
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

set (GCC_COVERAGE_COMPILE_FLAGS "-std=c++14 -O0")
set (GCC_COVERAGE_LINK_FLAGS "")
include_directories(${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_INSTALL_INCLUDEDIR}/${Umbrella})
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
file(GLOB HEADERS *.h)
file(GLOB SOURCES *.cpp)
add_library(FFJSONStatic STATIC ${SOURCES} ${HEADERS})
add_library(FFJSON SHARED ${SOURCES} ${HEADERS})
target_link_libraries(FFJSONStatic logger base ssl pthread xml2 crypto z)
if(!macOS)
target_link_libraries(FFJSONStatic rt)
endif(!macOS)
target_link_libraries(FFJSON logger base ssl pthread xml2 crypto z)
if(!macOS)
target_link_libraries(FFJSON rt)
endif(!macOS)
set_target_properties(FFJSONStatic PROPERTIES OUTPUT_NAME FFJSON)
set_target_properties(FFJSON
    PROPERTIES
    VERSION ${FFJSON_VERSION_MAJOR}.${FFJSON_VERSION_MINOR}
    SOVERSION ${FFJSON_VERSION_MAJOR}
    )
set_target_properties(FFJSONStatic
    PROPERTIES
    VERSION ${FFJSON_VERSION_MAJOR}.${FFJSON_VERSION_MINOR}
    SOVERSION ${FFJSON_VERSION_MAJOR}
    )
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${Umbrella})
install(FILES "${PROJECT_BINARY_DIR}/FFJSONConfig.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${Umbrella})
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    RENAME "lib${PROJECT_NAME}.pc")
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.pc"
    DESTINATION share/pkgconfig
    RENAME "lib${PROJECT_NAME}.pc")

include(CTest)
#set (GCC_COVERAGE_COMPILE_FLAGS "-std=c++14")
#set (GCC_COVERAGE_LINK_FLAGS "-lFFJSON -lbase -llogger")
#include_directories(${CMAKE_CURRENT_SOURCE_DIR} "/usr/local/include/ferryfair")
#link_directories(${PROJECT_BINARY_DIR})
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
#set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

add_executable(TestFFJSON
    tests/TestFFJSON.cpp
    sample.ffjson
    MultiLevelInheritance.ffjson
    MultiLineArray.ffjson
    ExtensionTest.ffjson
    UpdateTest.json
    example.json
    )
target_link_libraries(TestFFJSON FFJSON logger base ssl pthread xml2 crypto z)
if(!macOS)
target_link_libraries(TestFFJSON rt)
endif(!macOS)
target_include_directories(TestFFJSON PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/${Umbrella}>
    )

add_executable(IteratorIncrement tests/iteratorincrement.cpp)
target_link_libraries(IteratorIncrement FFJSON logger base ssl pthread xml2 crypto z)
if(!macOS)
target_link_libraries(IteratorIncrement rt)
endif(!macOS)
target_include_directories(IteratorIncrement PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/${Umbrella}>
    )

add_test(test1 TestFFJSON)
add_test(test2 IteratorIncrement)
