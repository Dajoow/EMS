#include "http_client.h"
#include "at24cxx.h"
#include "cJSON.h"
#include "ca_certificates.h"
#include "cmsis_os.h"
#include "lwip/api.h"
#include "lwip/dhcp.h"
#include "lwip/init.h"
#include "lwip/sockets.h"
#include "sntp_client.h"
#include "station_ctl.h"
#include "string.h"
#include "task.h"
#include "usart.h"
#include "wolfssl/wolfcrypt/coding.h"

#define SERVER_TOKEN "8cXNY4p+0HfK4Snvt4QLcQ=="

#if !defined(SERVER_TOKEN)
#warning                                                                      \
    "HTTPS SERVER TOKEN NOT DEFINED, DEFINE A TOKEN TO ENABLE HTTPS CLIENT"
#else
#define HTTPC_ENABLED
#endif

#define HOST "dz.hhkk.club"
#define HTTPS_PORT 82
#define HTTP_PORT 81
#define HTTP_HOST "dz.hhkk.club:81"
#define HTTPS_HOST "dz.hhkk.club:82"
#define API_STATISTICS "/api/bmsRequest/sendSum"
#define API_CLUSTERS "/api/bmsRequest/send"

#define HTTP_HEADER(api)                                                      \
  "POST "##api " HTTP/1.1\r\n"                                                \
               "HOST: " HTTPS_HOST "\r\n"                                     \
               "Accept: application/json\r\n"                                 \
               "Connection: keep-alive\r\n"                                   \
               "User-Agent: BSMU LWIP/" LWIP_VERSION_STRING "\r\n"            \
               "Content-Length: %d\r\n"                                       \
               "\r\n"

extern struct netif gnetif;
extern Client_Sd_t Client_Sd[cluster_num];
extern Client_Sd_Station_t Client_Sd_Station;

extern EEPROM_BSMU bsmuSetting;

osThreadId httpc_handle = NULL;

Client_Sd_Station_t httpc_station_statistics __attribute__((at(0xC040E330)));
Client_Sd_t httpc_clusters[cluster_num] __attribute__((at(0xC0400000)));

httpc_ctx_t http_client;

int
my_IORecv (WOLFSSL *ssl, char *buff, int sz, void *ctx)
{
  /* By default, ctx will be a pointer to the file descriptor to read from.
   * This can be changed by calling wolfSSL_SetIOReadCtx(). */
  int sockfd = *(int *)ctx;
  int recvd;

  /* Receive message from socket */
  if ((recvd = recv (sockfd, buff, sz, 0)) == -1)
    {
      /* error encountered. Be responsible and report it in wolfSSL terms */

      Debug_printf ("IO RECEIVE ERROR: ");
      switch (errno)
        {
#if EAGAIN != EWOULDBLOCK
        case EAGAIN: /* EAGAIN == EWOULDBLOCK on some systems, but not others
                      */
#endif
        case EWOULDBLOCK:
          if (!wolfSSL_dtls (ssl) || wolfSSL_get_using_nonblock (ssl))
            {
              Debug_printf ("would block\n");
              return WOLFSSL_CBIO_ERR_WANT_READ;
            }
          else
            {
              Debug_printf ("socket timeout\n");
              return WOLFSSL_CBIO_ERR_TIMEOUT;
            }
        case ECONNRESET:
          Debug_printf ("connection reset\n");
          return WOLFSSL_CBIO_ERR_CONN_RST;
        case EINTR:
          Debug_printf ("socket interrupted\n");
          return WOLFSSL_CBIO_ERR_ISR;
        case ECONNREFUSED:
          Debug_printf ("connection refused\n");
          return WOLFSSL_CBIO_ERR_WANT_READ;
        case ECONNABORTED:
          Debug_printf ("connection aborted\n");
          return WOLFSSL_CBIO_ERR_CONN_CLOSE;
        default:
          Debug_printf ("general error\n");
          return WOLFSSL_CBIO_ERR_GENERAL;
        }
    }
  else if (recvd == 0)
    {
      Debug_printf ("Connection closed\n");
      return WOLFSSL_CBIO_ERR_CONN_CLOSE;
    }

  /* successful receive */
  // Debug_printf ("my_IORecv: received %d bytes from %d\n", sz, sockfd);

  return recvd;
}

int
my_IOSend (WOLFSSL *ssl, char *buff, int sz, void *ctx)
{
  /* By default, ctx will be a pointer to the file descriptor to write to.
   * This can be changed by calling wolfSSL_SetIOWriteCtx(). */
  int sockfd = *(int *)ctx;
  int sent;

  /* Receive message from socket */
  if ((sent = send (sockfd, buff, sz, 0)) == -1)
    {
      /* error encountered. Be responsible and report it in wolfSSL terms */

      Debug_printf ("IO SEND ERROR: ");
      switch (errno)
        {
#if EAGAIN != EWOULDBLOCK
        case EAGAIN: /* EAGAIN == EWOULDBLOCK on some systems, but not others
                      */
#endif
        case EWOULDBLOCK:
          Debug_printf ("would block\n");
          return WOLFSSL_CBIO_ERR_WANT_WRITE;
        case ECONNRESET:
          Debug_printf ("connection reset\n");
          return WOLFSSL_CBIO_ERR_CONN_RST;
        case EINTR:
          Debug_printf ("socket interrupted\n");
          return WOLFSSL_CBIO_ERR_ISR;
        case EPIPE:
          Debug_printf ("socket EPIPE\n");
          return WOLFSSL_CBIO_ERR_CONN_CLOSE;
        default:
          Debug_printf ("general error %d\n", errno);
          return WOLFSSL_CBIO_ERR_GENERAL;
        }
    }
  else if (sent == 0)
    {
      Debug_printf ("Connection closed\n");
      return 0;
    }

  /* successful send */
  // Debug_printf ("my_IOSend: sent %d bytes to %d\n", sz, sockfd);
  return sent;
}

static int
create_statistics_payload (httpc_ctx_t *ctx)
{
  int len = -1;
  Client_Sd_Station_t *data = ctx->statistics_data;

  cJSON *obj = cJSON_CreateObject ();
  if (obj == NULL)
    goto end;

  // todo: debug only clear it on relaease
  // data->station_state = 2;
  // data->station_VOL = 200;
  // data->station_CUR = -20;
  // data->station_SOC = 98;
  // data->station_SOH = 99;
  // data->charge_power = 1200;
  // data->discharge_power = 600;

  cJSON *sta = cJSON_CreateNumber (data->station_state);
  if (sta == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "sta", sta);
  cJSON *vs = cJSON_CreateNumber (data->station_VOL);
  if (vs == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "vs", vs);
  cJSON *is = cJSON_CreateNumber (data->station_CUR);
  if (is == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "is", is);
  cJSON *soc = cJSON_CreateNumber (data->station_SOC);
  if (soc == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "soc", soc);
  cJSON *soh = cJSON_CreateNumber (data->station_SOH);
  if (soh == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "soh", soh);
  cJSON *cp = cJSON_CreateNumber (data->charge_power);
  if (cp == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "cp", cp);
  cJSON *dp = cJSON_CreateNumber (data->discharge_power);
  if (dp == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "dp", dp);
  cJSON *token = cJSON_CreateString (SERVER_TOKEN);
  if (token == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "token", token);

  ctx->json_statistics = cJSON_Print (obj);
  if (ctx->json_statistics == NULL)
    {
      Debug_printf ("cJSON print error json_statistics\r\n");
      goto end;
    }

  len = strlen (ctx->json_statistics);
  ctx->json_statistics_len = len;

end:
  cJSON_Delete (obj);
  return len;
}

static int
create_clusters_payload (httpc_ctx_t *ctx, int index)
{
  int len = -1;
  Client_Sd_t *data = &ctx->clusters_data[index];
  char base64_buffer[1024];
  int base64_buffer_len = sizeof (base64_buffer);

  cJSON *obj = cJSON_CreateObject ();
  if (obj == NULL)
    goto end;

  cJSON *sid = cJSON_CreateNumber (data->cluster_No);
  if (sid == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "sid", sid);

  cJSON *sta = cJSON_CreateNumber (data->work_state);
  if (sta == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "sta", sta);

  cJSON *vs = cJSON_CreateNumber (data->cluster_VOL);
  if (vs == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "vs", vs);

  cJSON *is = cJSON_CreateNumber (data->cluster_CUR);
  if (is == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "is", is);

  cJSON *soc = cJSON_CreateNumber (data->cluster_SOC);
  if (soc == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "soc", soc);

  cJSON *soh = cJSON_CreateNumber (data->cluster_SOH);
  if (soh == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "soh", soh);

  cJSON *rp = cJSON_CreateNumber (data->insulation_res_p);
  if (rp == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "rp", rp);

  cJSON *rn = cJSON_CreateNumber (data->insulation_res_n);
  if (rn == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "rn", rn);

  cJSON *un = cJSON_CreateNumber (data->grp_num);
  if (un == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "un", un);

  cJSON *bn = cJSON_CreateNumber (data->grp_bat_num);
  if (bn == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "bn", bn);

  Base64_Encode_NoNl ((uint8_t *)data->BAT_VOL,
                      TOTOL_BAT_num * sizeof (uint16_t), base64_buffer,
                      &base64_buffer_len);
  cJSON *vb = cJSON_CreateString (base64_buffer);
  if (vb == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "vb", vb);

  Base64_Encode_NoNl ((uint8_t *)data->BAT_TMP,
                      TOTOL_BAT_num * sizeof (uint16_t), base64_buffer,
                      &base64_buffer_len);
  cJSON *tb = cJSON_CreateString (base64_buffer);
  if (tb == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "tb", tb);

  Base64_Encode_NoNl ((uint8_t *)data->BAT_SOC,
                      TOTOL_BAT_num * sizeof (uint16_t), base64_buffer,
                      &base64_buffer_len);
  cJSON *socb = cJSON_CreateString (base64_buffer);
  if (socb == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "socb", socb);

  Base64_Encode_NoNl ((uint8_t *)data->BAT_FAULT,
                      TOTOL_BAT_num * sizeof (uint16_t), base64_buffer,
                      &base64_buffer_len);
  cJSON *wb = cJSON_CreateString (base64_buffer);
  if (wb == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "wb", wb);

  cJSON *token = cJSON_CreateString (SERVER_TOKEN);
  if (token == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "token", token);

  ctx->json_clusters = cJSON_Print (obj);
  if (ctx->json_clusters == NULL)
    {
      Debug_printf ("cJSON print error json_clusters\r\n");
      goto end;
    }

  len = strlen (ctx->json_clusters);
  ctx->json_clusters_len = len;

end:
  cJSON_Delete (obj);
  return len;
}

static int
httpc_ctx_init_default (httpc_ctx_t *ctx)
{
  ctx->json_statistics = NULL;
  ctx->json_statistics_len = -1;
  ctx->json_clusters = NULL;
  ctx->json_clusters_len = -1;
  ctx->statistics_data = &httpc_station_statistics;
  ctx->clusters_data = httpc_clusters;
  ctx->clusters_sum = bsmuSetting.cu_num;
  ctx->socket = -1;
  ctx->ssl_ctx = NULL;
  ctx->ssl = NULL;

  if (wolfSSL_Init () != WOLFSSL_SUCCESS)
    {
      Debug_printf ("Wolfssl init failed\r\n");
      return -1;
    }

  ctx->ssl_ctx = wolfSSL_CTX_new (wolfTLSv1_2_client_method ());
  if (ctx->ssl_ctx == NULL)
    {
      Debug_printf ("ssl ctx alloc failed\r\n");
      return -1;
    }

  if (wolfSSL_CTX_load_verify_buffer (ctx->ssl_ctx, __ssl_ca_certificate,
                                      __ssl_ca_certificate_len,
                                      WOLFSSL_FILETYPE_PEM)
      != SSL_SUCCESS)
    {
      Debug_printf ("load ca certificate failed\r\n");
      return -1;
    }

  // todo: remove it aftr solve the problem
  // workaround to avoid -188, -155 temporary
  wolfSSL_CTX_set_verify (ctx->ssl_ctx, SSL_VERIFY_NONE, 0);

  /* Register callbacks */
  wolfSSL_SetIORecv (ctx->ssl_ctx, my_IORecv);
  wolfSSL_SetIOSend (ctx->ssl_ctx, my_IOSend);

  return 0;
}

static int cjson_init(){
  cJSON_Hooks hooks;

  hooks.free_fn = vPortFree;
  hooks.malloc_fn = pvPortMalloc;

  cJSON_InitHooks (&hooks);

  return 0;
}

static int
socket_connect (httpc_ctx_t *ctx, const char *host, int port)
{
  // dns resolve
  ip_addr_t host_ip;
  err_t err = ERR_OK;
  err = netconn_gethostbyname (host, &host_ip);
  if (err != ERR_OK)
    {
      Debug_printf ("Host resolve error %d\r\n", err);
      return -1;
    }

  if (host_ip.addr == 0)
    {
      Debug_printf ("ip addr error\r\n");
      return -1;
    }

  // 进行 socket 连接
  struct sockaddr_in servaddr;

  /* Create a socket that uses an internet IPv4 address,
   * Sets the socket to be stream based (TCP),
   * 0 means choose the default protocol. */
  ctx->socket = socket (AF_INET, SOCK_STREAM, 0);

  memset (&servaddr, sizeof (servaddr), 0);
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons (HTTPS_PORT);

  inet_pton (AF_INET, inet_ntoa (host_ip), &servaddr.sin_addr);

  /* Connect to socket file descriptor */
  if (0
      != connect (ctx->socket, (struct sockaddr *)&servaddr,
                  sizeof (servaddr)))
    {
      Debug_printf ("socket connect failed\r\n");
    }

  return 0;
}

static int
httpc_connect (httpc_ctx_t *ctx, const char *host, int port)
{
  socket_connect (ctx, host, port);

  if ((ctx->ssl = wolfSSL_new (ctx->ssl_ctx)) == NULL)
    {
      Debug_printf ("SSL new failed\r\n");
    }

  // 进行 SSL 和 socket 绑定
  if (WOLFSSL_SUCCESS != wolfSSL_set_fd (ctx->ssl, ctx->socket))
    {
      Debug_printf ("https socket set failed\r\n");
    }

  return 0;
}

static int
httpc_disconnect (httpc_ctx_t *ctx)
{
  wolfSSL_free (ctx->ssl);
  wolfSSL_CTX_free (ctx->ssl_ctx);
  wolfSSL_Cleanup ();
  closesocket (ctx->socket);

  return 0;
}

static int
https_send (httpc_ctx_t *ctx, const char *pkg, int len)
{
  if (wolfSSL_write (ctx->ssl, pkg, len) != len)
    {
      Debug_printf ("ssl write failed\r\n");
    }

  // HAL_UART_Transmit (&huart4, pkg, len, 0xff);

  return 0;
}

#define REC_BUFFER_LEN 256
static int
httpc_recv (httpc_ctx_t *ctx)
{
  char buffer[REC_BUFFER_LEN];
  char *ret = NULL;
  char content_len[4]; // length always < 999
  int err_code = 0;

  if (wolfSSL_read (ctx->ssl, buffer, sizeof (buffer)) == 0)
    {
      Debug_printf ("ssl read failed\r\n");
      return -1;
    }

  if (ret = strstr (buffer, "HTTP/1.1 "))
    {
      char *mime_ptr = NULL;
      mime_ptr = strstr (ret, " ");
      if (mime_ptr != NULL)
        {
          mime_ptr += 1;

          while (*mime_ptr && (*mime_ptr == ' ' || *mime_ptr == '\t'))
            mime_ptr++;
        }
      err_code = atoi (mime_ptr);
    }

  if (err_code != 200)
    {
      return err_code;
    }

  if (ret = strstr (buffer, "code"))
    {
      char *mime_ptr = NULL;
      mime_ptr = strstr (ret, " ");
      if (mime_ptr != NULL)
        {
          mime_ptr += 1;

          while (*mime_ptr && (*mime_ptr == ',' || *mime_ptr == '\t'))
            mime_ptr++;
        }
      err_code = atoi (mime_ptr);
    }

  return err_code;
}


static int
httpc_send_data_statistics (httpc_ctx_t *ctx)
{
  char http_header[284];

  memcpy (ctx->statistics_data, &Client_Sd_Station,
          sizeof (Client_Sd_Station_t));
  create_statistics_payload (ctx);

  int http_header_len
      = snprintf (http_header, sizeof (http_header),
                  HTTP_HEADER (API_STATISTICS), ctx->json_statistics_len);
  https_send (ctx, http_header, http_header_len);
  https_send (ctx, ctx->json_statistics, ctx->json_statistics_len);

  // free josn string
  vPortFree(ctx->json_statistics);
  ctx->json_statistics = NULL;
  ctx->json_statistics_len = -1;

  return 0;
}

static int
httpc_send_data_clusters (httpc_ctx_t *ctx)
{
  int ret;
  char http_header[284];

  memcpy (ctx->clusters_data, &Client_Sd, sizeof (Client_Sd_t) * cluster_num);

  for (int i = 0; i < ctx->clusters_sum; i++)
    {
      // todo: comment it after debug
      // note: debug only
      // ctx->clusters_data[i].cluster_No = i + 1;
      // ctx->clusters_data[i].work_state = i;
      // ctx->clusters_data[i].grp_bat_num = 12;
      // ctx->clusters_data[i].grp_num = 30;
      // ctx->clusters_data[i].cluster_SOC = 100;
      // ctx->clusters_data[i].cluster_SOH = 100;

      // skip invalid data
      if (ctx->clusters_data[i].cluster_No > 20
          || ctx->clusters_data[i].cluster_No < 1)
        {
          continue;
        }

      ret = create_clusters_payload (ctx, i);

      if (ret == -1)
        {
          Debug_printf ("create clusters payload failed\r\n");
          break;
        }

      int http_header_len
          = snprintf (http_header, sizeof (http_header),
                      HTTP_HEADER (API_CLUSTERS), ctx->json_clusters_len);

      ret = https_send (ctx, http_header, http_header_len);
      ret = https_send (ctx, ctx->json_clusters, ctx->json_clusters_len);

      // free josn string
      vPortFree(ctx->json_clusters);
      ctx->json_clusters = NULL;
      ctx->json_clusters_len = -1;
    }

  return ret;
}

static void
httpc_task (void const *args)
{
  while (dhcp_supplied_address (&gnetif) == 0)
    {
      Debug_printf ("https client: waiting for DHCP\r\n");
      osDelay (2000);
    }
    
  // wolfSSL_Debugging_ON ();

  cjson_init ();
  httpc_ctx_init_default (&http_client);

  while (1)
    {
      int ret = 0;
      struct dhcp *dhcp;

      // todo: clear it
      // warning: this is a hack to display ip addr, do not use in productive
      // environment
      dhcp = netif_dhcp_data (&gnetif);
      memcpy (bsmuSetting.IP_ADD, &dhcp->offered_ip_addr, 4);
      memcpy (bsmuSetting.NETMASK, &dhcp->offered_sn_mask, 4);
      memcpy (bsmuSetting.GATEWAY, &dhcp->offered_gw_addr, 4);

      httpc_connect (&http_client, HOST, HTTPS_PORT);

      httpc_send_data_statistics (&http_client);
      ret = httpc_recv (&http_client);
      if(ret){
          Debug_printf ("statistics data send failed %d\r\n", ret);
          // todo: inform UI
      }

      httpc_send_data_clusters (&http_client);
      ret = httpc_recv (&http_client);
      if(ret){
          Debug_printf ("clusters data send failed %d\r\n", ret);
        // todo: inform UI
      }

      wolfSSL_free (http_client.ssl);
      closesocket (http_client.socket);
      osDelay (1000);
    }

  httpc_disconnect (&http_client);
}

void
http_client_init (void)
{
#ifdef HTTPC_ENABLED
  osThreadDef (http_client, httpc_task, 1, 0, 2 * 1024);
  httpc_handle = osThreadCreate (osThread (http_client), NULL);
#endif
}
