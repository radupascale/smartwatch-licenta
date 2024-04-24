FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.* ${CMAKE_SOURCE_DIR}/src/components/*.*)

idf_component_register(SRCS ${app_sources})
