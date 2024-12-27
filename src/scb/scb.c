/* SCB: Static Circular Buffer
 *
 * Copyright (c) 2024 imwoo90
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <scb.h>

#include <string.h>

#define SCB_DATA_LEN_SIZE 4

static int scb_copy_data(struct scb *scb, uint8_t *data, uint32_t size)
{
    if (scb->back + size > scb->size)
    {
        uint32_t first_size = scb->size - scb->back;
        memcpy(scb->buffer + scb->back, data, first_size);
        memcpy(scb->buffer, data + first_size, size - first_size);
    }
    else
    {
        memcpy(scb->buffer + scb->back, data, size);
    }
    scb->back = (scb->back + size) % scb->size;
    return 0;
}

static uint32_t scb_data_len(struct scb *scb, uint32_t current)
{
    uint32_t size = 0;
    if (current + SCB_DATA_LEN_SIZE > scb->size)
    {
        uint32_t first_size = scb->size - current;
        memcpy(&size, scb->buffer + current, first_size);
        memcpy((uint8_t *)&size + first_size, scb->buffer, SCB_DATA_LEN_SIZE - first_size);
    }
    else
    {
        memcpy(&size, scb->buffer + current, SCB_DATA_LEN_SIZE);
    }
    return size;
}

static int _scb_peak(struct scb *scb, uint32_t current, uint8_t *buf, uint32_t b_size)
{
    uint32_t expected_size = scb_data_len(scb, current);
    if (expected_size > b_size)
    {
        return -1;
    }

    current = (current + SCB_DATA_LEN_SIZE) % scb->size;
    if (buf != NULL)
    {
        if (current + expected_size > scb->size)
        {
            uint32_t first_size = scb->size - current;
            memcpy(buf, scb->buffer + current, first_size);
            memcpy(buf + first_size, scb->buffer, expected_size - first_size);
        }
        else
        {
            memcpy(buf, scb->buffer + current, expected_size);
        }
    }
    return expected_size;
}

int scb_init(struct scb *scb, uint8_t *buffer, uint32_t size)
{
    scb->buffer = buffer;
    scb->size = size;
    scb->front = 0;
    scb->back = 0;
    scb->num_items = 0;
    return 0;
}

int scb_push(struct scb *scb, uint8_t *data, uint32_t size)
{
    if (!scb_is_pushable(scb, size + SCB_DATA_LEN_SIZE) || data == NULL)
    {
        return -1;
    }

    scb_copy_data(scb, (uint8_t *)&size, SCB_DATA_LEN_SIZE);
    scb_copy_data(scb, data, size);
    scb->num_items++;
    return 0;
}

int scb_peak(struct scb *scb, uint8_t *buf, uint32_t b_size)
{
    return _scb_peak(scb, scb->front, buf, b_size);
}

int scb_pop(struct scb *scb, uint8_t *buf, uint32_t b_size)
{
    if (scb_is_empty(scb))
    {
        return -1;
    }
    uint32_t expected_size = _scb_peak(scb, scb->front, buf, b_size);

    scb->front = (scb->front + SCB_DATA_LEN_SIZE + expected_size) % scb->size;
    scb->num_items--;
    return expected_size;
}

int scb_walk(struct scb *scb, uint8_t *buf, uint32_t b_size, scb_walk_cb cb)
{
    if (cb == NULL || buf == NULL || b_size == 0)
    {
        return -1;
    }

    uint32_t front = scb->front;
    uint32_t num_items = scb->num_items;
    for (uint32_t i = 0; i < num_items; i++)
    {
        uint32_t expected_size = _scb_peak(scb, front, buf, b_size);
        if (expected_size > b_size)
        {
            return -1;
        }

        int rc = cb(buf, expected_size);
        if (rc != 0)
        {
            return rc;
        }
        front = (front + SCB_DATA_LEN_SIZE + expected_size) % scb->size;
    }

    if (front != scb->back)
    {
        return -1;
    }

    return 0;
}

bool scb_is_empty(struct scb *scb)
{
    return scb->num_items == 0;
}

bool scb_is_pushable(struct scb *scb, uint32_t size)
{
    if (size < SCB_DATA_LEN_SIZE)
    {
        return false;
    }

    if (scb->front > scb->back)
    {
        return scb->front - scb->back >= size;
    }
    else
    {
        return scb->size - scb->back + scb->front >= size;
    }
}
