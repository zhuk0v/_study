cmake_minimum_required(VERSION 3.12)

if(DEFINED ENV{GITHUB_RUN_NUMBER})
  set(PATCH_NUM $ENV{GITHUB_RUN_NUMBER})
else()
  set(PATCH_NUM 1)
endif()
set(PROJECT_VESRION 0.0.${PATCH_NUM})

# Create project name
project(custom_allocator VERSION ${PROJECT_VESRION})

# -----------------------------------------------
# Create executable file
add_executable(custom_allocator main.cpp)

set_target_properties(custom_allocator PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_compile_options(custom_allocator
    PRIVATE -Wall -Wextra -pedantic -Werror
)

# -----------------------------------------------
install(TARGETS custom_allocator RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT my@email.com)

include(CPack)
