/**
 * @file tuya_os_adapt_bt.h
 * @brief BLE 操作接口
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */
#include "tuya_os_adapter.h"
#include "tuya_os_adapter_error_code.h"
#include "tuya_os_adapt_bt.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uni_md5.h"
#include "aes_inf.h"

#if defined(TY_BT_MOD) && TY_BT_MOD == 1
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "mem_pool.h"

#include "bluetooth.h"
#include "hci.h"
#include "hci_lib.h"
#include "l2cap.h"
#include "uuid.h"
#include "mainloop.h"
#include "util.h"
#include "att.h"
#include "queue.h"
#include "timeout.h"
#include "gatt-db.h"
#include "gatt-server.h"

#define UUID_GAP                0x1800
#define UUID_GATT               0x1801
#define UUID_TUYA               0x1910
#define UUID_TUYA_WRITE         0x2B11
#define UUID_TUYA_NOTIFY        0x2B10

#define DEV_ID                  0
#define ATT_CID                 4
#define MAX_MTU                 512
/* 0x0320 * 0.625ms = 500ms*/
#define ADV_INTERVAL            0x0320

#ifndef BLE_LOG_EN
#define BLE_LOG_EN              TRUE
#endif

#define BLE_LOG_EN              TRUE
#if BLE_LOG_EN
#define BLE_TAG                 "BLE_INFO"
#define BLE_LOG(format,...)     printf("[%s]: "format"", BLE_TAG, ##__VA_ARGS__)
#else
#define BLE_LOG(format,...)     do {} while (0)
#endif

struct gatt_server_t
{
    struct bt_att *att;
    struct gatt_db *db;
    struct bt_gatt_server *gatt;
    struct gatt_db_attribute *tuya_service;
    struct gatt_db_attribute *tuya_write;
    struct gatt_db_attribute *tuya_notify;
    uint16_t tuya_handle;
    uint16_t tuya_write_handle;
    uint16_t tuya_notify_handle;
    uint16_t gatt_svc_chngd_handle;
    bool gatt_svc_chngd_enabled;
    bool gatt_tuya_svc_chngd_enabled;
    int hci_sock;
    bdaddr_t src_addr;
    TY_BT_MSG_CB bt_msg_cb;
};

struct gatt_server_t gatt_server;

/* add begin: by sunkz, interface regist */
TUYA_OS_BT_INTF m_tuya_os_bt_intfs = {
    .port_init      = tuya_os_adapt_bt_port_init,
    .port_deinit    = tuya_os_adapt_bt_port_deinit,
    .gap_disconnect = tuya_os_adapt_bt_gap_disconnect,
    .send           = tuya_os_adapt_bt_send,
    .reset_adv      = tuya_os_adapt_bt_reset_adv,
    .get_rssi       = tuya_os_adapt_bt_get_rssi,
    .start_adv      = tuya_os_adapt_bt_start_adv,
    .stop_adv       = tuya_os_adapt_bt_stop_adv,
    .assign_scan    = tuya_os_adapt_bt_assign_scan,
    .scan_init      = tuya_os_adapt_bt_scan_init,
    .start_scan     = tuya_os_adapt_bt_start_scan,
    .stop_scan      = tuya_os_adapt_bt_stop_scan,
};
/* add end */

/* 设置广播参数 */
int hci_le_set_advertise_parameters(int dd, le_set_advertising_parameters_cp adv_params_cp)
{
    struct hci_request rq;
    uint8_t status;

    BLE_LOG("Function hci_le_set_advertise_parameters called!\r\n");

    memset(&rq, 0, sizeof(rq));
    rq.ogf = OGF_LE_CTL;
    rq.ocf = OCF_LE_SET_ADVERTISING_PARAMETERS;
    rq.cparam = &adv_params_cp;
    rq.clen = LE_SET_ADVERTISING_PARAMETERS_CP_SIZE;
    rq.rparam = &status;
    rq.rlen = 1;

    return (hci_send_req(dd, &rq, 1000));
}

/* 设置广播数据 */
int hci_le_set_advertise_data(int dd, uint8_t *adv_data, uint8_t len)
{
    le_set_advertising_data_cp advertising_data_cp;
    struct hci_request rq;
    uint8_t status;

    BLE_LOG("Function hci_le_set_advertise_data called!\r\n");

    BLE_LOG("adv data len: [%d](", len);
    for (int i = 0; i < len; i++)
    {
        printf("%02X ", adv_data[i]);
    }
    printf("\b)\r\n");

    memset(&advertising_data_cp, 0, sizeof(advertising_data_cp));
    advertising_data_cp.length = len;
    memcpy(advertising_data_cp.data, adv_data, len);
    memset(&rq, 0, sizeof(rq));
    rq.ogf = OGF_LE_CTL;
    rq.ocf = OCF_LE_SET_ADVERTISING_DATA;
    rq.cparam = &advertising_data_cp;
    rq.clen = LE_SET_ADVERTISING_DATA_CP_SIZE;
    rq.rparam = &status;
    rq.rlen = 1;

    return (hci_send_req(dd, &rq, 1000));
}

/* 设置扫描回复数据 */
int hci_le_set_scan_rsp_data(int dd, uint8_t *rsp_data, uint8_t len)
{
    le_set_scan_response_data_cp scan_response_data_cp;
    struct hci_request rq;
    uint8_t status;

    BLE_LOG("Function hci_le_set_scan_rsp_data called!\r\n");

    BLE_LOG("rsp data len: [%d](", len);
    for (int i = 0; i < len; i++)
    {
        printf("%02X ", rsp_data[i]);
    }
    printf("\b)\r\n");

    memset(&scan_response_data_cp, 0, sizeof(scan_response_data_cp));
    scan_response_data_cp.length = len;
    memcpy(scan_response_data_cp.data, rsp_data, len);
    memset(&rq, 0, sizeof(rq));
    rq.ogf = OGF_LE_CTL;
    rq.ocf = OCF_LE_SET_SCAN_RESPONSE_DATA;
    rq.cparam = &scan_response_data_cp;
    rq.clen = LE_SET_SCAN_RESPONSE_DATA_CP_SIZE;
    rq.rparam = &status;
    rq.rlen = 1;

    return (hci_send_req(dd, &rq, 1000));
}

void gap_device_name_read_cb(struct gatt_db_attribute *attrib, unsigned int id, uint16_t offset,
                             uint8_t opcode, struct bt_att *att, void *user_data)
{
    struct gatt_server_t *gatt_server = user_data;
    uint8_t error = 0;
    size_t len = 0;
    int ret = -1;
    uint8_t device_name[HCI_MAX_NAME_LENGTH];
    uint8_t *value = NULL;

    BLE_LOG("Function gap_device_name_read_cb called!\r\n");

    if (offset > len)
    {
        error = BT_ATT_ERROR_INVALID_OFFSET;
        goto done;
    }

    len = sizeof(device_name);

    ret = hci_read_local_name(gatt_server->hci_sock, len, device_name, 1000);
    if (ret < 0)
    {
        perror("Fail to hci_read_local_name!\r\n");
        goto done;
    }

    len -= offset;
    value = len ? &device_name[offset] : NULL;

    BLE_LOG("device_name: %s\r\n", device_name);

done:
    gatt_db_attribute_read_result(attrib, id, error, value, len);
}

void confirm_write(struct gatt_db_attribute *attr, int err, void *user_data)
{
    BLE_LOG("Function confirm_write called!\r\n");

    if (!err)
    {
        return;
    }

    fprintf(stderr, "Error caching attribute %p - err: %d!\r\n", attr, err);
    exit(EXIT_FAILURE);
}

void create_gap_service(struct gatt_server_t *gatt_server)
{
    bt_uuid_t uuid;
    struct gatt_db_attribute *gap_service;
    struct gatt_db_attribute *gap_appearance;
    uint16_t appearance;

    BLE_LOG("Function create_gap_service called!\r\n");

    bt_uuid16_create(&uuid, UUID_GAP);
    gap_service = gatt_db_add_service(gatt_server->db, &uuid, true, 6);

    bt_uuid16_create(&uuid, GATT_CHARAC_DEVICE_NAME);
    gatt_db_service_add_characteristic(gap_service, &uuid, BT_ATT_PERM_READ, BT_GATT_CHRC_PROP_READ,
                                       gap_device_name_read_cb, NULL, gatt_server);

    bt_uuid16_create(&uuid, GATT_CHARAC_APPEARANCE);
    gap_appearance = gatt_db_service_add_characteristic(gap_service, &uuid, BT_ATT_PERM_READ, BT_GATT_CHRC_PROP_READ,
                                                        NULL, NULL, gatt_server);

    put_le16(128, &appearance);
    gatt_db_attribute_write(gap_appearance, 0, (void *)&appearance, sizeof(appearance),
                            BT_ATT_OP_WRITE_REQ, NULL, confirm_write, NULL);

    gatt_db_service_set_active(gap_service, true);
}

void gatt_service_changed_cb(struct gatt_db_attribute *attrib, unsigned int id, uint16_t offset,
                             uint8_t opcode, struct bt_att *att, void *user_data)
{
    BLE_LOG("Function gatt_service_changed_cb called!\r\n");

    gatt_db_attribute_read_result(attrib, id, 0, NULL, 0);
}

void gatt_svc_chngd_ccc_read_cb(struct gatt_db_attribute *attrib, unsigned int id, uint16_t offset,
                                uint8_t opcode, struct bt_att *att, void *user_data)
{
    struct gatt_server_t *gatt_server = user_data;
    uint8_t value[2];

    BLE_LOG("Function gatt_svc_chngd_ccc_read_cb called!\r\n");

    value[0] = gatt_server->gatt_svc_chngd_enabled ? 0x02 : 0x00;
    value[1] = 0x00;

    gatt_db_attribute_read_result(attrib, id, 0, value, sizeof(value));
}

void gatt_svc_chngd_ccc_write_cb(struct gatt_db_attribute *attrib, unsigned int id, uint16_t offset, const uint8_t *value,
                                 size_t len, uint8_t opcode, struct bt_att *att, void *user_data)
{
    struct gatt_server_t *gatt_server = user_data;
    uint8_t ecode = 0;

    BLE_LOG("Function gatt_svc_chngd_ccc_write_cb called!\r\n");

    if (!value || len != 2)
    {
        ecode = BT_ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LEN;
        goto done;
    }

    if (offset)
    {
        ecode = BT_ATT_ERROR_INVALID_OFFSET;
        goto done;
    }

    BLE_LOG("recv value len: [%d](", len);
    for (int i = 0; i < len; i++)
    {
        printf("%02X ", value[i]);
    }
    printf("\b)\r\n");

    if (value[0] == 0x00)
    {
        gatt_server->gatt_svc_chngd_enabled = false;
    }
    else if (value[0] == 0x02)
    {
        gatt_server->gatt_svc_chngd_enabled = true;
    }
    else
    {
        ecode = 0x80;
    }

    BLE_LOG("Service Changed Enabled: %s\n", gatt_server->gatt_svc_chngd_enabled ? "true" : "false");

done:
    gatt_db_attribute_write_result(attrib, id, ecode);
}

void create_gatt_service(struct gatt_server_t *gatt_server)
{
    bt_uuid_t uuid;
    struct gatt_db_attribute *gatt_service;
    struct gatt_db_attribute *gatt_svc_chngd;

    BLE_LOG("Function create_gatt_service called!\r\n");

    bt_uuid16_create(&uuid, UUID_GATT);
    gatt_service = gatt_db_add_service(gatt_server->db, &uuid, true, 4);

    bt_uuid16_create(&uuid, GATT_CHARAC_SERVICE_CHANGED);
    gatt_svc_chngd = gatt_db_service_add_characteristic(gatt_service, &uuid, BT_ATT_PERM_READ,
                                                        BT_GATT_CHRC_PROP_READ | BT_GATT_CHRC_PROP_INDICATE,
                                                        gatt_service_changed_cb, NULL, gatt_server);
    gatt_server->gatt_svc_chngd_handle = gatt_db_attribute_get_handle(gatt_svc_chngd);

    bt_uuid16_create(&uuid, GATT_CLIENT_CHARAC_CFG_UUID);
    gatt_db_service_add_descriptor(gatt_service, &uuid, BT_ATT_PERM_READ | BT_ATT_PERM_WRITE,
                                   gatt_svc_chngd_ccc_read_cb, gatt_svc_chngd_ccc_write_cb,  gatt_server);

    gatt_db_service_set_active(gatt_service, true);
}

/* 涂鸦服务写回调函数 */
void tuya_write_cb(struct gatt_db_attribute *attrib, unsigned int id, uint16_t offset, const uint8_t *value,
                   size_t len, uint8_t opcode, struct bt_att *att, void *user_data)
{
    struct gatt_server_t *server = user_data;
    tuya_ble_data_buf_t data = {0};
    uint8_t ecode = 0;

    BLE_LOG("Function tuya_write_cb called!\r\n");

    if (!value)
    {
        ecode = BT_ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LEN;
        goto done;
    }

    if (offset)
    {
        ecode = BT_ATT_ERROR_INVALID_OFFSET;
        goto done;
    }

    BLE_LOG("recv data len: [%d](", len);
    for (int i = 0; i < len; i++)
    {
        printf("%02X ", value[i]);
    }
    printf("\b)\r\n");

    if (gatt_server.bt_msg_cb == NULL)
    {
        return;
    }

    data.len = len;
    data.data = (char *)value;
    gatt_server.bt_msg_cb(0, TY_BT_EVENT_RX_DATA, &data);

done:
    gatt_db_attribute_write_result(attrib, id, ecode);
}

void gatt_tuya_svc_chngd_ccc_read_cb(struct gatt_db_attribute *attrib, unsigned int id, uint16_t offset,
                                uint8_t opcode, struct bt_att *att, void *user_data)
{
    struct gatt_server_t *gatt_server = user_data;
    uint8_t value[2];

    BLE_LOG("Function gatt_tuya_svc_chngd_ccc_read_cb called!\r\n");

    value[0] = gatt_server->gatt_tuya_svc_chngd_enabled ? 0x01 : 0x00;
    value[1] = 0x00;

    gatt_db_attribute_read_result(attrib, id, 0, value, sizeof(value));
}

void gatt_tuya_svc_chngd_ccc_write_cb(struct gatt_db_attribute *attrib, unsigned int id, uint16_t offset, const uint8_t *value,
                                 size_t len, uint8_t opcode, struct bt_att *att, void *user_data)
{
    struct gatt_server_t *gatt_server = user_data;
    uint8_t ecode = 0;

    BLE_LOG("Function gatt_tuya_svc_chngd_ccc_write_cb called!\r\n");

    if (!value || len != 2)
    {
        ecode = BT_ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LEN;
        goto done;
    }

    if (offset)
    {
        ecode = BT_ATT_ERROR_INVALID_OFFSET;
        goto done;
    }

    BLE_LOG("recv value len: [%d](", len);
    for (int i = 0; i < len; i++)
    {
        printf("%02X ", value[i]);
    }
    printf("\b)\r\n");

    if (value[0] == 0x00)
    {
        gatt_server->gatt_tuya_svc_chngd_enabled = false;
    }
    else if (value[0] == 0x01)
    {
        gatt_server->gatt_tuya_svc_chngd_enabled = true;
    }
    else
    {
        ecode = 0x80;
    }

    BLE_LOG("Service Changed Enabled: %s\n", gatt_server->gatt_tuya_svc_chngd_enabled ? "true" : "false");

done:
    gatt_db_attribute_write_result(attrib, id, ecode);
}

void create_tuya_service(struct gatt_server_t *gatt_server)
{
    bt_uuid_t uuid;

    BLE_LOG("Function create_tuya_service called!\r\n");

    bt_uuid16_create(&uuid, UUID_TUYA);
    gatt_server->tuya_service = gatt_db_add_service(gatt_server->db, &uuid, true, 8);
    gatt_server->tuya_handle = gatt_db_attribute_get_handle(gatt_server->tuya_service);

    bt_uuid16_create(&uuid, UUID_TUYA_WRITE);
    gatt_server->tuya_write = gatt_db_service_add_characteristic(gatt_server->tuya_service, &uuid,
                              BT_ATT_PERM_WRITE, BT_GATT_CHRC_PROP_WRITE_WITHOUT_RESP, NULL,
                              tuya_write_cb, gatt_server);
    gatt_server->tuya_write_handle = gatt_db_attribute_get_handle(gatt_server->tuya_write);

    bt_uuid16_create(&uuid, UUID_TUYA_NOTIFY);
    gatt_server->tuya_notify = gatt_db_service_add_characteristic(gatt_server->tuya_service, &uuid,
                               BT_ATT_PERM_NONE, BT_GATT_CHRC_PROP_NOTIFY, NULL, NULL, NULL);
    gatt_server->tuya_notify_handle = gatt_db_attribute_get_handle(gatt_server->tuya_notify);

    bt_uuid16_create(&uuid, GATT_CLIENT_CHARAC_CFG_UUID);
    gatt_db_service_add_descriptor(gatt_server->tuya_notify, &uuid, BT_ATT_PERM_READ | BT_ATT_PERM_WRITE,
                                   gatt_tuya_svc_chngd_ccc_read_cb, gatt_tuya_svc_chngd_ccc_write_cb,  gatt_server);

    gatt_db_service_set_active(gatt_server->tuya_service, true);
}

void create_server(struct gatt_server_t *gatt_server)
{
    BLE_LOG("Function create_server called!\r\n");

    create_gap_service(gatt_server);
    create_gatt_service(gatt_server);
    create_tuya_service(gatt_server);
}

int tuya_os_adapt_bt_port_init(ty_bt_param_t *p)
{
    /**
     * 函数中需要实现以下功能：
     *   a) 初始化蓝牙协议栈，保证蓝牙能正常通讯。
     *   b) 保存函数指针变量，把接收到的蓝牙数据通过该函数指针传递给 SDK。
     */
    int ret = -1;

    BLE_LOG("Function tuya_os_adapt_bt_port_init called!\r\n");

    gatt_server.bt_msg_cb = p->cb;

    mainloop_init();

    gatt_server.hci_sock = hci_open_dev(DEV_ID);
    if (gatt_server.hci_sock < 0)
    {
        perror("Fail to hci_open_dev!\r\n");
        return EXIT_FAILURE;
    }

    ret = hci_devba(DEV_ID, &gatt_server.src_addr);
    if (ret < 0)
    {
        perror("Fail to hci_devba!\r\n");
        return EXIT_FAILURE;
    }

    gatt_server.db = gatt_db_new();
    if (!gatt_server.db)
    {
        perror("Fail to gatt_db_new!\r\n");
        return EXIT_FAILURE;
    }

    return OPRT_OK;
}

/**
 * @brief tuya_os_adapt_bt 蓝牙断开关闭
 * @return OPERATE_RET 
 */
int tuya_os_adapt_bt_port_deinit(void)
{
    BLE_LOG("Function tuya_os_adapt_bt_port_deinit called!\r\n");

    gatt_db_service_set_active(gatt_server.tuya_service, false);
    gatt_db_unref(gatt_server.db);
    close(gatt_server.hci_sock);

    return OPRT_OK;
}

/**
 * @brief tuya_os_adapt_bt 蓝牙断开
 * @return OPERATE_RET 
 */
int tuya_os_adapt_bt_gap_disconnect(void)
{
    tuya_ble_data_buf_t data = {0};

    BLE_LOG("Function tuya_os_adapt_bt_gap_disconnect called!\r\n");

    mainloop_quit();

    if (gatt_server.bt_msg_cb == NULL)
    {
        return EXIT_FAILURE;
    }

    data.len = 0;
    data.data = NULL;
    gatt_server.bt_msg_cb(0, TY_BT_EVENT_DISCONNECTED, &data);
    gatt_server.bt_msg_cb(0, TY_BT_EVENT_ADV_READY, &data);

    return OPRT_OK;
}

/**
 * @brief tuya_os_adapt_bt 蓝牙发送
 * @return OPERATE_RET 
 */
int tuya_os_adapt_bt_send(unsigned char *data, unsigned char len)
{
    /**
     * 需要实现的逻辑：把数据通过 GATT Server Send Characteristic Notification 发给 App。
     */
    BLE_LOG("Function tuya_os_adapt_bt_send called!\r\n");

    BLE_LOG("send data len: [%d](", len);
    for (int i = 0; i < len; i++)
    {
        printf("%02X ", data[i]);
    }
    printf("\b)\r\n");

    bt_gatt_server_send_notification(gatt_server.gatt, gatt_server.tuya_notify_handle, data, len);

    return OPRT_OK;
}

/* 蓝牙连接管理线程 */
void * l2cap_server_pthread(void *pReserved)
{
    int l2cap_sock = -1;
    int l2cap_fd = -1;
    int ret = -1;
    struct sockaddr_l2 l2cap_serveraddr;
    struct sockaddr_l2 l2cap_clientaddr;
    socklen_t l2cap_clientaddr_len;
    struct bt_security bt_sec;
    char l2cap_client_bdaddr[18];
    tuya_ble_data_buf_t data = {0};

    BLE_LOG("Function l2cap_server_pthread called!\r\n");

    l2cap_sock = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    if (l2cap_sock < 0)
    {
        perror("Failed to socket!\r\n");
        return NULL;
    }

    memset(&l2cap_serveraddr, 0, sizeof(l2cap_serveraddr));
    l2cap_serveraddr.l2_family = AF_BLUETOOTH;
    l2cap_serveraddr.l2_cid = htobs(ATT_CID);
    l2cap_serveraddr.l2_bdaddr_type = BDADDR_LE_PUBLIC;
    bacpy(&l2cap_serveraddr.l2_bdaddr, &gatt_server.src_addr);

    ret = bind(l2cap_sock, (struct sockaddr *)&l2cap_serveraddr, sizeof(l2cap_serveraddr));
    if (ret < 0)
    {
        perror("Failed to bind!\r\n");
        return NULL;
    }

    memset(&bt_sec, 0, sizeof(bt_sec));
    bt_sec.level = BT_SECURITY_LOW;
    ret = setsockopt(l2cap_sock, SOL_BLUETOOTH, BT_SECURITY, &bt_sec, sizeof(bt_sec));
    if (ret != 0)
    {
        perror("Failed to setsockopt!\r\n");
        return NULL;
    }

    ret = listen(l2cap_sock, 10);
    if (ret < 0)
    {
        perror("Fail to listen!\r\n");
        return NULL;
    }

    BLE_LOG("Started listening on ATT channel. Waiting for connections...\r\n");

    memset(&l2cap_clientaddr, 0, sizeof(l2cap_clientaddr));
    l2cap_clientaddr_len = sizeof(l2cap_clientaddr);
    l2cap_fd = accept(l2cap_sock, (struct sockaddr *)&l2cap_clientaddr, &l2cap_clientaddr_len);
    BLE_LOG("l2cap_fd : %d...\r\n", l2cap_fd);
    if (l2cap_fd < 0)
    {
        perror("Fail to accept!\r\n");
        close(l2cap_sock);
        return NULL;
    }

    close(l2cap_sock);

    ba2str(&l2cap_clientaddr.l2_bdaddr, l2cap_client_bdaddr);
    BLE_LOG("Connect from : %s...\r\n", l2cap_client_bdaddr);

    gatt_server.att = bt_att_new(l2cap_fd, false);
    if (!gatt_server.att)
    {
        perror("Failed to bt_att_new!\r\n");
        close(l2cap_fd);
        return NULL;
    }

    gatt_server.gatt = bt_gatt_server_new(gatt_server.db, gatt_server.att, MAX_MTU, 0);
    if (!gatt_server.gatt)
    {
        perror("Failed to bt_gatt_server_new!\r\n");
        close(l2cap_fd);
        return NULL;
    }

    create_server(&gatt_server);

    if (gatt_server.bt_msg_cb == NULL)
    {
        return NULL;
    }

    data.len = 0;
    data.data = NULL;
    gatt_server.bt_msg_cb(0, TY_BT_EVENT_CONNECTED, &data);
    mainloop_run();

    BLE_LOG("Function l2cap_server_pthread exited!\r\n");

    bt_gatt_server_unref(gatt_server.gatt);
    bt_att_unref(gatt_server.att);
    close(l2cap_fd);
}

/**
 * @brief tuya_os_adapt_bt 广播包重置
 * @return OPERATE_RET 
 */
int tuya_os_adapt_bt_reset_adv(tuya_ble_data_buf_t *adv, tuya_ble_data_buf_t *scan_resp)
{
    int ret = -1;
    pthread_t l2cap_server_thread;
    tuya_ble_data_buf_t adv_data = {0};
    tuya_ble_data_buf_t rsp_data = {0};
    le_set_advertising_parameters_cp adv_params_cp;

    BLE_LOG("Function tuya_os_adapt_bt_reset_adv called!\r\n");

    if ((adv == NULL) || (adv->len == 0) || (scan_resp == NULL) || (scan_resp->len == 0))
    {
        return OPRT_INVALID_PARM;
    }

    if (adv_data.data != NULL)
    {
        Free(adv_data.data);
        adv_data.data = NULL;
    }
    adv_data.len = adv->len;
    adv_data.data = Malloc(adv_data.len);
    if (adv_data.data != NULL)
    {
        memcpy(adv_data.data, adv->data, adv->len);
    }
    
    if (rsp_data.data != NULL)
    {
        Free(rsp_data.data);
        rsp_data.data = NULL;
    }
    rsp_data.len = scan_resp->len;
    rsp_data.data = Malloc(rsp_data.len);
    if (rsp_data.data != NULL)
    {
        memcpy(rsp_data.data, scan_resp->data, scan_resp->len);
    }

    /**
     * 需要实现的逻辑：
     *   a) 开启广播（数据：adv_data.data，数据长度：adv_data.len）。
     *   b) 保存扫描响应数据，在收到扫描响应请求时返回该数据（数据：rsp_data.data，数据长度：rsp_data.len）。
     */

    hci_le_set_advertise_enable(gatt_server.hci_sock, 0x00, 1000);

    memset(&adv_params_cp, 0, sizeof(adv_params_cp));
    adv_params_cp.min_interval = htobs(ADV_INTERVAL);
    adv_params_cp.max_interval = htobs(ADV_INTERVAL);
    adv_params_cp.advtype = 0;
    adv_params_cp.chan_map = 7;
    ret = hci_le_set_advertise_parameters(gatt_server.hci_sock, adv_params_cp);
    if (ret < 0)
    {
        perror("Fail to hci_le_set_advertise_parameters!\r\n");
        return EXIT_FAILURE;
    }

    ret = hci_le_set_advertise_data(gatt_server.hci_sock, adv_data.data, adv_data.len);
    if (ret < 0)
    {
        perror("Fail to hci_le_set_advertise_data!\r\n");
        return EXIT_FAILURE;
    }

    ret = hci_le_set_scan_rsp_data(gatt_server.hci_sock, rsp_data.data, rsp_data.len);
    if (ret < 0)
    {
        perror("Fail to hci_le_set_scan_rsp_data!\r\n");
        return EXIT_FAILURE;
    }

    BLE_LOG("Enable advertise!\r\n");
    ret = hci_le_set_advertise_enable(gatt_server.hci_sock, 0x01, 1000);
    if (ret < 0)
    {
        perror("Fail to hci_le_set_advertise_enable!\r\n");
        return EXIT_FAILURE;
    }

    pthread_create(&l2cap_server_thread, NULL, l2cap_server_pthread, NULL);
    pthread_detach(l2cap_server_thread);

    return OPRT_OK;
}


/**
 * @brief tuya_os_adapt_bt 获取 rssi 信号值
 * @return OPERATE_RET 
 */
int tuya_os_adapt_bt_get_rssi(signed char *rssi)
{
    BLE_LOG("Function tuya_os_adapt_bt_get_rssi called!\r\n");

    return OPRT_OK;
}


/**
 * @brief tuya_os_adapt_bt 停止广播
 * @return OPERATE_RET 
 */
int tuya_os_adapt_bt_start_adv(void)
{
    BLE_LOG("Function tuya_os_adapt_bt_start_adv called!\r\n");

    return OPRT_OK;
}


/**
 * @brief tuya_os_adapt_bt 停止广播
 * @return OPERATE_RET 
 */
int tuya_os_adapt_bt_stop_adv(void)
{
    BLE_LOG("Function tuya_os_adapt_bt_stop_adv called!\r\n");

    return OPRT_OK;
}


/**
 * @brief tuya_os_adapt_bt 主动扫描
 * @return OPERATE_RET 
 */
int tuya_os_adapt_bt_assign_scan(IN OUT ty_bt_scan_info_t *info)
{
    BLE_LOG("Function tuya_os_adapt_bt_assign_scan called!\r\n");

    return OPRT_OK;
}

/**
 * @brief tuya_os_adapt_bt 广播接收初始化，包括监控数据状态和接收数据函数（用于蓝牙遥控器 ffc）老基线函数 tuya_bt_ffc_regist
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_os_adapt_bt_scan_init(IN TY_BT_SCAN_ADV_CB scan_adv_cb)
{
    BLE_LOG("Function tuya_os_adapt_bt_scan_init called!\r\n");

    return OPRT_OK;
}

/**
 * @brief tuya_os_adapt_bt 广播接收 scan start
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_os_adapt_bt_start_scan(void)
{
    BLE_LOG("Function tuya_os_adapt_bt_start_scan called!\r\n");

    return OPRT_OK;
}

/**
 * @brief tuya_os_adapt_bt 广播接收 scan stop
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_os_adapt_bt_stop_scan(void)
{
    BLE_LOG("Function tuya_os_adapt_bt_stop_scan called!\r\n");

    return OPRT_OK;
}

/**
 * @brief tuya_os_adapt_bt 接口注册
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_os_adapt_reg_bt_intf(void)
{
    BLE_LOG("Function tuya_os_adapt_reg_bt_intf called!\r\n");

    return OPRT_OK;
}

bool check_hci0_exists()
{
    int ret;

    ret = system("hciconfig hci0 > /dev/null 2>&1");
    if (ret == 0) {
        BLE_LOG("hci0 exists.\r\n");
        return true;
    }

    BLE_LOG("hci0 does not exist.\r\n");
    return false;
}

bool check_hci0_runnning()
{
    int ret;
    char buffer[128];

    ret = system("hciconfig hci0 > /dev/null 2>&1");
    if (ret == 0) {
        FILE *fp = popen("hciconfig hci0 | grep RUNNING", "r");
        if (fp == NULL) {
            perror("popen failed");
            return false;
        }

        if (fgets(buffer, sizeof(buffer), fp) != NULL) {
            pclose(fp);
            BLE_LOG("hci0 is already up and running.\r\n");
            return true;
        }

        pclose(fp);
    }

    return false;
}

bool bt_start()
{
    if(check_hci0_runnning())
    {
        return true;
    }
    system("bt_init.sh");
    for(int i=0; i<100; i++)
    {
        if(check_hci0_exists())
        {
            system("hciconfig hci0 up");
            break;
        }
        usleep(100000);
    }

    return check_hci0_runnning();
}

OPERATE_RET tuya_bt_init(VOID)
{
    BLE_LOG("Function tuya_bt_init called!\r\n");

    bt_start();
    
    return tuya_os_adapt_reg_intf(INTF_BT, &m_tuya_os_bt_intfs);
}

#endif
