CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
PROJECT(Example LANGUAGES CSharp CXX)

SET(CMAKE_DOTNET_TARGET_FRAMEWORK_VERSION "v4.7.2")
SET(CMAKE_CSharp_FLAGS "/langversion:6")

set(CMAKE_GENERATOR_PLATFORM x64)
SET(CMAKE_CSharp_FLAGS "/platform:x64")

SET(CMAKE_CXX_STANDARD 17)

# Define runtime intermediate build directory, so that debugging the Visual Studio solution works as expected.
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Bin/")

# Set export directory for CMake config.
SET(CMAKE_INSTALL_BINARY_DIR    "bin/")
SET(CMAKE_INSTALL_LIBRARY_DIR   "bin/")
SET(CMAKE_INSTALL_INCLUDE_DIR   "include/")
SET(CMAKE_INSTALL_EXPORT_DIR    "cmake/")

ADD_SUBDIRECTORY(Interface)
ADD_SUBDIRECTORY(Client)
ADD_SUBDIRECTORY(Server)
ADD_SUBDIRECTORY(NormalPlugin)
