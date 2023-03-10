# Setup project.
PROJECT(NormalPlugin VERSION 1.0.0.0 LANGUAGES CSharp)

# Setup project to use C# utilities.
INCLUDE(CSharpUtilities)

# Create assembly info with current version.
CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/Template/AssemblyInfo.cs.template" "${CMAKE_BINARY_DIR}/Config/${PROJECT_NAME}/AssemblyInfo.cs")

# Add shared library project.
ADD_LIBRARY(${PROJECT_NAME} SHARED
    "${CMAKE_BINARY_DIR}/Config/${PROJECT_NAME}/AssemblyInfo.cs"
    "NormalPlugin.cs"
)

# Set designer and XAML properties.
CSHARP_SET_DESIGNER_CS_PROPERTIES(
    "${CMAKE_BINARY_DIR}/Config/${PROJECT_NAME}/AssemblyInfo.cs"
)

# Set CLR assembly properties.
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
    VS_DOTNET_REFERENCES "System;System.Core"
    VS_GLOBAL_ROOTNAMESPACE ${PROJECT_NAME}
)
target_link_libraries(${PROJECT_NAME} PUBLIC Interface)

set_target_properties( ${PROJECT_NAME}
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Plugins
)