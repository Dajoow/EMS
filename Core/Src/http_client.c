#include "cmsis_os.h"
#include "task.h"
#include "lwip/dhcp.h"
#include "at24cxx.h"
#include "string.h"
#include "sntp_client.h"
#include "usart.h"

extern struct netif gnetif;
extern EEPROM_BSMU bsmuSetting;

osThreadId httpc_handle = NULL;

static httpc_task(void *args){
    while(1){
		struct dhcp *dhcp;

		//todo: clear it
		// warning: this is a hack to display ip addr, do not use in productive environment
		dhcp = netif_dhcp_data (&gnetif);
		memcpy (bsmuSetting.IP_ADD, &dhcp->offered_ip_addr, 4);
		memcpy (bsmuSetting.NETMASK, &dhcp->offered_sn_mask, 4);
		memcpy (bsmuSetting.GATEWAY, &dhcp->offered_gw_addr, 4);

        // Debug_printf ("timestamp:%lu\r\n", rtc_get_timestamp ());

        osDelay (1000);
    }
}

void http_client_init(void){
	osThreadDef(http_client, httpc_task, osPriorityNormal, 0, 512);
	httpc_handle = osThreadCreate(osThread(http_client), NULL);
}
