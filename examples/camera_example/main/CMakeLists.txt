idf_component_register(SRCS take_picture.c my_request.c image_upload.c
                        PRIV_INCLUDE_DIRS .
                        PRIV_REQUIRES nvs_flash esp_psram esp_http_client) 
