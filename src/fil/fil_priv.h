/* FIL: Flash interface Layer
 * 
 * Copyright (c) 2024 inmyung.woo
 * 
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef FIL_PRIV_H_
#define FIL_PRIV_H_

#include <string.h>
#include <errno.h>

#include <fil.h>

#ifdef __cplusplus
extern "C" {
#endif

const struct flash_parameters* fil_get_flash_parameters();
int fil_read(off_t offset, void *data, size_t len);
int fil_write(off_t offset, const void *data, size_t len);
int fil_erase(off_t offset, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* FIL_PRIV_H_ */