/*
 * Copyright (c) 2024 imwoo90
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>

#include <fcb.h>
#include "fcb_priv.h"
#include "../zephyr_macros.h"

int fcb_pop(struct fcb *fcbp, struct fcb_entry *pop_loc)
{
    int rc = 0;
    uint8_t fl_em[fcbp->f_align];
    off_t off;

    if (fcbp->impl_mutex_lock_forever)
    {
        rc = fcbp->impl_mutex_lock_forever();
        if (rc)
        {
            return -EINVAL;
        }
    }

    pop_loc->fe_sector = -1;
    rc = fcb_getnext_nolock(fcbp, pop_loc);
    if (rc)
        goto out;

    if (fcbp->f_oldest != pop_loc->fe_sector)
        fcb_rotate(fcbp);

    off = pop_loc->fe_data_off + fcb_len_in_flash(fcbp, pop_loc->fe_data_len);
    rc = fcb_flash_read(fcbp, pop_loc->fe_sector, off, &fl_em, sizeof(fl_em));
    if (rc)
        goto out;

    fl_em[1] = 0;
    rc = fcb_flash_write(fcbp, pop_loc->fe_sector, off, &fl_em, sizeof(fl_em));
    fcbp->f_oldest_elem_off = pop_loc->fe_elem_off;
    if (rc)
        goto out;

out:
    if (fcbp->impl_mutex_unlock)
        fcbp->impl_mutex_unlock();

    return rc;
}