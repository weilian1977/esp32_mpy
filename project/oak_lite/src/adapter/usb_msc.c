// Copyright 2020-2021 Espressif Systems (Shanghai) Co. Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "esp_err.h"
#include "esp_log.h"
#include "esp_partition.h"

#include "usb_msc.h"

#define BLOCK_SIZE          CFG_TUD_MSC_BUFSIZE

#define LOGICAL_DISK_NUM 2
static bool ejected[LOGICAL_DISK_NUM] = {true};

const esp_partition_t *partition1;
const esp_partition_t *partition2;
esp_err_t usb_msc_init()
{
    partition1 = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "vfs1");
    assert(partition1 != NULL);
    partition2 = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "vfs2");
    assert(partition2 != NULL);
    ESP_LOGD(__func__, "");
    return ESP_OK;
}

//--------------------------------------------------------------------+
// tinyusb callbacks
//--------------------------------------------------------------------+

// Invoked to determine max LUN
uint8_t tud_msc_get_maxlun_cb(void)
{
  return 2; // dual LUN
}

// Invoked when device is mounted
void tud_mount_cb(void)
{
    // Reset the ejection tracking every time we're plugged into USB. This allows for us to battery
    // power the device, eject, unplug and plug it back in to get the drive.
    for (uint8_t i = 0; i < LOGICAL_DISK_NUM; i++) {
        ejected[i] = false;
    }

    ESP_LOGD(__func__, "");
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
    ESP_LOGW(__func__, "");
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allows us to perform remote wakeup
// USB Specs: Within 7ms, device must draw an average current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    ESP_LOGW(__func__, "");
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
    ESP_LOGW(__func__, "");
}

// Callback invoked when WRITE10 command is completed (status received and accepted by host).
// used to flush any pending cache.
void tud_msc_write10_complete_cb(uint8_t lun)
{
    if (lun >= LOGICAL_DISK_NUM) {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return;
    }

    // This write is complete, start the autoreload clock.
    ESP_LOGD(__func__, "");
}

static bool _logical_disk_ejected(void)
{
    bool all_ejected = true;

    for (uint8_t i = 0; i < LOGICAL_DISK_NUM; i++) {
        all_ejected &= ejected[i];
    }

    return all_ejected;
}

// Invoked when received SCSI_CMD_INQUIRY
// Application fill vendor id, product id and revision with string up to 8, 16, 4 characters respectively
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM) {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return;
    }
    const char vid[] = "Espressif";
    const char pid[] = "Mass Storage1";
    const char rev[] = "1.0";

    memcpy(vendor_id, vid, strlen(vid));
    memcpy(product_id, pid, strlen(pid));
    memcpy(product_rev, rev, strlen(rev));
    // if (lun == 0)
    // {
    //     const char vid[] = "Espressif";
    //     const char pid[] = "Mass Storage1";
    //     const char rev[] = "1.0";

    //     memcpy(vendor_id, vid, strlen(vid));
    //     memcpy(product_id, pid, strlen(pid));
    //     memcpy(product_rev, rev, strlen(rev));
    // }
    // if (lun == 1)
    // {
    //     const char vid[] = "Espressif";
    //     const char pid[] = "Mass Storage2";
    //     const char rev[] = "1.0";

    //     memcpy(vendor_id, vid, strlen(vid));
    //     memcpy(product_id, pid, strlen(pid));
    //     memcpy(product_rev, rev, strlen(rev));
    // }
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool tud_msc_test_unit_ready_cb(uint8_t lun)
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM) {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return false;
    }

    if (_logical_disk_ejected()) {
        // Set 0x3a for media not present.
        tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3A, 0x00);
        return false;
    }
    return true;
}

// Invoked when received SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size
// Application update block count and block size
void tud_msc_capacity_cb(uint8_t lun, uint32_t *block_count, uint16_t *block_size)
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM) {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return;
    }
    if (lun == 0)
    {
        *block_size = BLOCK_SIZE;
        *block_count = partition1->size/BLOCK_SIZE;
    }
    if (lun == 1)
    {
        *block_size = BLOCK_SIZE;
        *block_count = partition2->size/BLOCK_SIZE;
    }
    ESP_LOGD(__func__, "GET_SECTOR_COUNT = %d, GET_SECTOR_SIZE = %d", *block_count, *block_size);
}

bool tud_msc_is_writable_cb(uint8_t lun)
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM) {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return false;
    }
    return true;
}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
    ESP_LOGD(__func__, "");
    (void) power_condition;

    if (lun >= LOGICAL_DISK_NUM) {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return false;
    }

    if (load_eject) {
        if (!start) {
            ejected[lun] = true;
        } else {
            // We can only load if it hasn't been ejected.
            ejected[lun] = false;
        }
    } else {
        // Always start the unit, even if ejected. Whether media is present is a separate check.
    }

    return true;
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize)
{
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM) {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return 0;
    }
    if (lun == 0)
    {
        ESP_ERROR_CHECK(esp_partition_read(partition1, lba * BLOCK_SIZE, buffer, bufsize));
    }
    if (lun == 1)
    {
        ESP_ERROR_CHECK(esp_partition_read(partition2, lba * BLOCK_SIZE, buffer, bufsize));
    }
    return bufsize;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize)
{
    ESP_LOGD(__func__, "");
    (void) offset;

    if (lun >= LOGICAL_DISK_NUM) {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return 0;
    }

    if (lun == 0)
    {
        ESP_ERROR_CHECK(esp_partition_erase_range(partition1, lba * BLOCK_SIZE, bufsize));
        ESP_ERROR_CHECK(esp_partition_write(partition1, lba * BLOCK_SIZE, buffer, bufsize));
    }
    if (lun == 1)
    {
        ESP_ERROR_CHECK(esp_partition_erase_range(partition2, lba * BLOCK_SIZE, bufsize));
        ESP_ERROR_CHECK(esp_partition_write(partition2, lba * BLOCK_SIZE, buffer, bufsize));
    }
    return bufsize;
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// - READ10 and WRITE10 has their own callbacks
int32_t tud_msc_scsi_cb(uint8_t lun, uint8_t const scsi_cmd[16], void *buffer, uint16_t bufsize)
{
    // read10 & write10 has their own callback and MUST not be handled here
    ESP_LOGD(__func__, "");

    if (lun >= LOGICAL_DISK_NUM) {
        ESP_LOGE(__func__, "invalid lun number %u", lun);
        return 0;
    }

    void const *response = NULL;
    uint16_t resplen = 0;

    // most scsi handled is input
    bool in_xfer = true;

    switch (scsi_cmd[0]) {
        case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
            // Host is about to read/write etc ... better not to disconnect disk
            resplen = 0;
            break;

        default:
            // Set Sense = Invalid Command Operation
            tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);

            // negative means error -> tinyusb could stall and/or response with failed status
            resplen = -1;
            break;
    }

    // return resplen must not larger than bufsize
    if (resplen > bufsize) {
        resplen = bufsize;
    }

    if (response && (resplen > 0)) {
        if (in_xfer) {
            memcpy(buffer, response, resplen);
        } else {
            // SCSI output
        }
    }

    return resplen;
}
