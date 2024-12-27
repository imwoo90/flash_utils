/* SFCB: Simple Flash Circular Buffer
 *
 * Copyright (c) 2024 imwoo90
 *
 * SPDX-License-Identifier: Apache-2.0
 */
// #include "fil_priv.h"

#include <crc.h>
#include <sfcb.h>
#include "sfcb_priv.h"
#include "../zephyr_macros.h"

struct sfcb_header
{
    uint16_t reserved;
    uint16_t num_of_check;
    uint32_t data_length;
};

int sfcb_mount(struct sfcb_fs *fs)
{
    int rc;
    rc = fcb_init(&fs->_fcb);
    if (rc)
        return rc;

    // total data size initialization

    return 0;
}

int sfcb_clear(struct sfcb_fs *fs)
{
    return fcb_clear(&fs->_fcb);
}

int sfcb_is_possible_to_append(struct sfcb_fs *fs, uint32_t len)
{
    struct fcb *fcb = &fs->_fcb;
    uint8_t tmp_str[MAX(8, fcb->f_align)];
    uint32_t cnt = fcb_put_len(fcb, tmp_str, len);
    if (cnt < 0)
    {
        return cnt;
    }
    cnt = fcb_len_in_flash(fcb, cnt);
    // len = fcb_len_in_flash(fcb, len) + fcb_len_in_flash(fcb, FCB_CRC_SZ);

    const uint32_t additional_len_for_each_chunk = cnt + fcb_len_in_flash(fcb, FCB_CRC_SZ);
    const uint32_t max_len_for_in_sector = fcb->f_sector_size - additional_len_for_each_chunk;

    fs->_fcb.f_active.fe_sector;
    fs->_fcb.f_active.fe_elem_off;

    return 0;
}

int sfcb_append(struct sfcb_fs *fs, const void *data, uint32_t len)
{
    int rc = 0;
    struct fcb_entry loc = {.fe_sector = -1};
    struct sfcb_header header = {.reserved = 0};
    if (len < sizeof(uint32_t))
    {
        header.num_of_check = 0;
        memcpy(&header.data_length, data, len);
    }

    rc = fcb_append(&fs->_fcb, sizeof(header), &loc);
    if (rc)
    {
        return rc;
    }

    return 0;
}

int sfcb_append_forcibly(struct sfcb_fs *fs, const void *data, uint32_t len)
{
    return 0;
}

int sfcb_read(struct sfcb_fs *fs, const void *buf, uint32_t len)
{
    return 0;
}

int sfcb_pop(struct sfcb_fs *fs)
{
    return 0;
}

int sfcb_read_and_pop(struct sfcb_fs *fs, const void *buf, uint32_t len)
{
    return 0;
}

int sfcb_is_empty(struct sfcb_fs *fs)
{
    return fs->total_data_size == 0 ? true : false;
}

int sfcb_get_next()
{
}