/* SFCB: Simple Flash Circular Buffer
 *
 * Copyright (c) 2024 imwoo90
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef SFCB_PRIV_H_
#define SFCB_PRIV_H_

#include "../fil/fil_priv.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define SFCB_HEADER_ID 0xcb
#define CRC16_SEED 0xffff

    typedef enum
    {
        SHORT_DATA, // Store data in header (next_loc is used for it)
        SPLIT_DATA_START,
        SPLIT_DATA,
        SPLIT_DATA_END,
        NORMAL_DATA,
    } sfcb_data_t;

    struct sfcb_header
    {
        uint32_t next_loc;
        uint8_t sfcb_header_id;
        uint8_t data_type;
        uint16_t crc16; /* crc16 check of the entry's header*/
    } __packed;

#ifdef __cplusplus
}
#endif

#endif /* SFCB_PRIV_H_ */