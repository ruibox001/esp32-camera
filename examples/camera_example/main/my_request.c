#include "my_request.h"
#include <esp_log.h>
#include <esp_http_client.h>

static const char *TAG = "my_request";


esp_err_t http_post_json(const char *url,
                          const char *json_data,
                          char *response_buf,
                          int buf_len)
{
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .timeout_ms = 10000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to init http client");
        return ESP_FAIL;
    }

    // 设置 JSON header
    esp_http_client_set_header(client, "Content-Type", "application/json");

    // 设置 POST body
    if (json_data) {
        esp_http_client_set_post_field(client, json_data, strlen(json_data));
    }

    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTP POST failed: %s", esp_err_to_name(err));
        goto cleanup;
    }

    int status_code = esp_http_client_get_status_code(client);
    ESP_LOGI(TAG, "HTTP Status = %d", status_code);

    if (status_code != 200) {
        ESP_LOGE(TAG, "Server returned status %d", status_code);
        err = ESP_FAIL;
        goto cleanup;
    }

    // 读取响应
    int read_len = esp_http_client_read_response(client, response_buf, buf_len - 1);

    if (read_len <= 0) {
        ESP_LOGE(TAG, "No response data");
        err = ESP_FAIL;
        goto cleanup;
    }

    response_buf[read_len] = '\0';
    ESP_LOGI(TAG, "Response:\n%s", response_buf);

cleanup:
    esp_http_client_cleanup(client);
    return err;
}

esp_err_t upload_image(const char *url, uint8_t *img_buf, size_t img_len)
{
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .timeout_ms = 10000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to init http client");
        return ESP_FAIL;
    }

    // 设置 Content-Type 为 multipart
    esp_http_client_set_header(client, "Content-Type",
                               "multipart/form-data; boundary=----ESP32CAM");

    // 构造 multipart body
    char *body = malloc(img_len + 300);
    if (!body) {
        ESP_LOGE(TAG, "malloc failed");
        esp_http_client_cleanup(client);
        return ESP_ERR_NO_MEM;
    }

    int len = snprintf(body, 300,
        "------ESP32CAM\r\n"
        "Content-Disposition: form-data; name=\"image\"; filename=\"cam.jpg\"\r\n"
        "Content-Type: image/jpeg\r\n"
        "\r\n");

    memcpy(body + len, img_buf, img_len);
    len += img_len;

    strcpy(body + len, "\r\n------ESP32CAM--\r\n");

    esp_http_client_set_post_field(client, body, strlen(body));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Upload OK, status = %d",
                 esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE(TAG, "Upload failed: %s", esp_err_to_name(err));
    }

    free(body);
    esp_http_client_cleanup(client);
    return err;
}