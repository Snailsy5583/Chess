cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(Chess C CXX)

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Dist"
    "Release"
    CACHE STRING "" FORCE
)

################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################
add_subdirectory(Chess)
add_subdirectory(vendor/GLAD)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/stb/stb)
