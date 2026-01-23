cmake_minimum_required(VERSION 3.12)

project(pueorawdata VERSION 0.1
                    DESCRIPTION "Read/write PUEO Raw data"
                    LANGUAGES C  CXX)

include(GNUInstallDirs)

# Require C17 with gcc extensions
# also float32 support
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(default_build_type "RelWithDebInfo")
set(WARNFLAGS -Wall -Wextra -Wno-missing-braces -Wno-override-init)

find_package(ZLIB REQUIRED)

find_path(ZSTD_INCLUDE_DIR NAMES zstd.h)
find_library(ZSTD_LIBRARY NAMES zstd zstd_static)

find_package(PostgreSQL)
find_package(SQLite3)

set(PUEO_INSTALL_DIR "" CACHE STRING "Installation directory for PUEO libraries")
message(STATUS $ENV{PUEO_UTIL_INSTALL_DIR})

if(NOT PUEO_INSTALL_DIR STREQUAL "")
  # Already specified by command line or cached value
  message(STATUS "PUEO_INSTALL_DIR is set to ${PUEO_INSTALL_DIR}")
elseif (DEFINED ENV{PUEO_UTIL_INSTALL_DIR})
  message(STATUS "PUEO_INSTALL_DIR is set from environment variable: $ENV{PUEO_UTIL_INSTALL_DIR}")
  set(PUEO_INSTALL_DIR "$ENV{PUEO_UTIL_INSTALL_DIR}" CACHE STRING "Installation directory for PUEO library" FORCE)
elseif(EXISTS /pueo)
  message(STATUS "/pueo exists so defaulting installation to /pueo")
  set(PUEO_INSTALL_DIR "/pueo" CACHE STRING "Installation directory for PUEO library" FORCE)
else()
  message(STATUS "Setting installation dir to /usr/local")
  set(PUEO_INSTALL_DIR "/usr/local" CACHE STRING "Installation directory for PUEO library" FORCE)
endif()

set (SOURCES src/rawio.c src/rawio_packets.c src/rawio_versions.c src/sensor_ids.c src/pueocrc.c src/rawio_dump.c src/rawio_db.c src/prio_interface.c)
set(PUBLIC_HEADERS inc/pueo/rawdata.h inc/pueo/rawio.h inc/pueo/sensor_ids.h inc/pueo/sensor_ids_compat.h inc/pueo/pueo.h inc/pueo/prio_interface.h)
set(PRIVATE_HEADERS src/float16_guard.h src/pueocrc.h src/rawio_packets.h src/rawio_versions.h)
set(HEADERS ${PUBLIC_HEADERS} ${PRIVATE_HEADERS})

add_library(pueorawdata SHARED ${SOURCES} ${HEADERS})
add_library(pueorawdata-static STATIC ${SOURCES} ${HEADERS})

#add_library(fakeprio SHARED test/fakeprio.c)
#target_link_libraries(fakeprio pueorawdata)

#add_library(fakeprio++ SHARED test/fakeprio.cc)
#target_link_libraries(fakeprio++ pueorawdata)


target_compile_options(pueorawdata PRIVATE ${WARNFLAGS})
target_link_libraries(pueorawdata PRIVATE ZLIB::ZLIB -lm -ldl)
target_link_libraries(pueorawdata PRIVATE ${ZSTD_LIBRARY})

target_include_directories(pueorawdata PUBLIC inc)
target_include_directories(pueorawdata PRIVATE src)
target_include_directories(pueorawdata PRIVATE ${ZSTD_INCLUDE_DIR})
target_include_directories(pueorawdata PRIVATE ${ZLIB_INCLUDE_DIR})

target_include_directories(pueorawdata-static PUBLIC inc)
target_include_directories(pueorawdata-static PRIVATE src)

if (PostgreSQL_FOUND)
  message(STATUS "Found PostgreSQL")
  target_link_libraries(pueorawdata PRIVATE PostgreSQL::PostgreSQL)
  target_include_directories(pueorawdata PRIVATE ${PostgreSQL_INCLUDE_DIRS})
  add_compile_definitions(PGSQL_ENABLED)
else()
  message(STATUS "PostgreSQL not found")
endif()

if (SQLite3_FOUND)
  message(STATUS "Found SQLite3")
  target_link_libraries(pueorawdata PRIVATE ${SQLite3_LIBRARIES})
  target_include_directories(pueorawdata PRIVATE ${SQLite3_INCLUDE_DIRS})
  add_compile_definitions(SQLITE_ENABLED)
else()
  message(STATUS "SQLite3 not found")
endif()


if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui/cmake-tui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


install (TARGETS pueorawdata pueorawdata-static DESTINATION ${PUEO_INSTALL_DIR}/lib)
install (DIRECTORY inc/pueo DESTINATION ${PUEO_INSTALL_DIR}/include)

macro(add_program binary_name where)
      add_executable(${binary_name} ${where}/${binary_name}.c)
      target_link_libraries(${binary_name}  pueorawdata)
      INSTALL (TARGETS ${binary_name} DESTINATION ${PUEO_INSTALL_DIR}/bin)
endmacro()

add_program(write-wf test)
add_program(write-cmd-echo test)
add_program(read-wf test)
add_program(write-wfs test)
add_program(read-wfs test)
add_program(test-prio test)
add_program(packet-type test)
add_program(read-sensors-telem test)
add_program(pueo-init-db progs)
add_program(read-packets test)
add_program(read-events test)
add_program(read-image test)
add_program(read-files test)
