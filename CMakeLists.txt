cmake_minimum_required (VERSION 3.6)
include(GNUInstallDirs)
project (FFJSON)
if (DEFINED _DEBUG)
ADD_DEFINITIONS(-D_DEBUG=${_DEBUG})
endif()
set (VERSION_MAJOR 1)
set (VERSION_MINOR 0)
set (Umbrella "ferryfair")

# macOS specific code
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(macOS ON)
endif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# Linux specific code
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(Linux ON)
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# Windows specific code
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set (Windows ON)
endif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

if(NOT LINKAGE)
  set(LINKAGE "STATIC")
  message("Building static by default")
else(NOT LINKAGE)
  string(TOUPPER "${LINKAGE}" LINKAGE)
endif()

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)
configure_file(
  "${PROJECT_SOURCE_DIR}/pkgconfig.pc.in"
  "${PROJECT_BINARY_DIR}/pkgconfig.pc"
)

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIC")

if (NOT macOS)
  set (CMAKE_EXE_LINKER_FLAGS 
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-unresolved-symbols=ignore-in-shared-libs"
  )
endif ()

file(GLOB HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.h)
file(GLOB SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.cpp)
list(FILTER HEADERS EXCLUDE REGEX "^\\..+")
list(FILTER SOURCES EXCLUDE REGEX "^\\..+")

include_directories(${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_INSTALL_FULL_INCLUDEDIR}/${Umbrella}
)
include_directories (
  ".."
  "../logger"
)
link_directories("${CMAKE_INSTALL_FULL_LIBDIR}")
if(CMAKE_CONFIGURATION_TYPES)
  link_directories("../ferrybase/build/$(ConfigurationName)")
  link_directories("../logger/build/$(ConfigurationName)")
else(CMAKE_CONFIGURATION_TYPES)
  link_directories("../ferrybase/build")
  link_directories("../logger/build")
endif()
MESSAGE("CMAKE_INSTALL_FULL_INCLUDEDIR ${CMAKE_INSTALL_FULL_INCLUDEDIR}")
MESSAGE("CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR}")
MESSAGE("CMAKE_INSTALL_FULL_LIBDIR ${CMAKE_INSTALL_FULL_LIBDIR}")
if (${LINKAGE} MATCHES "STATIC")
  add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
else ()
  add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
endif ()
set_target_properties(${PROJECT_NAME}
  PROPERTIES
  VERSION ${VERSION_MAJOR}.${VERSION_MINOR}
  SOVERSION ${VERSION_MAJOR}
)
target_link_libraries(${PROJECT_NAME} logger ferrybase)
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${Umbrella})
install(FILES "${PROJECT_BINARY_DIR}/config.h"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${Umbrella}
  RENAME ${PROJECT_NAME}Config.h
)
install(FILES "${PROJECT_BINARY_DIR}/pkgconfig.pc"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
  RENAME "${PROJECT_NAME}.pc"
)
install(FILES "${PROJECT_BINARY_DIR}/pkgconfig.pc"
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig
  RENAME "${PROJECT_NAME}.pc"
)

if (DEFINED BUILD_TESTS)
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
  target_link_libraries(TestFFJSON FFJSON logger ferrybase)
  target_include_directories(TestFFJSON PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_INSTALL_FULL_INCLUDEDIR}/${Umbrella}
  )
  message("INSTALL_INTERFACE:include/${Umbrella}:"
    $<INSTALL_INTERFACE:include/${Umbrella}>
  )
  add_executable(IteratorIncrement tests/iteratorincrement.cpp)
  target_link_libraries(IteratorIncrement FFJSON logger ferrybase)
  target_include_directories(IteratorIncrement PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_INSTALL_FULL_INCLUDEDIR}/${Umbrella}
  )

  add_test(test1 TestFFJSON)
  add_test(test2 IteratorIncrement)
endif()

