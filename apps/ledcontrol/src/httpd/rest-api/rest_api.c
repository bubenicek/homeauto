
#include "rest_api.h"

#define TRACE_TAG "rest-api"
#if !ENABLE_TRACE_REST_API
#undef TRACE
#define TRACE(...)
#endif

/** REST calls definitions */
const httpd_rest_call_t httpd_restcalls[] =
{
//  Version      Name,                                                        GET,     UPDATE(PUT),      INSERT(POST),     DELETE,  Object definition
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
   {REST_API_V1 "/system/info",                                               rest_api_sys_get_info},
   {REST_API_V1 "/system/restart",                                            NULL, rest_api_sys_restart},

   {REST_API_V1 "/led/set_pixel/{channel}/{pixel}/{rgb}",                     rest_api_led_set_pixel, rest_api_led_set_pixel, rest_api_led_set_pixel, NULL},
   {REST_API_V1 "/led/update_pixel/{channel}/{pixel}/{rgb}",                  rest_api_led_update_pixel, rest_api_led_update_pixel, rest_api_led_update_pixel, NULL},
   {REST_API_V1 "/led/update_channel/{channel}/{rgb}",                        rest_api_led_update_channel, rest_api_led_update_channel, rest_api_led_update_channel, NULL},
   {REST_API_V1 "/led/update_channel_ramp/{channel}/{timelen}{rgb}",          rest_api_led_update_channel_ramp, rest_api_led_update_channel_ramp, rest_api_led_update_channel_ramp, NULL},
   {REST_API_V1 "/led/refresh/{channel}",                                     rest_api_led_refresh, rest_api_led_refresh, rest_api_led_refresh, NULL},
   {REST_API_V1 "/led/clear/{channel}",                                       rest_api_led_clear, rest_api_led_clear, rest_api_led_clear, NULL},

   {NULL}
};

const char *rest_get_local_url(struct httpd_connection *con, char *buf, int bufsize)
{
   struct sockaddr_in ipaddr;
   socklen_t addrlen = sizeof(ipaddr);

   if (getsockname(con->sd, (struct sockaddr * )&ipaddr, &addrlen) != 0)
   {
      TRACE("Get Local IP address failed - %s", strerror(errno));
      snprintf(buf, bufsize, "%s", strerror(errno));
   }
   else
   {
      snprintf(buf, bufsize, "http://%s:%d/rest", inet_ntoa(ipaddr.sin_addr), HTTPD_PORT);
   }

   return buf;
}


const char *rest_get_local_ipaddr(char *buf, int bufsize)
{
   return hal_net_get_local_ipaddr(CFG_NETIF, buf, bufsize);
}
