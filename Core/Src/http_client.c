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
#include "CAN_Control.h"

#define SERVER_TOKEN "8cXNY4p+0HfK4Snvt4QLcQ=="

#if !defined(SERVER_TOKEN)
#warning "HTTPS SERVER TOKEN NOT DEFINED, DEFINE A TOKEN TO ENABLE HTTPS CLIENT"
#define SERVER_TOKEN "NULL"
#else
#define HTTPC_ENABLED
#endif

#define HOST           "hdzbdzbms.online"
#define HTTPS_PORT     "443"
#define HTTP_PORT      "80"
#define HTTP_HOST      HOST
#define HTTPS_HOST     HOST
#define API_STATISTICS "/api/bmsRequest/sendSum"
#define API_CLUSTERS   "/api/bmsRequest/clusterData"

#define HTTP_HEADER \
                "POST %s HTTP/1.1\r\n" \
                "HOST: " HTTPS_HOST "\r\n" \
                "Content-Type: application/json\r\n" \
                "Content-Length: %d\r\n" \
                "Accept: application/json\r\n" \
                "User-Agent: BSMU LWIP/" LWIP_VERSION_STRING "\r\n" \
                "Connection: keep-alive\r\n\r\n"
                

extern struct netif gnetif;
extern Client_Sd_t Client_Sd[cluster_num];
extern Client_Sd_Station_t Client_Sd_Station;
extern error_info_t Client_errors[cluster_num][MAX_ERROR];
extern BCMU_Mail_t BCMU[cluster_num];

extern EEPROM_BSMU bsmuSetting;

extern osSemaphoreId http_snd_sem_handle;
osThreadId httpc_handle = NULL;

Client_Sd_Station_t httpc_station_statistics __attribute__ ((at (0xC040E6A0)));
Client_Sd_t httpc_clusters[cluster_num] __attribute__ ((at (0xC0400000)));

httpc_ctx_t http_client;

static const char *pers   = "ssl_client";
static const int pers_len = sizeof (pers);

static mbedtls_net_context server_fd;

static int
create_statistics_payload (httpc_ctx_t *ctx)
{
    int len                   = -1;
    Client_Sd_Station_t *data = ctx->statistics_data;
    //避免跳转进入尚未完成初始化的变量作用域，方便可靠清理资源。
    cJSON *obj = NULL;
    cJSON *sta = NULL;
    cJSON *vs = NULL;
    cJSON *is = NULL;
    cJSON *soc = NULL;
    cJSON *soh = NULL;
    cJSON *cp = NULL;
    cJSON *dp = NULL;
    cJSON *token = NULL;
    obj = cJSON_CreateObject ();
    if (obj == NULL) goto end;

    sta = cJSON_CreateNumber (data->station_state);
    if (sta == NULL) goto end;
    cJSON_AddItemToObject (obj, "sta", sta);

    vs = cJSON_CreateNumber (data->station_VOL);
    if (vs == NULL) goto end;
    cJSON_AddItemToObject (obj, "vs", vs);

    is = cJSON_CreateNumber (data->station_CUR);
    if (is == NULL) goto end;
    cJSON_AddItemToObject (obj, "is", is);

    soc = cJSON_CreateNumber (data->station_SOC);
    if (soc == NULL) goto end;
    cJSON_AddItemToObject (obj, "soc", soc);

    soh = cJSON_CreateNumber (data->station_SOH);
    if (soh == NULL) goto end;
    cJSON_AddItemToObject (obj, "soh", soh);

    cp = cJSON_CreateNumber (data->charge_power);
    if (cp == NULL) goto end;
    cJSON_AddItemToObject (obj, "cp", cp);

    dp = cJSON_CreateNumber (data->discharge_power);
    if (dp == NULL) goto end;
    cJSON_AddItemToObject (obj, "dp", dp);

    token = cJSON_CreateString (SERVER_TOKEN);
    if (token == NULL) goto end;
    cJSON_AddItemToObject (obj, "token", token);

    ctx->json_statistics = cJSON_Print (obj);
    if (ctx->json_statistics == NULL) {
        Debug_printf ("cJSON print error json_statistics\r\n");
        goto end;
    }

    len                      = strlen (ctx->json_statistics);
    ctx->json_statistics_len = len;

end:
    cJSON_Delete (obj);
    return len;
}

static int
create_clusters_payload (httpc_ctx_t *ctx, int index)
{
    int ret           = -1;
    int len           = -1;
    Client_Sd_t *data = &ctx->clusters_data[index];
    unsigned char base64_buffer[1024];
    int base64_buffer_len = sizeof (base64_buffer);
    cJSON *obj = NULL;
    cJSON *sid = NULL;
    cJSON *sta = NULL;
    cJSON *vs = NULL;
    cJSON *is = NULL;
    cJSON *soc = NULL;
    cJSON *soh = NULL;
    cJSON *rp = NULL;
    cJSON *rn = NULL;
    cJSON *un = NULL;
    cJSON *bn = NULL;
    cJSON *bal_state = NULL;
    cJSON *vb = NULL;
    cJSON *tb = NULL;
    cJSON *socb = NULL;
    cJSON *sohb = NULL;
    cJSON *wb_cnt = NULL;
    cJSON *wb = NULL;
    cJSON *clu_res = NULL;
    cJSON *token = NULL;
    obj = cJSON_CreateObject ();
    if (obj == NULL) goto end;

    sid = cJSON_CreateNumber (data->cluster_No);
    if (sid == NULL) goto end;
    cJSON_AddItemToObject (obj, "sid", sid);

    sta = cJSON_CreateNumber (data->work_state);
    if (sta == NULL) goto end;
    cJSON_AddItemToObject (obj, "sta", sta);

    vs = cJSON_CreateNumber (data->cluster_VOL);
    if (vs == NULL) goto end;
    cJSON_AddItemToObject (obj, "vs", vs);

    is = cJSON_CreateNumber (data->cluster_CUR);
    if (is == NULL) goto end;
    cJSON_AddItemToObject (obj, "is", is);

    soc = cJSON_CreateNumber (data->cluster_SOC);
    if (soc == NULL) goto end;
    cJSON_AddItemToObject (obj, "soc", soc);

    soh = cJSON_CreateNumber (data->cluster_SOH);
    if (soh == NULL) goto end;
    cJSON_AddItemToObject (obj, "soh", soh);

    rp = cJSON_CreateNumber (data->insulation_res_p);
    if (rp == NULL) goto end;
    cJSON_AddItemToObject (obj, "rp", rp);

    rn = cJSON_CreateNumber (data->insulation_res_n);
    if (rn == NULL) goto end;
    cJSON_AddItemToObject (obj, "rn", rn);

    un = cJSON_CreateNumber (data->grp_num);
    if (un == NULL) goto end;
    cJSON_AddItemToObject (obj, "un", un);

    bn = cJSON_CreateNumber (data->grp_bat_num);
    if (bn == NULL) goto end;
    cJSON_AddItemToObject (obj, "bn", bn);

    ret = mbedtls_base64_encode (base64_buffer, base64_buffer_len, NULL, (uint8_t *)data->bal_state, GRP_num + 1);
    if (ret != 0) { Debug_printf ("MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL\r\n"); }

    bal_state = cJSON_CreateString ((const char *)base64_buffer);
    if (bal_state == NULL) goto end;
    cJSON_AddItemToObject (obj, "bal_state", bal_state);

    ret = mbedtls_base64_encode (base64_buffer, base64_buffer_len, NULL, (uint8_t *)data->BAT_VOL,
                                 TOTOL_BAT_num * sizeof (uint16_t));
    if (ret != 0) { Debug_printf ("MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL\r\n"); }
    vb = cJSON_CreateString ((const char *)base64_buffer);
    if (vb == NULL) goto end;
    cJSON_AddItemToObject (obj, "vb", vb);

    ret = mbedtls_base64_encode (base64_buffer, base64_buffer_len, NULL, (uint8_t *)data->BAT_TMP,
                                 TOTOL_BAT_num * sizeof (uint16_t));
    if (ret != 0) { Debug_printf ("MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL\r\n"); }
    tb = cJSON_CreateString ((const char *)base64_buffer);
    if (tb == NULL) goto end;
    cJSON_AddItemToObject (obj, "tb", tb);

    ret = mbedtls_base64_encode (base64_buffer, base64_buffer_len, NULL, (uint8_t *)data->BAT_SOC,
                                 TOTOL_BAT_num * sizeof (uint16_t));
    if (ret != 0) { Debug_printf ("MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL\r\n"); }
    socb = cJSON_CreateString ((const char *)base64_buffer);
    if (socb == NULL) goto end;
    cJSON_AddItemToObject (obj, "socb", socb);

    ret = mbedtls_base64_encode (base64_buffer, base64_buffer_len, NULL, (uint8_t *)data->BAT_SOH,
                                 TOTOL_BAT_num * sizeof (uint16_t));
    if (ret != 0) { Debug_printf ("MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL\r\n"); }
    sohb = cJSON_CreateString ((const char *)base64_buffer);
    if (sohb == NULL) goto end;
    cJSON_AddItemToObject (obj, "sohb", sohb);

    wb_cnt = cJSON_CreateNumber (data->error_count);
    if (wb_cnt == NULL) goto end;
    cJSON_AddItemToObject (obj, "wb_cnt", wb_cnt);

    ret = mbedtls_base64_encode (base64_buffer, base64_buffer_len, NULL, (uint8_t *)Client_errors[data->cluster_No - 1],
                                 data->error_count * sizeof (error_info_t));
    if (ret != 0) { Debug_printf ("MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL\r\n"); }
    wb = cJSON_CreateString ((const char *)base64_buffer);
    if (wb == NULL) goto end;
    cJSON_AddItemToObject (obj, "wb", wb);

    ret = mbedtls_base64_encode (base64_buffer, base64_buffer_len, NULL, (uint8_t *)&(data->cluster_res),
                                 sizeof (float));
    if (ret != 0) { Debug_printf ("MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL\r\n"); }
    clu_res = cJSON_CreateString ((const char *)base64_buffer);
    if (clu_res == NULL) goto end;
    cJSON_AddItemToObject (obj, "clu_res", clu_res);

    token = cJSON_CreateString (SERVER_TOKEN);
    if (token == NULL) goto end;
    cJSON_AddItemToObject (obj, "token", token);

    ctx->json_clusters = cJSON_Print (obj);
    if (ctx->json_clusters == NULL) {
        Debug_printf ("cJSON print error json_clusters\r\n");
        goto end;
    }

    len                    = strlen (ctx->json_clusters);
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

    ctx->json_statistics     = NULL;
    ctx->json_statistics_len = -1;
    ctx->json_clusters       = NULL;
    ctx->json_clusters_len   = -1;
    ctx->statistics_data     = &httpc_station_statistics;
    ctx->clusters_data       = httpc_clusters;
    ctx->clusters_sum        = cluster_num;

    ctx->mbedtls.net_ctx  = &server_fd;
    ctx->mbedtls.ssl_ctx  = &ssl;
    ctx->mbedtls.conf     = &conf;
    ctx->mbedtls.cert     = &cert;
    ctx->mbedtls.ctr_drbg = &ctr_drbg;
    ctx->mbedtls.entropy  = &entropy;

    ctx->cert_verify_passed = 0;

    MX_MBEDTLS_Init ();

    if ((ret = mbedtls_ctr_drbg_seed (ctx->mbedtls.ctr_drbg, mbedtls_entropy_func, ctx->mbedtls.entropy,
                                      (const unsigned char *)pers, pers_len))
        != 0) {
        Debug_printf ("failed! mbedtls_ctr_drbg_seed returned -0x%x\r\n", -ret);
        goto end;
    }

    /*
     * 1. Initialize certificates
     */
    ret = mbedtls_x509_crt_parse (ctx->mbedtls.cert, (const unsigned char *)__ssl_ca_certificate,
                                  __ssl_ca_certificate_len);
    if (ret < 0) {
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

    hooks.free_fn   = vPortFree;
    hooks.malloc_fn = pvPortMalloc;

    cJSON_InitHooks (&hooks);

    return 0;
}

static int
httpc_connect (httpc_ctx_t *ctx, const char *host, char *port)
{
    int ret;

    // todo: There is currently an ugly workaround where we do not have a domain.
    //  ipaddr_aton("47.113.147.27", &ctx->host_ip);

    // dns resolve, run once
    if (ctx->host_ip.addr == 0) {
        err_t err = ERR_OK;
        err       = netconn_gethostbyname (host, &ctx->host_ip);
        if (err != ERR_OK) {
            Debug_printf ("Host resolve error %d\r\n", err);
            ret = -1;
            goto end;
        }

        if (ctx->host_ip.addr == 0) {
            Debug_printf ("ip addr error\r\n");
            ret = -1;
            goto end;
        }
    }

    /*
     * 2. Start the connection
     */
    if ((ret = mbedtls_net_connect (ctx->mbedtls.net_ctx, inet_ntoa (ctx->host_ip), port, MBEDTLS_NET_PROTO_TCP))
        != 0) {
        Debug_printf ("failed! mbedtls_net_connect returned %d\n\n", ret);
        goto end;
    }

    /*
     * 3. Setup stuff
     */
    if ((ret = mbedtls_ssl_config_defaults (ctx->mbedtls.conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
                                            MBEDTLS_SSL_PRESET_DEFAULT))
        != 0) {
        Debug_printf ("failed! mbedtls_ssl_config_defaults returned %d\n\n", ret);
        goto end;
    }

    /* OPTIONAL is not optimal for security,
     * but makes interop easier in this simplified application */
    mbedtls_ssl_conf_authmode (ctx->mbedtls.conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
    mbedtls_ssl_conf_ca_chain (ctx->mbedtls.conf, ctx->mbedtls.cert, NULL);
    mbedtls_ssl_conf_rng (ctx->mbedtls.conf, mbedtls_ctr_drbg_random, ctx->mbedtls.ctr_drbg);

    if ((ret = mbedtls_ssl_setup (ctx->mbedtls.ssl_ctx, ctx->mbedtls.conf)) != 0) {
        Debug_printf ("failed! mbedtls_ssl_setup returned %d\n\n", ret);
        goto end;
    }

    if ((ret = mbedtls_ssl_set_hostname (ctx->mbedtls.ssl_ctx, HTTPS_HOST)) != 0) {
        Debug_printf ("failed! mbedtls_ssl_set_hostname returned %d\n\n", ret);
        goto end;
    }

    mbedtls_ssl_set_bio (ctx->mbedtls.ssl_ctx, ctx->mbedtls.net_ctx, mbedtls_net_send, mbedtls_net_recv, NULL);

    /*
     * 4. Handshake
     */
    while ((ret = mbedtls_ssl_handshake (ctx->mbedtls.ssl_ctx)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            Debug_printf ("failed! mbedtls_ssl_handshake returned -0x%x\n\n", -ret);
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
    if ((ret = mbedtls_ssl_get_verify_result (ctx->mbedtls.ssl_ctx)) != 0) {
        char vrfy_buf[256];

        Debug_printf ("failed ");
        mbedtls_x509_crt_verify_info ((char *)vrfy_buf, sizeof (vrfy_buf), "!", ret);

        Debug_printf ("%s\n", vrfy_buf);
    } else {
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

#define REC_BUFFER_LEN 1024  // 增大缓冲区大小

static int
httpc_recv (httpc_ctx_t *ctx)
{
    char buffer[REC_BUFFER_LEN];  // 接收缓冲区，长度为REC_BUFFER_LEN
    char *http_start = NULL;      // 指向HTTP响应起始位置的指针
    int ssl_ret = 0;              // SSL读取操作的返回值
    int total_received = 0;       // 总共接收到的字节数
    
    // 1. 初始化接收缓冲区，全部清零
    memset(buffer, 0, sizeof(buffer));
    
    // 2. 循环读取SSL数据
    do {
        // 从SSL连接读取数据，使用偏移量避免覆盖已接收的数据
        ssl_ret = mbedtls_ssl_read(ctx->mbedtls.ssl_ctx, 
                                  (unsigned char *)buffer + total_received, 
                                  sizeof(buffer) - total_received - 1U);
        
        // 如果SSL需要更多数据或需要发送数据，继续循环
        if (ssl_ret == MBEDTLS_ERR_SSL_WANT_READ || 
            ssl_ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
            continue;  // 继续尝试读取
        }
        
        // 如果对端关闭了连接，跳出循环
        if (ssl_ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
            break;  // 对端发送了关闭通知
        }
        
        // 如果SSL读取出错，打印错误信息并返回错误码
        if (ssl_ret < 0) {
            Debug_printf("SSL read error: %d\n", ssl_ret);
            return ssl_ret;  // 返回SSL错误码
        }
        
        // 如果成功读取到数据
        if (ssl_ret > 0) {
            total_received += ssl_ret;     // 更新总接收字节数
            buffer[total_received] = '\0';  // 在接收数据末尾添加null终止符，确保字符串安全
            
            // 检查是否已经收到了完整的HTTP头部（HTTP头部以\r\n\r\n结束）
            if (strstr(buffer, "\r\n\r\n") != NULL) {
                break;  // 收到完整HTTP头部，可以停止读取
            }
            
            // 防止缓冲区溢出，如果缓冲区快满了，打印警告并停止读取
            if (total_received >= sizeof(buffer) - 1) {
                Debug_printf("Warning: Buffer full, may have truncated response\n");
                break;  // 缓冲区已满，可能截断了响应
            }
        }
    } while (ssl_ret > 0);  // 只要还有数据就继续读取
    
    // 打印调试信息，显示总共接收了多少字节
    Debug_printf("Received %d bytes\n", total_received);
    
    // 3. 在接收到的数据中查找HTTP响应起始位置
    http_start = strstr(buffer, "HTTP/");
    if (http_start == NULL) {
        Debug_printf("Error: No HTTP header found\n");
        return -1;  // 没有找到HTTP头部，返回错误
    }
    
    // 4. 找到HTTP版本后面的第一个空格（HTTP/1.1和状态码之间的空格）
    char *space_pos = strchr(http_start, ' ');
    if (space_pos == NULL) {
        Debug_printf("Error: No space after HTTP version\n");
        return -2;  // 没有找到空格，HTTP格式错误
    }
    
    // 5. 将指针移动到空格后面，即状态码开始的位置
    char *status_start = space_pos + 1;
    
    // 6. 跳过状态码前面可能存在的空格或制表符
    while (*status_start == ' ' || *status_start == '\t') {
        status_start++;  // 移动到状态码的第一个数字
    }
    
    // 7. 验证状态码是否是三位数字
    // 使用isdigit()函数检查三个字符是否都是数字
    if (!isdigit(status_start[0]) || 
        !isdigit(status_start[1]) || 
        !isdigit(status_start[2])) {
        Debug_printf("Error: Status code not 3 digits at: %.10s\n", status_start);
        return -3;  // 状态码格式错误，不是三位数字
    }
    
    // 8. 将三个数字字符转换为整数状态码
    // 例如，字符'2','0','0'转换为整数200
    int http_status = (status_start[0] - '0') * 100 +  // 百位数字
                      (status_start[1] - '0') * 10 +   // 十位数字
                      (status_start[2] - '0');         // 个位数字
    
    // 打印解析到的HTTP状态码
    Debug_printf("HTTP status code: %d\n", http_status);
    
    // 返回HTTP状态码，如200表示成功
    return http_status;
}

static int
https_send (httpc_ctx_t *ctx, const char *pkg, int len)
{
    int ret = 0;

    ret = mbedtls_ssl_write (ctx->mbedtls.ssl_ctx, (const unsigned char *)pkg, (size_t)len);
    if (ret != len) { Debug_printf ("ssl write failed -0x%x\r\n", -ret); }

    return ret;
}

static int
httpc_send_data_statistics (httpc_ctx_t *ctx)
{
    int ret;
    char http_header[284];

    memcpy (ctx->statistics_data, &Client_Sd_Station, sizeof (Client_Sd_Station_t));

    create_statistics_payload (ctx);

    int http_header_len
        = snprintf (http_header, sizeof (http_header), 
                    HTTP_HEADER, 
                     API_STATISTICS,
                    ctx->json_statistics_len);
    https_send (ctx, http_header, http_header_len);
    https_send (ctx, ctx->json_statistics, ctx->json_statistics_len);

    // free josn string
    vPortFree (ctx->json_statistics);
    ctx->json_statistics     = NULL;
    ctx->json_statistics_len = -1;

    ret = httpc_recv (&http_client);
    if (ret!=200) {
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

    Debug_printf("Start sending clusters data...\n");
    
    memcpy (ctx->clusters_data, &Client_Sd, sizeof (Client_Sd_t) * cluster_num);

    for (int i = 0; i < ctx->clusters_sum; i++) {
        Debug_printf("Processing clusters %d\n", i);
        
        // skip offline
        if (BCMU[i].OnlineOrOffline == Offline) {
            Debug_printf("cluster %d offline，skip\n", i);
            continue;
        }

        // skip invalid data
        if (ctx->clusters_data[i].cluster_No > 20 || ctx->clusters_data[i].cluster_No < 1) { 
            Debug_printf("cluster %d Invalid number: %d，skip\n", i, ctx->clusters_data[i].cluster_No);
            continue; 
        }

        Debug_printf("For cluster %d to create payload...\n", i);
        ret = create_clusters_payload (ctx, i);

        if (ret == -1) {
            Debug_printf ("create clusters payload failed\r\n");
            break;
        }
        
        Debug_printf("Create payload success, JSON lenth: %d\n", ctx->json_clusters_len);

        int http_header_len
            = snprintf (http_header, sizeof (http_header),
                        HTTP_HEADER,
                        API_CLUSTERS, 
                        ctx->json_clusters_len);
        
        Debug_printf("HTTP header_len: %d\n", http_header_len);

        Debug_printf("Send HTTP header...\n");
        ret = https_send (ctx, http_header, http_header_len);
        Debug_printf("https_send(header) return: %d\n", ret);
        
        if (ret < 0) {
            Debug_printf("Send HTTP header failed!\n");
            vPortFree (ctx->json_clusters);
            ctx->json_clusters = NULL;
            ctx->json_clusters_len = -1;
            continue;  // 继续下一个集群
        }

        Debug_printf("Send JSON data...\n");
        ret = https_send (ctx, ctx->json_clusters, ctx->json_clusters_len);
        Debug_printf("https_send(JSON) return: %d\n", ret);
        
        if (ret < 0) {
            Debug_printf("Send JSON data failed!\n");
            vPortFree (ctx->json_clusters);
            ctx->json_clusters = NULL;
            ctx->json_clusters_len = -1;
            continue;  // 继续下一个集群
        }

        // free json string
        vPortFree (ctx->json_clusters);
        ctx->json_clusters     = NULL;
        ctx->json_clusters_len = -1;

        Debug_printf("Waiting for server response...\n");
        ret = httpc_recv (&http_client);
        Debug_printf("httpc_recv return: %d\n", ret);
        
        if (ret != 200) {
            Debug_printf ("clusters data send failed %d\r\n", ret);
            
            // 注意：这里打印的是指针地址，不是内容！
            Debug_printf ("json_clusters is %d\r\n", ctx->clusters_data);
            
            // todo: inform UI
        } else {
            Debug_printf("cluster %d Sent successfully!\n", i);
        }
    }

    return ret;
}

static void
httpc_task (void const *args)
{
    while (dhcp_supplied_address (&gnetif) == 0) {
        Debug_printf ("https client: waiting for DHCP\r\n");
        osDelay (2000);
    }

    cjson_init ();
    httpc_ctx_init_default (&http_client);

    // verify cert
    // httpc_connect (&http_client, HOST, HTTPS_PORT);
    // httpc_verify_cert (&http_client);
    // httpc_disconnect (&http_client);
    // 定义两个临时的缓存变量
//Client_Sd_t temp_clusters[cluster_num];
//Client_Sd_Station_t temp_station;
    while (1) {
        if (xSemaphoreTake (http_snd_sem_handle, portMAX_DELAY) == pdTRUE) // 等待获取信号量(等待FDCAN轮询完大概1s)
        {
//            memcpy(temp_clusters, Client_Sd, sizeof(Client_Sd)); 
//            memcpy(&temp_station, &Client_Sd_Station, sizeof(Client_Sd_Station));
            httpc_connect (&http_client, HOST, HTTPS_PORT);
            if(httpc_send_data_clusters (&http_client) == 200)
            {
                httpc_send_data_statistics (&http_client);
            }
            httpc_disconnect (&http_client);
        }
        osDelay (1);
    }

//end:
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
int
http_get_cloud_ip (char *ip_str, uint8_t len)
{
    char *str_p;
    int str_len;

    str_p   = ipaddr_ntoa (&http_client.host_ip);
    str_len = strlen (str_p);

    if (str_len > len) {
        Debug_printf ("http_get_cloud_ip: buffer is too short.\r\n");
        return -1;
    }

    memcpy (ip_str, str_p, len);

    return 0;
}

/**
 * @brief get device wan ip offered by DHCP
 * @param ip_str ip string buffer
 * @param len buffer len
 * @return 0:ssuccess -1:error
 */
int
http_get_wan_ip (char *ip_str, uint8_t len)
{
    struct dhcp *dhcp;
    char *str_p;
    int str_len;

    dhcp    = netif_dhcp_data (&gnetif);
    str_p   = ipaddr_ntoa (&dhcp->offered_ip_addr);
    str_len = strlen (str_p);

    if (str_len > len) {
        Debug_printf ("http_get_wan_ip: buffer is too short.\r\n");
        return -1;
    }

    memcpy (ip_str, str_p, len);

    return 0;
}
