/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "bleprph.h"
#include "drv_ble.h"
/**
 * The vendor specific security test service consists of two characteristics:
 *     o random-number-generator: generates a random 32-bit number each time
 *       it is read.  This characteristic can only be read over an encrypted
 *       connection.
 *     o static-value: a single-byte characteristic that can always be read,
 *       but can only be written over an encrypted connection.
 */


/* {6E400001-B5A3-F393-E0A9-E50E24DCCA9E} */
const ble_uuid128_t gatt_svr_svc_uart_uuid =
    BLE_UUID128_INIT(0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
                     0x93, 0xf3, 0xa3, 0xb5, 0x01, 0x00, 0x40, 0x6e);

/* {6E400002-B5A3-F393-E0A9-E50E24DCCA9E} */
const ble_uuid128_t gatt_svr_chr_uart_write_uuid =
    BLE_UUID128_INIT(0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
                     0x93, 0xf3, 0xa3, 0xb5, 0x02, 0x00, 0x40, 0x6e);


/* {6E400003-B5A3-F393-E0A9-E50E24DCCA9E} */
const ble_uuid128_t gatt_svr_chr_uart_read_uuid =
    BLE_UUID128_INIT(0x9e, 0xca, 0xdc, 0x24, 0x0e, 0xe5, 0xa9, 0xe0,
                     0x93, 0xf3, 0xa3, 0xb5, 0x03, 0x00, 0x40, 0x6e);



static uint8_t msg_buffer[32];


/* ble uart attr read handle */
uint16_t g_ble_attr_read_handle;

/* ble uart attr write handle */
uint16_t g_ble_attr_write_handle;



extern uint16_t g_conn_handle;
uint8_t msg_heart[] = {"Car:[0x87]"};

static int gatt_svr_chr_access_uart(uint16_t conn_handle, uint16_t attr_handle,
                             struct ble_gatt_access_ctxt *ctxt,
                             void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        /*** Service: uart. */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_uart_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[])
        { {
                /*** Characteristic: uart read. */
                .uuid = &gatt_svr_chr_uart_read_uuid.u,
                .val_handle = &g_ble_attr_read_handle,
                .access_cb = gatt_svr_chr_access_uart,
                .flags = BLE_GATT_CHR_F_NOTIFY,
            }, {
                /*** Characteristic: uart write. */
                .uuid = &gatt_svr_chr_uart_write_uuid.u,
                .val_handle = &g_ble_attr_write_handle,
                .access_cb = gatt_svr_chr_access_uart,
                .flags = BLE_GATT_CHR_F_WRITE_NO_RSP | BLE_GATT_CHR_F_WRITE,
            }, {
                0, /* No more characteristics in this service. */
            }
        },
    },

    {
        0, /* No more services. */
    },
};


    
uint32_t ble_send_uplink(uint8_t *buf, uint8_t length)
{
    struct os_mbuf *om;
    uint32_t rc;

    rc = ble_gap_conn_find(g_conn_handle, NULL);
    if(rc)
    {
        /**not connect **/
        return rc;
    }

    om = ble_hs_mbuf_from_flat(buf, length);
    if (!om) {
        return 1;
    }
    
    rc = ble_gattc_notify_custom(g_conn_handle,
                            g_ble_attr_read_handle, om);
    return rc;
}

static int
gatt_svr_chr_write(struct os_mbuf *om, uint16_t min_len, uint16_t max_len,
                   void *dst, uint16_t *len)
{
    uint16_t om_len;
    int rc;

    om_len = OS_MBUF_PKTLEN(om);
    if (om_len < min_len || om_len > max_len) {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    rc = ble_hs_mbuf_to_flat(om, dst, max_len, len);
    if (rc != 0) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    return 0;
}

static int gatt_svr_chr_access_uart(uint16_t conn_handle, uint16_t attr_handle,
                             struct ble_gatt_access_ctxt *ctxt,
                             void *arg)
{
    
    const ble_uuid_t *uuid;
    int rc;
    int rand_num;


    //int i;
    //uint8_t *p_data;

    uuid = ctxt->chr->uuid;

    if(ble_uuid_cmp(uuid, &gatt_svr_chr_uart_read_uuid.u) == 0){
        rc = 0;
        
        rc = os_mbuf_append(ctxt->om, &msg_heart, sizeof(msg_heart));
        switch (ctxt->op) {
            case BLE_GATT_ACCESS_OP_READ_CHR:
                
                rand_num = rand();
                rc = os_mbuf_append(ctxt->om, &rand_num, sizeof rand_num);
                //rc = os_mbuf_append(ctxt->om, &msg_heart, sizeof(msg_heart));
                return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
                break;
            
            default:
                MODLOG_DFLT(INFO, "unknow read uuid op: %d\n", ctxt->op);
                break;
        }
        return rc;

    }
    
    if(ble_uuid_cmp(uuid, &gatt_svr_chr_uart_write_uuid.u) == 0){
        rc = 0;
        switch (ctxt->op) {
            case BLE_GATT_ACCESS_OP_WRITE_CHR:
                rc = gatt_svr_chr_write(ctxt->om, 0, sizeof(msg_buffer), &msg_buffer, NULL);
                ble_rx_data(msg_buffer, ctxt->om->om_len);
            
                // MODLOG_DFLT(INFO, "write uuid recive data len: %d \n", ctxt->om->om_len);
                // print_bytes(msg_buffer, ctxt->om->om_len);
                // MODLOG_DFLT(INFO, "\n");

                //ble_send_uplink(msg_heart, sizeof(msg_heart));
                break;
            
            default:
                MODLOG_DFLT(INFO, "unknow write uuid op: %d\n", ctxt->op);
                break;
        }
        return rc;

    }
    /* Unknown characteristic; the nimble stack should not have called this
     * function.
     */
    assert(0);
    return BLE_ATT_ERR_UNLIKELY;


}

void
gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    char buf[BLE_UUID_STR_LEN];

    switch (ctxt->op) {
    case BLE_GATT_REGISTER_OP_SVC:
        MODLOG_DFLT(DEBUG, "registered service %s with handle=%d\n",
                    ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                    ctxt->svc.handle);
        break;

    case BLE_GATT_REGISTER_OP_CHR:
        MODLOG_DFLT(DEBUG, "registering characteristic %s with "
                    "def_handle=%d val_handle=%d\n",
                    ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                    ctxt->chr.def_handle,
                    ctxt->chr.val_handle);
        break;

    case BLE_GATT_REGISTER_OP_DSC:
        MODLOG_DFLT(DEBUG, "registering descriptor %s with handle=%d\n",
                    ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                    ctxt->dsc.handle);
        break;

    default:
        assert(0);
        break;
    }
}

int
gatt_svr_init(void)
{
    int rc;

    ble_svc_gap_init();
    ble_svc_gatt_init();

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    return 0;
}
