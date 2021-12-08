
#include "rest_api.h"
#include "utils.h"
#include "ledctl.h"

#define TRACE_TAG "rest-api"
#if !ENABLE_TRACE_REST_API
#undef TRACE
#define TRACE(...)
#endif

int rest_api_led_set_pixel(struct httpd_connection *con, const httpd_rest_call_t *restcall, const char *argv[], int argc)
{
    char *params[3];
    int channel;
    int pixel;
    int r,g,b;

    REST_API_VERIFY_PARAMS(3);

    TRACE("Set pixel  arg[0]='%s'  arg[1]='%s'  arg[2]='%s'", argv[0], argv[1], argv[2]);

    channel = atoi(argv[0]);
    pixel = atoi(argv[1]);

    if (split_line((char *)argv[2], ',', params, 3) != 3)
    {
        TRACE_ERROR("Parse RGB value failed");
        return REST_API_ERR;
    }

    r = atoi(params[0]);
    g = atoi(params[1]);
    b = atoi(params[2]);

    if (ledctl_set_pixel(channel, pixel, r, g, b) != 0)
    {
        TRACE_ERROR("Set channel: %d pixel: %d failed", channel, pixel);
        return REST_API_ERR;
    }

    return REST_API_OK;
}

int rest_api_led_update_pixel(struct httpd_connection *con, const httpd_rest_call_t *restcall, const char *argv[], int argc)
{
    char *params[3];
    int channel;
    int pixel;
    int r,g,b;

    REST_API_VERIFY_PARAMS(3);

    TRACE("Update pixel  arg[0]='%s'  arg[1]='%s'  arg[2]='%s'", argv[0], argv[1], argv[2]);

    channel = atoi(argv[0]);
    pixel = atoi(argv[1]);

    if (split_line((char *)argv[2], ',', params, 3) != 3)
    {
        TRACE_ERROR("Parse RGB value failed");
        return REST_API_ERR;
    }

    r = atoi(params[0]);
    g = atoi(params[1]);
    b = atoi(params[2]);

    if (ledctl_update_pixel(channel, pixel, r, g, b) != 0)
    {
        TRACE_ERROR("Set channel: %d pixel: %d failed", channel, pixel);
        return REST_API_ERR;
    }

    return REST_API_OK;
}

int rest_api_led_update_channel(struct httpd_connection *con, const httpd_rest_call_t *restcall, const char *argv[], int argc)
{
    char *params[3];
    int channel;
    int r,g,b;

    REST_API_VERIFY_PARAMS(2);

    TRACE("Update channel arg[0]='%s'  arg[1]='%s'", argv[0], argv[1]);

    channel = atoi(argv[0]);

    if (split_line((char *)argv[1], ',', params, 3) != 3)
    {
        TRACE_ERROR("Parse RGB value failed");
        return REST_API_ERR;
    }

    r = atoi(params[0]);
    g = atoi(params[1]);
    b = atoi(params[2]);

    if (ledctl_update_channel_pixels(channel, r, g, b) != 0)
    {
        TRACE_ERROR("Update channel: %d pixels failed", channel);
        return REST_API_ERR;
    }

    return REST_API_OK;
}

int rest_api_led_update_channel_ramp(struct httpd_connection *con, const httpd_rest_call_t *restcall, const char *argv[], int argc)
{
    char *params[3];
    int channel;
    int timelen;
    int r,g,b;

    REST_API_VERIFY_PARAMS(3);

    TRACE("Update channel ramp arg[0]='%s'  arg[1]='%s' argv[2]=%d", argv[0], argv[1], argv[2]);

    channel = atoi(argv[0]);
    timelen = atoi(argv[1]);

    if (split_line((char *)argv[2], ',', params, 3) != 3)
    {
        TRACE_ERROR("Parse RGB value failed");
        return REST_API_ERR;
    }

    r = atoi(params[0]);
    g = atoi(params[1]);
    b = atoi(params[2]);

    if (ledctl_update_channel_pixels_ramp(channel, timelen, r, g, b) != 0)
    {
        TRACE_ERROR("Update ramp channel: %d pixels failed", channel);
        return REST_API_ERR;
    }

    return REST_API_OK;
}

int rest_api_led_refresh(struct httpd_connection *con, const httpd_rest_call_t *restcall, const char *argv[], int argc)
{
    int channel;

    REST_API_VERIFY_PARAMS(1);

    TRACE("Refresh arg[0]='%s'", argv[0]);

    channel = atoi(argv[0]);

    ledctl_refresh(channel);

    return REST_API_OK;
}

int rest_api_led_clear(struct httpd_connection *con, const httpd_rest_call_t *restcall, const char *argv[], int argc)
{
    int channel;

    REST_API_VERIFY_PARAMS(1);

    TRACE("Clear arg[0]='%s'", argv[0]);

    channel = atoi(argv[0]);

    ledctl_clear(channel);

    return REST_API_OK;
}
