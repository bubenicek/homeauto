#include "ets_sys.h"
#include "mem.h"
#include "osapi.h"
#include "os_type.h"
#include "user_config.h"
#include "user_interface.h"
#include "smartconfig.h"
#include "espconn.h"

#include "driver/ds18b20.h"
#include "debug.h"
#include "utils.h"
#include "httpclient.h"

#define TRACE_TAG    "temp"

// Types:
typedef struct
{
   uint8 ssid[32];
   uint8 password[64];

} cfg_params_t;

// Prototypes:
bool ICACHE_FLASH_ATTR smartconfig_is_enabled(void);
static void ICACHE_FLASH_ATTR sleep_mode(void);
static void ICACHE_FLASH_ATTR timer_callback(void *arg);
static void ICACHE_FLASH_ATTR http_sent_callback(char * response, int http_status, char * full_response);
static void ICACHE_FLASH_ATTR wifi_event_callback(System_Event_t *evt);
static void ICACHE_FLASH_ATTR smartconfig_done_callback(sc_status status, void *pdata);

// Locals:
static cfg_params_t cfg_params;
static uint32_t start_time;
static ETSTimer timer;
static struct mdns_info mdns_info;

// if you are using client api only, all you need is to define the variables in your user_main.c
unsigned char *default_certificate;
unsigned int default_certificate_len = 0;
unsigned char *default_private_key;
unsigned int default_private_key_len = 0;


void user_rf_pre_init(void)
{
   // Set maximum value of RF TX Power, unit :0.25dBm range 0 -82
   system_phy_set_max_tpw(0);
}

// Init function
void ICACHE_FLASH_ATTR user_init()
{
   struct rst_info *rst_info;
   struct ip_info ip_info;
   struct station_config station_conf;

   lwip_init();

   start_time = os_time_ms();

#ifndef DEBUG
   // Disable system debug output
   system_set_os_print(0);
#endif

   //
   // Set UART RX as gpio pin for enable/disable smartconfig
   //
   PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_GPIO3);
   // Enable pull up R
   PIN_PULLUP_EN(PERIPHS_IO_MUX_U0RXD_U);
   // Enable input
   GPIO_DIS_OUTPUT(FUNC_GPIO3);

   // Set sleep type
   //wifi_set_sleep_type(LIGHT_SLEEP_T);

   // Get Reset info
   rst_info = system_get_rst_info();

   uart_div_modify(0, UART_CLK_FREQ / 115200);
   TRACE_PRINTF("\n");
   TRACE("Temp sensor - SDK ver: %s  reset_reason: %d", system_get_sdk_version(), rst_info->reason);

   // Register wifi callback
   wifi_set_event_handler_cb(wifi_event_callback);

   // Set station mode
   wifi_set_opmode(STATION_MODE);

   if (smartconfig_is_enabled())
   {
      // Start smart configuration mode
      smartconfig_start(smartconfig_done_callback);
   }
   else
   {
      if (rst_info->reason == REASON_DEEP_SLEEP_AWAKE)
      {
         // Use static IP
         TRACE("Using static IP cfg");

         // Stop dhcp client
         wifi_station_dhcpc_stop();

         // Read dhcp settings from RTC RAM
         system_rtc_mem_read(64, &ip_info, sizeof(struct ip_info));

         // Configure interface
         if (!wifi_set_ip_info(STATION_IF, &ip_info))
         {
            TRACE_ERROR("Set static IP failed, using dhcp");
            wifi_station_dhcpc_start();
         }
      }
      else
      {
         // Use DHCP
         TRACE("Using DHCP IP cfg");
      }

      // Set wifi settings from stored configuration
      system_param_load(ESP_PARAM_START_SEC, 0, &cfg_params, sizeof(cfg_params));

      os_memcpy(&station_conf.ssid, &cfg_params.ssid, sizeof(station_conf.ssid));
      os_memcpy(&station_conf.password, &cfg_params.password, sizeof(station_conf.password));
      wifi_station_set_config_current(&station_conf);
   }
}

bool ICACHE_FLASH_ATTR smartconfig_is_enabled(void)
{
   return (GPIO_INPUT_GET(FUNC_GPIO3) == 0);
}

static void ICACHE_FLASH_ATTR sleep_mode(void)
{
   // Radio calibration ON
   system_deep_sleep_set_option(0);
   // Goto sleep mode
   system_deep_sleep(CFG_SLEEP_SECONDS * 1000 * 1000);
}


//
// Callbacks
//

/** Temperature measurement event timer */
static void ICACHE_FLASH_ATTR timer_callback(void *arg)
{
   char temp[20];
   static char buf[1024];

   // Get temperature
   ds_read_temp(temp, sizeof(temp));

   // http://api.thingspeak.com/update?key=A7E6HESRP8IDOM09&field1=1&field2=2&field3=3&field4=4
   os_sprintf(buf, "%s?key=%s&field1=%d&field2=%s&field3=%d&field4=%d",
              WRITE_URL, WRITE_KEY, os_time_ms(), temp, system_get_vdd33(), wifi_station_get_rssi());

   TRACE("Send: %s", buf);

   // Send temperature
   http_get(buf, "", http_sent_callback);
}

/** Callback invoked after http packet sent */
static void ICACHE_FLASH_ATTR  http_sent_callback(char * response, int http_status, char * full_response)
{
   TRACE("Http_status: %d", http_status);
   if (http_status != HTTP_STATUS_GENERIC_ERROR)
   {
      TRACE("Response: %s", response);
   }

   TRACE("Total exec time: %d ms", os_time_ms() - start_time);

   // Goto sleep mode
   sleep_mode();
}

/** Wifi events callback */
static void ICACHE_FLASH_ATTR wifi_event_callback(System_Event_t *evt)
{
   switch (evt->event)
   {
   case EVENT_STAMODE_CONNECTED:
      TRACE("Connect to ssid: %s, channel: %d",
            evt->event_info.connected.ssid,
            evt->event_info.connected.channel);
      break;

   case EVENT_STAMODE_DISCONNECTED:
      TRACE("Disconnect from ssid: %s, reason: %d",
            evt->event_info.disconnected.ssid,
            evt->event_info.disconnected.reason);

      if (evt->event_info.disconnected.reason != REASON_ASSOC_LEAVE)
      {
         sleep_mode();
      }
      break;

   case EVENT_STAMODE_AUTHMODE_CHANGE:
      TRACE("mode: %d -> %d",
            evt->event_info.auth_change.old_mode,
            evt->event_info.auth_change.new_mode);
      break;

   case EVENT_STAMODE_GOT_IP:
   {
      struct ip_info ip_info =
      {
         .ip = evt->event_info.got_ip.ip,
         .gw = evt->event_info.got_ip.gw,
         .netmask = evt->event_info.got_ip.mask
      };

      TRACE("IP:" IPSTR ", mask:" IPSTR ", gw:" IPSTR,
            IP2STR(&evt->event_info.got_ip.ip),
            IP2STR(&evt->event_info.got_ip.mask),
            IP2STR(&evt->event_info.got_ip.gw));

      // Save IP config to RTC RAM
      system_rtc_mem_write(64, &ip_info, sizeof(struct ip_info));

      // Start temperature mesurement
      ds_start();

      // Start measurement timer
      os_timer_disarm(&timer);
      os_timer_setfn(&timer, timer_callback, NULL);
      os_timer_arm(&timer, 1000, 0);
   }
   break;

   case EVENT_SOFTAPMODE_STACONNECTED:
      TRACE("station: " MACSTR "join, AID = %d",
            MAC2STR(evt->event_info.sta_connected.mac),
            evt->event_info.sta_connected.aid);
      break;
   }
}

/** Smartconfig events callback */
static void ICACHE_FLASH_ATTR smartconfig_done_callback(sc_status status, void *pdata)
{
   switch(status)
   {
   case SC_STATUS_WAIT:
      TRACE("SC_STATUS_WAIT");
      break;

   case SC_STATUS_FIND_CHANNEL:
      TRACE("SC_STATUS_FIND_CHANNEL");
      break;

   case SC_STATUS_GETTING_SSID_PSWD:
      TRACE("SC_STATUS_GETTING_SSID_PSWD");
      sc_type *type = pdata;
      if (*type == SC_TYPE_ESPTOUCH)
      {
         TRACE("SC_TYPE:SC_TYPE_ESPTOUCH");
      }
      else
      {
         TRACE("SC_TYPE:SC_TYPE_AIRKISS");
      }
      break;

   case SC_STATUS_LINK:
   {
      struct station_config *sta_conf = pdata;

      memcpy(cfg_params.ssid, sta_conf->ssid, sizeof(cfg_params.ssid));
      memcpy(cfg_params.password, sta_conf->password, sizeof(cfg_params.password));

      // Save SSID and pasword to flash
      system_param_save_with_protect(ESP_PARAM_START_SEC, &cfg_params, sizeof(cfg_params));

      TRACE("SC_STATUS_LINK, cfg saved");

      wifi_station_set_config(sta_conf);
      wifi_station_disconnect();
      wifi_station_connect();
   }
   break;

   case SC_STATUS_LINK_OVER:
      TRACE("SC_STATUS_LINK_OVER");
      if (pdata != NULL)
      {
         uint8 phone_ip[4] = {0};
         os_memcpy(phone_ip, (uint8*)pdata, 4);
         TRACE("Phone ip: %d.%d.%d.%d", phone_ip[0], phone_ip[1], phone_ip[2], phone_ip[3]);
      }
      smartconfig_stop();
      break;
   }
}
