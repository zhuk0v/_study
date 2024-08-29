cmake_minimum_required(VERSION 3.12)

if(DEFINED ENV{GITHUB_RUN_NUMBER})
  set(PATCH_NUM $ENV{GITHUB_RUN_NUMBER})
else()
  set(PATCH_NUM 1)
endif()
# Version
set(PROJECT_VESRION 0.0.${PATCH_NUM})

# Create project name
project(db VERSION ${PROJECT_VESRION})

# -----------------------------------------------
# Dependencies

# Add threads
find_package(Threads REQUIRED)

# Add fmt library
find_package(fmt)

# Add Boost libraries
find_package(Boost)
include_directories(${Boost_INCLUDE_DIR})

# Add SQLite3 library
find_package (SQLite3)
include_directories(${SQLITE3_INCLUDE_DIRS})

# -----------------------------------------------
# Executable file
set(SOURCE_EXE 
  "main.cpp")

set (EXE_NAME "${PROJECT_NAME}")

# Create executable file
add_executable(${EXE_NAME} ${SOURCE_EXE})

# Add directories
target_include_directories(${EXE_NAME}
  PUBLIC ../include/pdc)

# Add link to library
target_link_libraries(${EXE_NAME} fmt::fmt)
target_link_libraries (${EXE_NAME} SQLite::SQLite3)

# Add properties
set_target_properties(${EXE_NAME} PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)

# # Add options
# target_compile_options(${EXE_NAME}
#     PRIVATE -Wall -Wextra -pedantic -Werror
# )

# -----------------------------------------------
install(TARGETS ${EXE_NAME} RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT my@email.com)

include(CPack)
