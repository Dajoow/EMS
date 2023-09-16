#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#include "station_ctl.h"
#include "lwip/ip_addr.h"
#include "mbedtls.h"
#include "mbedtls/net_sockets.h"

typedef struct
{
  struct{
    mbedtls_ssl_context *ssl_ctx;
    mbedtls_net_context *net_ctx;
    mbedtls_ssl_config *conf;
    mbedtls_x509_crt *cert;
    mbedtls_ctr_drbg_context *ctr_drbg;
    mbedtls_entropy_context *entropy;
  } mbedtls;
  int cert_verify_passed;
  ip_addr_t host_ip;

  char *json_statistics;
  int json_statistics_len;
  char *json_clusters;
  int json_clusters_len;
  int clusters_sum;
  Client_Sd_Station_t *statistics_data;
  Client_Sd_t *clusters_data;
}httpc_ctx_t;


void http_client_init(void);

int http_get_cloud_ip(char *ip_str, uint8_t len);

int http_get_wan_ip(char *ip_str, uint8_t len);

#endif
