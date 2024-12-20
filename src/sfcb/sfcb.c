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

#define PAGE_SIZE 256 // Assume that the page size is 256

inline bool sfcb_is_header(const struct sfcb_header *hdr)
{
    return !crc16_ccitt(CRC16_SEED, hdr, sizeof(struct sfcb_header));
}

inline int sfcb_read_header(off_t offset, struct sfcb_header *hdr)
{
    fil_read(offset, hdr, sizeof(struct sfcb_header));
    if (sfcb_is_header(hdr))
    {
        return true;
    }
    return false;
}

inline int sfcb_get_fist_header(struct sfcb_fs *fs, int sector_idx, struct sfcb_header *hdr)
{
    off_t offset = fs->offset + sector_idx * fs->sector_size;
    return sfcb_read_header(offset, hdr);
}

unsigned int align_to_word(struct sfcb_fs *fs, unsigned int value)
{
    const uint32_t mask = fs->align_mask;
    return (value + mask) & ~mask; // 4바이트로 정렬
}

int sfcb_get_next_header(struct sfcb_fs *fs, off_t cur_off, struct sfcb_header *cur, struct sfcb_header *next)
{
    off_t next_header_offset;
    if (cur->data_type == SHORT_DATA)
    {
        next_header_offset = cur_off + sizeof(struct sfcb_header);
    }
    else
    {
        next_header_offset = align_to_word(fs, cur->next_loc);
    }

    if (sfcb_read_header(next_header_offset, next))
    {
        return next_header_offset;
    }
    return -1;
}

int sfcb_startup(struct sfcb_fs *fs)
{
    struct sfcb_header cur_hdr;

    int erased_idx = -1;
    int cnt_no_header = 0;
    for (int i = 0; i < fs->sector_count; i++)
    {
        if (!sfcb_get_fist_header(fs, i, &cur_hdr))
        {
            cnt_no_header += 1;
            erased_idx = i;
        }
    }

    if (cnt_no_header == 0)
    {
        // need initialize
        fil_erase(fs->offset, fs->sector_size);
        fs->oldest_loc = fs->offset;
        fs->latest_loc = fs->offset;
        return 0;
    }

    // find oldest
    int search_start = (erased_idx + 1) % fs->sector_count;
    off_t cur_off = fs->offset + fs->sector_size * search_start;
    struct sfcb_header next_hdr;
    if (sfcb_read_header(cur_off, &cur_hdr) == false)
    {
        // broken sfcb
        fil_erase(fs->offset, fs->sector_count * fs->sector_size);
        fs->oldest_loc = fs->offset;
        fs->latest_loc = fs->offset;
        return 0;
    }
    do
    {
        if (cur_hdr.data_type == SHORT_DATA || cur_hdr.data_type == NORMAL_DATA)
        {
            fs->oldest_loc = cur_off;
            break;
        }
        int next_off = sfcb_get_next_header(fs, cur_off, &cur_hdr, &next_hdr);
        if (next_off == -1)
        {
            // broken sfcb
            fil_erase(fs->offset, fs->sector_count * fs->sector_size);
            fs->oldest_loc = fs->offset;
            fs->latest_loc = fs->offset;
            return 0;
        }
        cur_off = next_off;
        memcpy(&cur_hdr, &next_hdr, sizeof(next_hdr));
    } while (1);

    // find latest
    do
    {
        int next_off = sfcb_get_next_header(fs, cur_off, &cur_hdr, &next_hdr);
        if (next_off == -1)
        {
            fs->latest_loc = cur_hdr.next_loc;
            break;
        }
        cur_off = next_off;
        memcpy(&cur_hdr, &next_hdr, sizeof(next_hdr));
    } while (1);

    return 0;
}

int sfcb_mount(struct sfcb_fs *fs)
{
    int rc;
    size_t write_block_size;

    if (fs->impl_init)
        fs->impl_init();

    if (!fil_is_ready())
    {
        LOG_ERR("Flash Interface Layer is not ready");
        return -EPERM;
    }

    write_block_size = fil_get_flash_parameters()->write_block_size;

    /* check that the write block size is supported */
    if (write_block_size == 0)
    {
        LOG_ERR("Unsupported write block size");
        return -EINVAL;
    }

    if (!fs->sector_size || fs->sector_size % write_block_size)
    {
        LOG_ERR("Invalid sector size");
        return -EINVAL;
    }
    fs->align_mask = write_block_size - 1;

    /* check the number of sectors, it should be at least 2 */
    if (fs->sector_count < 2)
    {
        LOG_ERR("Configuration error - sector count");
        return -EINVAL;
    }

    rc = sfcb_startup(fs);
    if (rc)
    {
        return rc;
    }

    LOG_INF("%d Sectors of %d bytes", fs->sector_count, fs->sector_size);
    LOG_INF("latest_loc[%x] oldest_loc[%x]", fs->latest_loc, fs->oldest_loc);

    return 0;
}

int sfcb_clear(struct sfcb_fs *fs)
{
}

int sfcb_append(struct sfcb_fs *fs, const void *data, uint32_t len)
{
}

int sfcb_append_forcibly(struct sfcb_fs *fs, const void *data, uint32_t len)
{
}

int sfcb_rotate(struct sfcb_fs *fs)
{
}

int sfcb_is_empty(struct sfcb_fs *fs)
{
}

int sfcb_getnext(struct sfcb_fs *fs, struct sfcb_entry *entry)
{
}

int sfcb_read(struct sfcb_fs *fs, struct sfcb_entry *entry)
{
}