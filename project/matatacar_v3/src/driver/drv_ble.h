
#ifndef __DRV_BLE_H__
#define __DRV_BLE_H__

#define BLE_STATE_DISCONNECT    (0)
#define BLE_STATE_CONNECT       (1)

typedef struct{
    uint8_t protocol;
    uint8_t state;
    uint8_t heart_enable;
    uint8_t resv;
}ble_status_t;

typedef enum{
    MSG_PROTOCOL_V1 = 1,
    MSG_PROTOCOL_V2,
    MSG_PROTOCOL_V3,
}MSG_PROTOCOL_E;

typedef enum{
    IRQ_TYPE_CONNECT = 1,
    IRQ_TYPE_DISCONNECT,
    IRQ_TYPE_RECIVE,
}BLE_IRQ_TYPE_E;

typedef void* (*ble_irq_func)(int32_t type);
void ble_irq_event(int32_t event);
void ble_irq_func_register(ble_irq_func func);

uint32_t ble_get_event(void);

void ble_ringbuf_init(void);
void ble_init(void);
void ble_protocol_v1_msg_rx(const uint8_t *pdata, uint32_t length);
void ble_protocol_v2_msg_rx(const uint8_t *pdata, uint32_t length);
uint32_t ble_get_msg_len(void);
uint32_t ble_get_msg(uint8_t *buf);
void ble_set_connect_state(uint32_t connect_state);
uint8_t ble_get_connect_state(void);
void ble_set_protocol_version(uint8_t version);
uint8_t ble_get_protocol_version(void);
void ble_set_heart_enable(uint8_t heart_enable);
uint8_t ble_get_heart_enable(void);
int32_t ble_rx_data(uint8_t *buffer, uint32_t len);
int32_t ble_tx_data(uint8_t *buffer, uint32_t len);
int32_t ble_msg_process(uint8_t *buffer, uint32_t len);


#endif
