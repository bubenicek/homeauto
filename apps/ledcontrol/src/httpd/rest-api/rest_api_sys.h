
#ifndef __REST_API_SYS_H
#define __REST_API_SYS_H

int rest_api_sys_get_info(struct httpd_connection *con, const httpd_rest_call_t *restcall, const char *argv[], int argc);
int rest_api_sys_restart(struct httpd_connection *con, const httpd_rest_call_t *restcall, const char *argv[], int argc);

#endif // __REST_API_SYS_H
