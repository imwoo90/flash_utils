/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2015 Runtime Inc
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <fcb.h>
#include "fcb_priv.h"

/*
 * Call 'cb' for every element in flash circular buffer. If sector is specified,
 * only elements with that sector are reported.
 */
int fcb_walk(struct fcb *fcb, uint32_t sector, fcb_walk_cb cb,
			 void *cb_arg)
{
	struct fcb_entry_ctx entry_ctx;
	int rc;

	entry_ctx.loc.fe_sector = sector;
	entry_ctx.loc.fe_elem_off = 0U;

	if (fcb->impl_mutex_lock_forever)
	{
		rc = fcb->impl_mutex_lock_forever();
		if (rc)
		{
			return -EINVAL;
		}
	}
	while ((rc = fcb_getnext_nolock(fcb, &entry_ctx.loc)) !=
		   -ENOTSUP)
	{
		if (fcb->impl_mutex_unlock)
			fcb->impl_mutex_unlock();
		if (sector != -1 && entry_ctx.loc.fe_sector != sector)
		{
			return 0;
		}

		rc = cb(&entry_ctx, cb_arg);
		if (rc)
		{
			return rc;
		}
		if (fcb->impl_mutex_lock_forever)
		{
			rc = fcb->impl_mutex_lock_forever();
			if (rc)
			{
				return -EINVAL;
			}
		}
	}
	if (fcb->impl_mutex_unlock)
		fcb->impl_mutex_unlock();
	return 0;
}
