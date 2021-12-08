
#include "rest_api.h"

#define TRACE_TAG "rest-api"
#if !ENABLE_TRACE_REST_API
#undef TRACE
#define TRACE(...)
#endif

extern char _sdata;
extern char _edata;
#define DATA_SEG_SIZE   0  //(&_edata - &_sdata)

extern char _sbss;
extern char _ebss;
#define BSS_SEG_SIZE    0 //(&_ebss - &_sbss)

extern char _sccmram;
extern char _eccmram;
#define CCM_SEG_SIZE    0 //(&_eccmram - &_sccmram)



/** Get system info */
int rest_api_sys_get_info(struct httpd_connection *con, const httpd_rest_call_t *restcall, const char *argv[], int argc)
{
   rest_output_begin(con, REST_API_RESULT_OK, NULL);

   rest_output_object_begin(con, NULL);
   rest_output_object_begin(con, "sysinfo");

   rest_output_value_str(con, "uptime", "%s", trace_uptime());
   rest_output_value_str(con, "systime", "%s", trace_systime());
   rest_output_value_str(con, "hwver", "%d.%d", CFG_HW_VERSION_MODEL, CFG_HW_VERSION_REVISION);
   rest_output_value_str(con, "fwver", "%d.%d", CFG_FW_VERSION_MAJOR, CFG_FW_VERSION_MINOR);

   rest_output_object_begin(con, "meminfo");
   rest_output_value_int(con, "free_heap_size", (int)osMemGetFreeSize());
   rest_output_value_int(con, "total_heap_size", (int)osMemGetTotalSize());
   rest_output_value_int(con, "data_size", DATA_SEG_SIZE);
   rest_output_value_int(con, "bss_size", BSS_SEG_SIZE);
   rest_output_value_int(con, "ccm_size", CCM_SEG_SIZE);
   rest_output_value_int(con, "total_used_memory", (osMemGetTotalSize() - osMemGetFreeSize()) + DATA_SEG_SIZE + CCM_SEG_SIZE);
   rest_output_object_end(con);


   rest_output_object_end(con);
   rest_output_object_end(con);

   rest_output_end(con);

   return 0;
}

int rest_api_sys_restart(struct httpd_connection *con, const httpd_rest_call_t *restcall, const char *argv[], int argc)
{
   rest_output_begin(con, REST_API_RESULT_OK, NULL);
   rest_output_end(con);

   hal_reset();

   return 0;
}
