#include "http_client.h"
#include "at24cxx.h"
#include "cJSON.h"
#include "ca_certificates.h"
#include "cmsis_os.h"
#include "lwip/api.h"
#include "lwip/dhcp.h"
#include "lwip/init.h"
#include "lwip/sockets.h"
#include "mbedtls.h"
#include "mbedtls/base64.h"
#include "mbedtls/net_sockets.h"
#include "sntp_client.h"
#include "station_ctl.h"
#include "string.h"
#include "task.h"
#include "usart.h"

// #define SERVER_TOKEN

#if !defined(SERVER_TOKEN)
#warning                                                                      \
    "HTTPS SERVER TOKEN NOT DEFINED, DEFINE A TOKEN TO ENABLE HTTPS CLIENT"
#define SERVER_TOKEN "NULL"
#else
#define HTTPC_ENABLED
#endif

#define HOST "dzhk.hhkk.club"
#define HTTPS_PORT "443"
#define HTTP_PORT "80"
#define HTTP_HOST "dzhk.hhkk.club"
#define HTTPS_HOST "dzhk.hhkk.club"
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

Client_Sd_Station_t httpc_station_statistics __attribute__ ((at (0xC040E330)));
Client_Sd_t httpc_clusters[cluster_num] __attribute__ ((at (0xC0400000)));

httpc_ctx_t http_client;

static const char *pers = "ssl_client";
static const int pers_len = sizeof (pers);

static mbedtls_net_context server_fd;

static int
create_statistics_payload (httpc_ctx_t *ctx)
{
  int len = -1;
  Client_Sd_Station_t *data = ctx->statistics_data;

  cJSON *obj = cJSON_CreateObject ();
  if (obj == NULL)
    goto end;

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
  int ret = -1;
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

  ret = mbedtls_base64_encode (base64_buffer, &base64_buffer_len, NULL,
                               (uint8_t *)data->BAT_VOL,
                               TOTOL_BAT_num * sizeof (uint16_t));
  if (ret != 0)
    {
      Debug_printf ("MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL\r\n");
    }
  cJSON *vb = cJSON_CreateString (base64_buffer);
  if (vb == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "vb", vb);

  ret = mbedtls_base64_encode (base64_buffer, &base64_buffer_len, NULL,
                               (uint8_t *)data->BAT_TMP,
                               TOTOL_BAT_num * sizeof (uint16_t));
  if (ret != 0)
    {
      Debug_printf ("MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL\r\n");
    }
  cJSON *tb = cJSON_CreateString (base64_buffer);
  if (tb == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "tb", tb);

  ret = mbedtls_base64_encode (base64_buffer, &base64_buffer_len, NULL,
                               (uint8_t *)data->BAT_SOC,
                               TOTOL_BAT_num * sizeof (uint16_t));
  if (ret != 0)
    {
      Debug_printf ("MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL\r\n");
    }
  cJSON *socb = cJSON_CreateString (base64_buffer);
  if (socb == NULL)
    goto end;
  cJSON_AddItemToObject (obj, "socb", socb);

  ret = mbedtls_base64_encode (base64_buffer, &base64_buffer_len, NULL,
                               (uint8_t *)data->BAT_FAULT,
                               TOTOL_BAT_num * sizeof (uint16_t));
  if (ret != 0)
    {
      Debug_printf ("MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL\r\n");
    }
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
  int ret;
  extern mbedtls_ssl_context ssl;
  extern mbedtls_ssl_config conf;
  extern mbedtls_x509_crt cert;
  extern mbedtls_ctr_drbg_context ctr_drbg;
  extern mbedtls_entropy_context entropy;

  memset (ctx, 0, sizeof (httpc_ctx_t));

  ctx->json_statistics = NULL;
  ctx->json_statistics_len = -1;
  ctx->json_clusters = NULL;
  ctx->json_clusters_len = -1;
  ctx->statistics_data = &httpc_station_statistics;
  ctx->clusters_data = httpc_clusters;
  ctx->clusters_sum = bsmuSetting.cu_num;

  ctx->mbedtls.net_ctx = &server_fd;
  ctx->mbedtls.ssl_ctx = &ssl;
  ctx->mbedtls.conf = &conf;
  ctx->mbedtls.cert = &cert;
  ctx->mbedtls.ctr_drbg = &ctr_drbg;
  ctx->mbedtls.entropy = &entropy;

  ctx->cert_verify_passed = 0;

  MX_MBEDTLS_Init ();

  if ((ret = mbedtls_ctr_drbg_seed (ctx->mbedtls.ctr_drbg,
                                    mbedtls_entropy_func, ctx->mbedtls.entropy,
                                    (const unsigned char *)pers, pers_len))
      != 0)
    {
      Debug_printf ("failed! mbedtls_ctr_drbg_seed returned -0x%x\r\n", -ret);
      goto end;
    }

  /*
   * 1. Initialize certificates
   */
  ret = mbedtls_x509_crt_parse (ctx->mbedtls.cert,
                                (const unsigned char *)__ssl_ca_certificate,
                                __ssl_ca_certificate_len);
  if (ret < 0)
    {
      Debug_printf ("failed! mbedtls_x509_crt_parse returned -0x%x\r\n", -ret);
      goto end;
    }

end:
  return ret;
}

static int
cjson_init ()
{
  cJSON_Hooks hooks;

  hooks.free_fn = vPortFree;
  hooks.malloc_fn = pvPortMalloc;

  cJSON_InitHooks (&hooks);

  return 0;
}

static int
httpc_connect (httpc_ctx_t *ctx, const char *host, char *port)
{
  int ret;

  // dns resolve, run once
  if (ctx->host_ip.addr == 0)
    {
      err_t err = ERR_OK;
      err = netconn_gethostbyname (host, &ctx->host_ip);
      if (err != ERR_OK)
        {
          Debug_printf ("Host resolve error %d\r\n", err);
          ret = -1;
          goto end;
        }

      if (ctx->host_ip.addr == 0)
        {
          Debug_printf ("ip addr error\r\n");
          ret = -1;
          goto end;
        }
    }

  /*
   * 2. Start the connection
   */
  if ((ret = mbedtls_net_connect (ctx->mbedtls.net_ctx, inet_ntoa (ctx->host_ip),
                                  port, MBEDTLS_NET_PROTO_TCP))
      != 0)
    {
      Debug_printf ("failed! mbedtls_net_connect returned %d\n\n", ret);
      goto end;
    }

  /*
   * 3. Setup stuff
   */
  if ((ret = mbedtls_ssl_config_defaults (
           ctx->mbedtls.conf, MBEDTLS_SSL_IS_CLIENT,
           MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT))
      != 0)
    {
      Debug_printf ("failed! mbedtls_ssl_config_defaults returned %d\n\n",
                    ret);
      goto end;
    }

  /* OPTIONAL is not optimal for security,
   * but makes interop easier in this simplified application */
  mbedtls_ssl_conf_authmode (ctx->mbedtls.conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
  mbedtls_ssl_conf_ca_chain (ctx->mbedtls.conf, ctx->mbedtls.cert, NULL);
  mbedtls_ssl_conf_rng (ctx->mbedtls.conf, mbedtls_ctr_drbg_random,
                        ctx->mbedtls.ctr_drbg);

  if ((ret = mbedtls_ssl_setup (ctx->mbedtls.ssl_ctx, ctx->mbedtls.conf)) != 0)
    {
      Debug_printf ("failed! mbedtls_ssl_setup returned %d\n\n", ret);
      goto end;
    }

  if ((ret = mbedtls_ssl_set_hostname (ctx->mbedtls.ssl_ctx, HTTPS_HOST)) != 0)
    {
      Debug_printf ("failed! mbedtls_ssl_set_hostname returned %d\n\n", ret);
      goto end;
    }

  mbedtls_ssl_set_bio (ctx->mbedtls.ssl_ctx, ctx->mbedtls.net_ctx,
                       mbedtls_net_send, mbedtls_net_recv, NULL);

  /*
   * 4. Handshake
   */
  while ((ret = mbedtls_ssl_handshake (ctx->mbedtls.ssl_ctx)) != 0)
    {
      if (ret != MBEDTLS_ERR_SSL_WANT_READ
          && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
          Debug_printf ("failed! mbedtls_ssl_handshake returned -0x%x\n\n",
                        -ret);
          goto end;
        }
    }

  // httpc_verify_cert(ctx);

end:
  return ret;
} 

static int
httpc_verify_cert (httpc_ctx_t *ctx)
{
  int ret;

  /*
   * 5. Verify the server certificate
   */
  if ((ret = mbedtls_ssl_get_verify_result (ctx->mbedtls.ssl_ctx)) != 0)
    {
      char vrfy_buf[256];

      Debug_printf ("failed ");
      mbedtls_x509_crt_verify_info ((char *)vrfy_buf, sizeof (vrfy_buf), "!",
                                    ret);

      Debug_printf ("%s\n", vrfy_buf);
    }
  else
    {
      ctx->cert_verify_passed = 1;
    }

  return ret;
}

static int
httpc_disconnect (httpc_ctx_t *ctx)
{
  mbedtls_ssl_close_notify (ctx->mbedtls.ssl_ctx);

  mbedtls_net_free (ctx->mbedtls.net_ctx);

  // mbedtls_x509_crt_free( ctx->mbedtls.cert );
  mbedtls_ssl_free (ctx->mbedtls.ssl_ctx);
  mbedtls_ssl_config_free (ctx->mbedtls.conf);
  // mbedtls_ctr_drbg_free( ctx->mbedtls.ctr_drbg );
  // mbedtls_entropy_free (ctx->mbedtls.entropy);

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

  do
    {
      err_code
          = mbedtls_ssl_read (ctx->mbedtls.ssl_ctx, buffer, sizeof (buffer));

      if (err_code == MBEDTLS_ERR_SSL_WANT_READ
          || err_code == MBEDTLS_ERR_SSL_WANT_WRITE)
        {
          continue;
        }

      if (err_code == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
        {
          break;
        }

      if (err_code < 0)
        {
          Debug_printf ("failed! mbedtls_ssl_read returned %d\n\n",
                        err_code);
          break;
        }

      if (err_code > 0)
        {
          break;
        }
    }
  while (1);

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
https_send (httpc_ctx_t *ctx, const char *pkg, int len)
{
  int ret = 0;

  ret = mbedtls_ssl_write (ctx->mbedtls.ssl_ctx, pkg, len);
  if (ret != len)
    {
      Debug_printf ("ssl write failed -0x%x\r\n", -ret);
    }

  return ret;
}

static int
httpc_send_data_statistics (httpc_ctx_t *ctx)
{
  int ret;
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
  vPortFree (ctx->json_statistics);
  ctx->json_statistics = NULL;
  ctx->json_statistics_len = -1;

  ret = httpc_recv (&http_client);
  if (ret)
    {
      Debug_printf ("statistics data send failed %d\r\n", ret);
      // todo: inform UI
    }

  return ret;
}

static int
httpc_send_data_clusters (httpc_ctx_t *ctx)
{
  int ret;
  char http_header[284];

  memcpy (ctx->clusters_data, &Client_Sd, sizeof (Client_Sd_t) * cluster_num);

  for (int i = 0; i < ctx->clusters_sum; i++)
    {
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
      vPortFree (ctx->json_clusters);
      ctx->json_clusters = NULL;
      ctx->json_clusters_len = -1;

      ret = httpc_recv (&http_client);
      if (ret)
        {
          Debug_printf ("clusters data send failed %d\r\n", ret);
          // todo: inform UI
        }
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

  cjson_init ();
  httpc_ctx_init_default (&http_client);

  // verify cert
  // httpc_connect (&http_client, HOST, HTTPS_PORT);
  // httpc_verify_cert (&http_client);
  // httpc_disconnect (&http_client);

  while (1)
    {
      int ret = 0;
      
      httpc_connect (&http_client, HOST, HTTPS_PORT);

      httpc_send_data_statistics (&http_client);

      httpc_send_data_clusters (&http_client);

      httpc_disconnect (&http_client);

      osDelay (1000);
    }

end:
  // mbedtls_ssl_close_notify( ctx->mbedtls.ssl_ctx );

  // mbedtls_net_free( ctx->mbedtls.net_ctx );

  mbedtls_x509_crt_free (http_client.mbedtls.cert);
  // mbedtls_ssl_free( ctx->mbedtls.ssl_ctx );
  // mbedtls_ssl_config_free( ctx->mbedtls.conf );
  mbedtls_ctr_drbg_free (http_client.mbedtls.ctr_drbg);
  mbedtls_entropy_free (http_client.mbedtls.entropy);
}

void
http_client_init (void)
{
// If SERVER_TOKEN is not defined, clinet will not start
#ifdef HTTPC_ENABLED
  osThreadDef (http_client, httpc_task, osPriorityNormal, 0, 2 * 1024);
  httpc_handle = osThreadCreate (osThread (http_client), NULL);
#endif
}

/**
 * @brief get dns resloved cloud ip
 * @param ip_str ip string buffer
 * @param len buffer len
 * @return 0:ssuccess -1:error
*/
int http_get_cloud_ip(char *ip_str, uint8_t len) {
  char *str_p;
  int str_len;

  str_p = ipaddr_ntoa(&http_client.host_ip); 
  str_len = strlen(str_p);

  if (str_len > len){
      Debug_printf("http_get_cloud_ip: buffer is too short.\r\n");
      return -1;
  }

  memcpy(ip_str, str_p, len);

  return 0;
}

/**
 * @brief get device wan ip offered by DHCP
 * @param ip_str ip string buffer
 * @param len buffer len
 * @return 0:ssuccess -1:error
*/
int http_get_wan_ip(char *ip_str, uint8_t len){
  struct dhcp *dhcp;
  char *str_p;
  int str_len;

  dhcp = netif_dhcp_data (&gnetif);
  str_p = ipaddr_ntoa(&dhcp->offered_ip_addr);
  str_len = strlen(str_p);

  if (str_len > len){
      Debug_printf("http_get_wan_ip: buffer is too short.\r\n");
      return -1;
  }

  memcpy(ip_str, str_p, len);

  return 0;
}
