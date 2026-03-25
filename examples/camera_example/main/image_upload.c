#include "image_upload.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "cJSON.h"
#include <string.h>

static const char *TAG = "IMAGE_UPLOAD";

esp_err_t upload_my_image(const char *url, uint8_t *image_buf, size_t buf_len)
{
    if (image_buf == NULL || buf_len == 0 || url == NULL) {
        ESP_LOGE(TAG, "Invalid parameters");
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = ESP_OK;
    esp_http_client_handle_t client = NULL;
    int http_status_code = 0;

    esp_http_client_config_t http_config = {
        .url = url,
        .timeout_ms = 15000,
        .method = HTTP_METHOD_POST,
    };

    client = esp_http_client_init(&http_config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return ESP_FAIL;
    }

    char content_length_str[32];
    snprintf(content_length_str, sizeof(content_length_str), "%zu", buf_len);

    esp_http_client_set_header(client, "Content-Type", "image/jpeg");
    esp_http_client_set_header(client, "Content-Length", content_length_str);

    err = esp_http_client_open(client, buf_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return err;
    }

    int written = esp_http_client_write(client, (char *)image_buf, buf_len);
    if (written < 0) {
        ESP_LOGE(TAG, "Error writing image data");
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return ESP_FAIL;
    }

    http_status_code = esp_http_client_fetch_headers(client);
    ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %lld",
             http_status_code, esp_http_client_get_content_length(client));

    if (http_status_code >= 200 && http_status_code < 300) {
        ESP_LOGI(TAG, "Image uploaded successfully");
        err = ESP_OK;
    } else {
        ESP_LOGE(TAG, "Image upload failed with HTTP status: %d", http_status_code);
        err = ESP_FAIL;
    }

    esp_http_client_close(client);
    esp_http_client_cleanup(client);

    return err;
}
