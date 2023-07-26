#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#include "station_ctl.h"
#include "wolfssl/ssl.h"

typedef struct
{
  int socket;
  WOLFSSL_CTX *ssl_ctx;
  WOLFSSL *ssl;
  char *json_statistics;
  int json_statistics_len;
  char *json_clusters;
  int json_clusters_len;
  int clusters_sum;
  Client_Sd_Station_t *statistics_data;
  Client_Sd_t *clusters_data;
}httpc_ctx_t;


void http_client_init(void);

#endif
