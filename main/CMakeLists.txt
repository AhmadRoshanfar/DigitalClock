idf_component_register(SRCS "main.c" "rest_server.c"
                    INCLUDE_DIRS ".")
                    
set(WEB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../front/led-quasar/dist")
if(EXISTS ${WEB_SRC_DIR}/spa)
    spiffs_create_partition_image(www ${WEB_SRC_DIR}/spa FLASH_IN_PROJECT)
else()
    message(FATAL_ERROR "${WEB_SRC_DIR}/spa doesn't exit. Please run 'npm run build' in ${WEB_SRC_DIR}")
endif()
