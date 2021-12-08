
#ifndef __REST_API_LED_H
#define __REST_API_LED_H

int rest_api_led_set_pixel(struct httpd_connection *con, const httpd_rest_call_t *restcall, const char *argv[], int argc);
int rest_api_led_update_pixel(struct httpd_connection *con, const httpd_rest_call_t *restcall, const char *argv[], int argc);
int rest_api_led_update_channel(struct httpd_connection *con, const httpd_rest_call_t *restcall, const char *argv[], int argc);
int rest_api_led_update_channel_ramp(struct httpd_connection *con, const httpd_rest_call_t *restcall, const char *argv[], int argc);
int rest_api_led_refresh(struct httpd_connection *con, const httpd_rest_call_t *restcall, const char *argv[], int argc);
int rest_api_led_clear(struct httpd_connection *con, const httpd_rest_call_t *restcall, const char *argv[], int argc);



#endif // __REST_API_SYS_H
