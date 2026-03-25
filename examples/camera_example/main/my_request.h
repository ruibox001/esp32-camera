#ifndef MY_REQUEST_H
#define MY_REQUEST_H

#include <esp_err.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t http_post_json(const char *url,
                          const char *json_data,
                          char *response_buf,
                          int buf_len);
esp_err_t upload_image(const char *url, uint8_t *img_buf, size_t img_len);

#ifdef __cplusplus
}
#endif

#endif // MY_REQUEST_H