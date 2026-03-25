#ifndef IMAGE_UPLOAD_H
#define IMAGE_UPLOAD_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t upload_my_image(const char *url, uint8_t *image_buf, size_t buf_len);

#ifdef __cplusplus
}
#endif

#endif
