#include <string.h>
#include "lwip/api.h"
#include "lwip/netif.h"
#include "debug.h"

uint32_t pp_rx_pool_usage = 0;
uint32_t netif_num = 0;
struct netif *netif_list = NULL;
struct netif *netif_default = NULL;
struct tcp_pcb *tcp_active_pcbs = NULL;
const ip_addr_t ip_addr_any = IPADDR4_INIT(IPADDR_ANY);

typedef void (*tcpip_init_done_fn)(void *arg);
typedef void (*tcpip_callback_fn)(void *ctx);
typedef err_t (*netif_init_fn)(struct netif *netif);
typedef err_t (*netif_input_fn)(struct pbuf *p, struct netif *inp);

typedef err_t (*netif_output_fn)(struct netif *netif, struct pbuf *p,
       const ip4_addr_t *ipaddr);

sys_prot_t sys_arch_protect(void)
{
   return 0;
}

void sys_arch_unprotect(sys_prot_t xValue)
{
}

void netif_set_default(struct netif *netif)
{
   TRACE_INFO("stub netif_set_default\r\n");
   netif_default = netif;
}

struct netif *netif_add(struct netif *netif, const ip4_addr_t *ipaddr,
   const ip4_addr_t *netmask, const ip4_addr_t *gw, void *state,
   netif_init_fn init, netif_input_fn input)
{
   uint8_t macAddr[6];

   TRACE_INFO("stub netif_add\r\n");

   memcpy(macAddr, netif->hwaddr, 6);
   memset(netif, 0, sizeof(struct netif));
   memcpy(netif->hwaddr, macAddr, 6);

   netif->state = state;
   netif->num = netif_num++;
   netif->input = input;

   if(init(netif) != ERR_OK)
      return NULL;

   netif->next = netif_list;
   netif_list = netif;

   return netif;
}

void netif_set_addr(struct netif *netif, const ip4_addr_t *ipaddr,
   const ip4_addr_t *netmask, const ip4_addr_t *gw)
{
   TRACE_INFO("stub netif_set_addr\r\n");
}

err_t ethernetif_init(struct netif *netif)
{
   TRACE_INFO("stub ethernetif_init\r\n");
   netif->flags = NETIF_FLAG_LINK_UP;
   return ERR_OK;
}

void tcp_free_ooseq(struct tcp_pcb *pcb)
{
   TRACE_INFO("stub tcp_free_ooseq\r\n");
}

void tcpip_init(tcpip_init_done_fn tcpip_init_done, void *arg)
{
   TRACE_INFO("stub tcpip_init\r\n");
}

err_t  tcpip_input(struct pbuf *p, struct netif *inp)
{
   TRACE_INFO("stub tcpip_input\r\n");
   return ERR_OK;
}

err_t  tcpip_try_callback(tcpip_callback_fn function, void *ctx)
{
   TRACE_INFO("stub tcpip_try_callback\r\n");
   return ERR_OK;
}

err_t etharp_output(struct netif *netif, struct pbuf *q,
   const ip4_addr_t *ipaddr)
{
   TRACE_INFO("stub etharp_output\r\n");
   return ERR_OK;
}

err_t dhcp_start(struct netif *netif)
{
   TRACE_INFO("stub dhcp_start\r\n");
   return ERR_OK;
}

err_t dhcp_stop(struct netif *netif)
{
   TRACE_INFO("stub dhcp_stop\r\n");
   return ERR_OK;
}

void dhcp_release_and_stop(struct netif *netif)
{
   TRACE_INFO("stub dhcp_release_and_stop\r\n");
}

u8_t dhcp_supplied_address(const struct netif *netif)
{
   TRACE_INFO("stub dhcp_supplied_address\r\n");
   return 0;
}

ssize_t lwip_write(int s, const void *data, size_t size)
{
   TRACE_INFO("stub lwip_write\r\n");
   return -1;
}

ssize_t lwip_read(int s, void *mem, size_t len)
{
   TRACE_INFO("stub lwip_read\r\n");
   return -1;
}

int lwip_close(int s)
{
   TRACE_INFO("stub lwip_close\r\n");
   return 0;
}

void pp_recycle_rx_pbuf(struct pbuf *p)
{
   sdk_system_pp_recycle_rx_pkt(p->esf_buf);
   taskENTER_CRITICAL();
   pp_rx_pool_usage--;
   taskEXIT_CRITICAL();
}

struct pbuf *sdk_pbuf_alloc(pbuf_layer layer, u16_t length, pbuf_type type)
{
   struct pbuf *p;

   if(type == 0)
      p = pbuf_alloc(PBUF_RAW_TX, length, PBUF_RAM);
   else if(type == 2)
      p = pbuf_alloc_reference(NULL, length, PBUF_ALLOC_FLAG_RX | PBUF_TYPE_ALLOC_SRC_MASK_ESP_RX);
   else
      p = NULL;

   return p;
}
