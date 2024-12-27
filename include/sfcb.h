/* SFCB: Simple Flash Circular Buffer
 *
 * Copyright (c) 2024 imwoo90
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef SFCB_H_
#define SFCB_H_

#include <fcb.h>

#ifdef __cplusplus
extern "C"
{
#endif

	/**
	 * @brief Simple Flash Circular Buffer File system structure
	 */
	struct sfcb_fs
	{
		struct fcb _fcb;

		uint32_t total_data_size;
	};

	/**
	 * @brief Mount an SFCB file system onto the flash device specified in @p fs.
	 *
	 * @param[in] fs Pointer to file system
	 * @retval 0 Success
	 * @retval -ERRNO errno code if error
	 */
	int sfcb_mount(struct sfcb_fs *fs);

	/**
	 * @brief Clear the SFCB file system from flash.
	 *
	 * @param[in] fs Pointer to file system
	 * @retval 0 Success
	 * @retval -ERRNO errno code if error
	 */
	int sfcb_clear(struct sfcb_fs *fs);

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
	int sfcb_append(struct sfcb_fs *fs, const void *data, uint32_t len);

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
	int sfcb_append_forcibly(struct sfcb_fs *fs, const void *data, uint32_t len);

	/**
	 * read an oldest entry to circular buffer
	 *
	 * @param[in] fs Pointer to file system
	 * @param[in] buf Pointer of buf which is read buffer
	 * @param[in] len Length of buf
	 *
	 * @return 0 on success, non-zero on failure.
	 */
	int sfcb_read(struct sfcb_fs *fs, const void *buf, uint32_t len);

	/**
	 * Pop an oldest entry to circular buffer
	 *
	 * @param[in] fs Pointer to file system
	 *
	 * @return 0 on success, non-zero on failure.
	 */
	int sfcb_pop(struct sfcb_fs *fs);

	/**
	 * read and pop an oldest entry to circular buffer
	 *
	 * @param[in] fs Pointer to file system
	 * @param[in] buf Pointer of buf which is read buffer
	 * @param[in] len Length of buf
	 *
	 * @return 0 on success, non-zero on failure.
	 */
	int sfcb_read_and_pop(struct sfcb_fs *fs, const void *buf, uint32_t len);

	/**
	 * Check whether SFCB has any data.
	 *
	 * @param[in] fs Pointer to file system
	 *
	 * @return Positive value if fcb is empty, otherwise 0.
	 */
	int sfcb_is_empty(struct sfcb_fs *fs);

#ifdef __cplusplus
}
#endif

#endif /* SFCB_H_ */