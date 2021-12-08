
#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H

#define CFG_FW_VERSION_MAJOR                0
#define CFG_FW_VERSION_MINOR                1

#define CFG_CMSIS_OS_API                    1


//
// Traces
//
#define ENABLE_TRACE_MAIN                   1
#define ENABLE_TRACE_HAL                    1
#define ENABLE_TRACE_REST_API               1


//
// Network settings
//
#define CFG_NETIF                           HAL_NETIF_WIFI
#define CFG_NET_SSID                        "suxoap"
#define CFG_NET_PASSWD                      "kokosak123456"
#define CFG_NET_IPADDR                      HAL_NET_IPADDR(192,168,8,12)
#define CFG_NET_GW                          HAL_NET_IPADDR(192,168,8,1)
#define CFG_NET_NETMASK                     HAL_NET_IPADDR(255,255,255,0)

//
// LED controller config
//

#define CFG_LEDCTL_NUM_CHANNELS             4

#define CFG_LEDCTL_CHANNEL0_LENGTH 		    256
#define CFG_LEDCTL_CHANNEL0_GPIO 		    GPIO_NUM_21

#define CFG_LEDCTL_CHANNEL1_LENGTH 		    256
#define CFG_LEDCTL_CHANNEL1_GPIO 		    GPIO_NUM_19

#define CFG_LEDCTL_CHANNEL2_LENGTH 		    256
#define CFG_LEDCTL_CHANNEL2_GPIO 		    GPIO_NUM_18

#define CFG_LEDCTL_CHANNEL3_LENGTH 		    256
#define CFG_LEDCTL_CHANNEL3_GPIO 		    GPIO_NUM_5

//
// HTTPD config
//
#define HTTPD_PORT      80

/** Define HTTP connection context variables */
#define HTTPD_CON_REST_API_CONTEXT \
   int element_count;

//
// Tasks
//
#define CFG_HTTP_THREAD_STACK_SIZE         1024
#define CFG_HTTP_THREAD_PRIORITY           osPriorityNormal

#define CFG_HTTPD_THREAD_STACK_SIZE        4096
#define CFG_HTTPD_THREAD_PRIORITY          osPriorityNormal


#endif
