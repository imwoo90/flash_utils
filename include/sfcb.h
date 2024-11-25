/*
 * 1. append
 * 2. walk
 * 3. get_next
 * 4. read
 * 5. rotate
 * 6. is_empty
 * 7. append_force
 * 
 * 1. mount
 */

/* SFCB: Simple Flash Circular Buffer
 * 
 * Copyright (c) 2024 inmyung.woo
 * 
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef SFCB_H_
#define SFCB_H_

#include <fil.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Simple Flash Circular Buffer File system structure
 */
struct sfcb_fs {
	 /** File system offset in flash **/
	off_t offset;
	/** File system is split into sectors, each sector must be multiple of erase-block-size */
	uint16_t sector_size;
	/** Number of sectors in the file system */
	uint16_t sector_count;
    /* APIs to implement */
	int (*impl_init)();
	int (*impl_mutex_lock_forever)();
	int (*impl_mutex_unlock)();

    off_t latest_loc; // location for new element
    off_t oldest_loc; // oldest element header location
    uint32_t align_mask;
};

/**
 * @brief Simple Flash Circular Buffer File system structure
 */
struct sfcb_entry {
	uint32_t fe_elem_off;
	/**< Offset from the start of the sector to beginning of element. */

    void* fe_data;
	uint32_t fe_data_len; /**< Size of data area in fcb entry*/
};

/**
 * @brief Mount an SFCB file system onto the flash device specified in @p fs.
 *
 * @param[in] fs Pointer to file system
 * @retval 0 Success
 * @retval -ERRNO errno code if error
 */
int sfcb_mount(struct sfcb_fs* fs);

/**
 * @brief Clear the SFCB file system from flash.
 *
 * @param[in] fs Pointer to file system
 * @retval 0 Success
 * @retval -ERRNO errno code if error
 */
int sfcb_clear(struct sfcb_fs* fs);

/**
 * Appends an entry to circular buffer.
 *
 * @param[in] fs Pointer to file system
 * @param[in] data Pointer of data which are expected to be written as the entry
 * @param[in] len Length of data which are expected to be written as the entry
 *            payload.
 *
 * @return 0 on success, -1 on failure because the expected data size will overflow, other on failure.
 */
int sfcb_append(struct sfcb_fs* fs, const void* data, uint32_t len);

/**
 * Appends an entry to circular buffer forcibly when failed because expected data size overflowed
 *
 * @param[in] fs Pointer to file system
 * @param[in] data Pointer of data which are expected to be written as the entry
 * @param[in] len Length of data which are expected to be written as the entry
 *            payload.
 *
 * @return 0 on success, non-zero on failure.
 */
int sfcb_append_forcibly(struct sfcb_fs* fs, const void* data, uint32_t len);

/**
 * Rotate SFCB
 *
 * Function erases the data from oldest sector. Upon that the next sector
 * becomes the oldest.
 *
 * @param[in] fs Pointer to file system
 */
int sfcb_rotate(struct sfcb_fs* fs);

/**
 * Check whether SFCB has any data.
 *
 * @param[in] fs Pointer to file system
 *
 * @return Positive value if fcb is empty, otherwise 0.
 */
int sfcb_is_empty(struct sfcb_fs* fs);

/**
 * Get next sfcb entry information for the size of data to read.
 *
 * @param[in] fs Pointer to file system
 * @param[in,out] entry entry information (element location, data length)
 *
 * @return 0 on success, non-zero on failure.
 */
int sfcb_getnext(struct sfcb_fs* fs, struct sfcb_entry* entry);

/**
 * Read data of entry.
 *
 * @param[in] fs Pointer to file system
 * @param[in, out] entry entry for read data
 *
 * @return 0 on success, non-zero on failure.
 */
int sfcb_read(struct sfcb_fs* fs, struct sfcb_entry* entry);
#ifdef __cplusplus
}
#endif


#endif /* SFCB_H_ */