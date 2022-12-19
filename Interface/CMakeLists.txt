PROJECT(Interface VERSION 1.0.0.0 LANGUAGES CXX)

# Create assembly info with current version.
CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/Template/AssemblyInfo.cpp.template" "${CMAKE_BINARY_DIR}/Config/${PROJECT_NAME}/AssemblyInfo.cpp")

SET(CppCliLib_HEADERS
    "Interface.h"
)

SET(CppCliLib_SOURCES
    "Interface.cpp"
)

# Optional: Create source groups for better code organization.
SOURCE_GROUP(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CppCliLib_SOURCES} ${CppCliLib_SOURCES})

# Add shared library project.
ADD_LIBRARY(${PROJECT_NAME} SHARED
    "${CMAKE_BINARY_DIR}/Config/${PROJECT_NAME}/AssemblyInfo.cpp"
    ${CppCliLib_HEADERS}
    ${CppCliLib_SOURCES}
)

# Set target link options (allows to step into and set breakpoints for mixed-mode C++ assemblies).
# This will not be enabled for Release or MinSizeRel builds.
TARGET_LINK_OPTIONS(${PROJECT_NAME} 
    PUBLIC $<$<OR:$<CONFIG:>,$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:/DEBUG /ASSEMBLYDEBUG>
)
target_link_options(${PROJECT_NAME} PRIVATE /INCREMENTAL:NO)

# Define the assembly to be mixed-mode (managed + unmanaged).
SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY COMMON_LANGUAGE_RUNTIME "")

# Set CLR assembly properties.
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
    VS_DOTNET_REFERENCES "System;System.Core"
    VS_GLOBAL_ROOTNAMESPACE ${PROJECT_NAME}
)