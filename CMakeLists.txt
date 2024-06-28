cmake_minimum_required(VERSION 3.12)

if(DEFINED ENV{GITHUB_RUN_NUMBER})
  set(PATCH_NUM $ENV{GITHUB_RUN_NUMBER})
else()
  set(PATCH_NUM 1)
endif()
set(PROJECT_VESRION 0.0.${PATCH_NUM})

# Create project name
project(async VERSION ${PROJECT_VESRION})

# -----------------------------------------------
# Library
set(SOURCE_LIB
  "async.cpp")

# Create library file
add_library(${PROJECT_NAME} SHARED ${SOURCE_LIB})

# Add threads
find_package(Threads REQUIRED)

# Add Boost
find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
)

target_compile_options(${PROJECT_NAME}
    PRIVATE -Wall -Wextra -pedantic -Werror
)


# -----------------------------------------------
# Test executable file
set(SOURCE_EXE  
  "main.cpp")

set (PROJECT_TEST_NAME "test_${PROJECT_NAME}")

# Create executable file
add_executable(${PROJECT_TEST_NAME} ${SOURCE_EXE})

# Add libraries
target_link_libraries(${PROJECT_TEST_NAME}
  PRIVATE ${PROJECT_NAME})

# -----------------------------------------------
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT my@email.com)

include(CPack)
