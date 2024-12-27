/* SCB: Static Circular Buffer
 *
 * Copyright (c) 2024 imwoo90
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SCB_H_
#define SCB_H_

#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef int (*scb_walk_cb)(uint8_t *data, uint32_t len);

    struct scb
    {
        uint8_t *buffer;
        uint32_t size;
        uint32_t front;
        uint32_t back;
        uint32_t num_items;
    };

    int scb_init(struct scb *scb, uint8_t *buffer, uint32_t size);
    int scb_push(struct scb *scb, uint8_t *data, uint32_t size);
    int scb_peak(struct scb *scb, uint8_t *buf, uint32_t b_size);
    int scb_pop(struct scb *scb, uint8_t *buf, uint32_t b_size);
    int scb_walk(struct scb *scb, uint8_t *buf, uint32_t b_size, scb_walk_cb cb);
    bool scb_is_empty(struct scb *scb);
    bool scb_is_pushable(struct scb *scb, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* SFCB_PRIV_H_ */