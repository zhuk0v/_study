cmake_minimum_required(VERSION 3.12)

if(DEFINED ENV{GITHUB_RUN_NUMBER})
  set(PATCH_NUM $ENV{GITHUB_RUN_NUMBER})
else()
  set(PATCH_NUM 1)
endif()
# Version
set(PROJECT_VESRION 0.0.${PATCH_NUM})

# Create project name
project(pdc VERSION ${PROJECT_VESRION})

# -----------------------------------------------
# Dependencies

# Add threads
find_package(Threads REQUIRED)

# Add Boost
find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

# -----------------------------------------------
# Test executable file
set(SOURCE_EXE  
  "test_1.cpp")

set (TEST_1_EXE_NAME "${PROJECT_NAME}_test_1")

# Create executable file
add_executable(${TEST_1_EXE_NAME} ${SOURCE_EXE})

# Add directories
target_include_directories(${TEST_1_EXE_NAME}
  PUBLIC ../include/pdc)

# Add properties
set_target_properties(${TEST_1_EXE_NAME} PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)

# Add options
target_compile_options(${TEST_1_EXE_NAME}
    PRIVATE -Wall -Wextra -pedantic -Werror
)

# -----------------------------------------------
install(TARGETS ${TEST_1_EXE_NAME} RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT my@email.com)

include(CPack)
