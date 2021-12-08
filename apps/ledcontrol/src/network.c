
#include "system.h"
#include "rest_api.h"

#define TRACE_TAG "network"
#if !ENABLE_TRACE_MAIN
#undef TRACE
#define TRACE(...)
#endif

// Prototypes:
static void hal_net_event_handler(hal_netif_t netif, hal_netif_event_t event);

// Locals:
static httpd_t httpd;
static bool connected;


int network_init(void)
{
    hal_netif_config_t netconf = {
        .wifi.ssid = CFG_NET_SSID,
        .wifi.passwd = CFG_NET_PASSWD,
        .dhcp_enabled = false,
        .ipaddr = CFG_NET_IPADDR,
        .gw = CFG_NET_GW,
        .netmask = CFG_NET_NETMASK,
    };

    connected = false;

    // Configure network interface
    if (hal_net_configure(CFG_NETIF, &netconf, hal_net_event_handler) != 0)
    {
        TRACE_ERROR("Configure network failed");
        return -1;
    }

  // Start http server
   if (httpd_init(&httpd, HTTPD_PORT) != 0)
   {
      TRACE_ERROR("Start httpd server");
      return -1;
   }
   TRACE("HTTPD initialized");

    return 0;
}

bool network_is_connected(void)
{
    return connected;
}

static void hal_net_event_handler(hal_netif_t netif, hal_netif_event_t event)
{
    switch(event)
    {
        case HAL_NETIF_EVENT_CONNECTED:
            connected = true;
            break;

        case HAL_NETIF_EVENT_DISCONNECTED:
            connected = false;
            break;
    }
}
