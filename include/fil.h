/* FIL: Flash interface Layer
 *
 * Copyright (c) 2024 imwoo90
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef FIL_H_
#define FIL_H_

#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    struct flash_parameters
    {
        const size_t write_block_size; // word size ex) 4bytes
        uint8_t erase_value;
    };

    struct fil
    {
        int (*read)(off_t offset, void *data, size_t len);
        int (*write)(off_t offset, const void *data, size_t len);
        int (*erase)(off_t offset, size_t len);
        int (*mutex_lock_forever)(void);
        int (*mutex_unlock)(void);
    };

    int fil_init(struct fil *fil, struct flash_parameters *params);
    bool fil_is_ready();
#ifdef __cplusplus
}
#endif

#endif /* FIL_H_ */