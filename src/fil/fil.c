/* FIL: Flash interface Layer
 * 
 * Copyright (c) 2024 inmyung.woo
 * 
 * SPDX-License-Identifier: Apache-2.0
 */
#include "fil_priv.h"

static bool g_is_ready = false;
static struct fil g_fil;
static struct flash_parameters g_flash_parameters;

const struct flash_parameters* fil_get_flash_parameters() {
    return &g_flash_parameters;
}

bool fil_is_ready() {
    return g_is_ready;
}

int fil_init(struct fil* pfil, struct flash_parameters* params) {
    if (!pfil || !params)
        return -EINVAL;
    g_is_ready = true;
    memcpy(&g_fil, pfil, sizeof(g_fil));
    memcpy(&g_flash_parameters, params, sizeof(g_flash_parameters));
    return 0;
}

int fil_read(off_t offset, void *data, size_t len) {
    if (g_fil.read == NULL)
        return -EPERM;

    if (g_fil.mutex_lock_forever)
        g_fil.mutex_lock_forever();

    int ret = g_fil.read(offset, data, len);

    if (g_fil.mutex_unlock)
        g_fil.mutex_unlock();
    return ret;
}

int fil_write(off_t offset, const void *data, size_t len) {
    if (g_fil.write == NULL)
        return -EPERM;

    if (g_fil.mutex_lock_forever)
        g_fil.mutex_lock_forever();

    int ret = g_fil.write(offset, data, len);

    if (g_fil.mutex_unlock)
        g_fil.mutex_unlock();
    return ret;
}

int fil_erase(off_t offset, size_t len) {
    if (g_fil.erase == NULL)
        return -EPERM;

    if (g_fil.mutex_lock_forever)
        g_fil.mutex_lock_forever();

    int ret = g_fil.erase(offset, len);

    if (g_fil.mutex_unlock)
        g_fil.mutex_unlock();
    return ret;
}