FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.* ${CMAKE_SOURCE_DIR}/src/components/*.*
                            ${CMAKE_SOURCE_DIR}/src/apps/*.*
                            ${CMAKE_SOURCE_DIR}/src/resources/fonts/*.*
                            ${CMAKE_SOURCE_DIR}/src/resources/images/*.*)
idf_component_register(SRCS ${app_sources}
                    REQUIRES esp_netif)
