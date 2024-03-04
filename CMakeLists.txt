cmake_minimum_required(VERSION 3.12)

if(DEFINED ENV{GITHUB_RUN_NUMBER})
  set(PATCH_NUM $ENV{GITHUB_RUN_NUMBER})
else()
  set(PATCH_NUM 1)
endif()
set(PROJECT_VESRION 0.0.${PATCH_NUM})

# Create project name
project(helloworld VERSION ${PROJECT_VESRION})

# Create template defines
configure_file(version.h.in version.h)

# Create library file
add_library(helloworld lib.cpp)

set_target_properties(helloworld PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

target_compile_options(helloworld
    PRIVATE -Wall -Wextra -pedantic -Werror
)

target_include_directories(helloworld
    PRIVATE "${CMAKE_BINARY_DIR}"
)

# Create executable file
add_executable(helloworld_exe main.cpp)

set_target_properties(helloworld_exe PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

target_compile_options(helloworld_exe
    PRIVATE -Wall -Wextra -pedantic -Werror
)

target_link_libraries(helloworld_exe 
    PRIVATE helloworld
)

# Create test
include(FetchContent)

FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(helloworld_test_exe test_version.cpp)

set_target_properties(helloworld_test_exe PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

target_compile_options(helloworld_exe
    PRIVATE -Wall -Wextra -pedantic -Werror
)

target_link_libraries(helloworld_test_exe helloworld GTest::gtest_main)

include(GoogleTest)

gtest_discover_tests(helloworld_test_exe)

install(TARGETS helloworld_exe RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT my@email.com)

include(CPack)
