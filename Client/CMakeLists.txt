cmake_minimum_required(VERSION 3.5)
project(Client)

set(SOURCE_FILES main.cpp)
add_executable(Client ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.6")
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DOTNET_REFERENCES "System")
set_target_properties(${PROJECT_NAME} PROPERTIES COMMON_LANGUAGE_RUNTIME "")
string(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
target_link_options(Client PRIVATE /INCREMENTAL:NO)

TARGET_LINK_LIBRARIES(Client)