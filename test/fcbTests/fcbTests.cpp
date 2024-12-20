#include <gtest/gtest.h>
#include <fil.h>
#include <fcb.h>

static uint8_t flash_sim[4096 * 1024]; // 4M

int impl_read(off_t offset, void *data, size_t len)
{
    memcpy(data, flash_sim + offset, len);
    return 0;
}
int impl_write(off_t offset, const void *data, size_t len)
{
    memcpy(flash_sim + offset, data, len);
    return 0;
}
int impl_erase(off_t offset, size_t len)
{
    memset(flash_sim + offset, 0xff, len);
    return 0;
}

struct fil g_fil = {
    .read = impl_read,
    .write = impl_write,
    .erase = impl_erase,
};

struct flash_parameters g_fp = {
    .write_block_size = 0x4,
    .erase_value = 0xff,
};

int init_before_test()
{
    fil_init(&g_fil, &g_fp);
    memset(flash_sim, 0xff, sizeof(flash_sim));
    return 0;
}

struct fcb g_fcb = {
    .offset = 0,
    .f_sector_size = 4096,
    .f_sector_cnt = 1024,
    .impl_init = init_before_test,
    .impl_mutex_lock_forever = NULL,
    .impl_mutex_unlock = NULL};

int fcb_append_data(fcb *fcbp, const void *src, uint16_t len)
{
    int rc;
    struct fcb_entry loc;
    rc = fcb_append(fcbp, len, &loc);
    if (rc)
    {
        return rc;
    }
    rc = fcb_flash_write(fcbp, loc.fe_sector, loc.fe_data_off, src, len);
    if (rc)
    {
        return rc;
    }
    rc = fcb_append_finish(fcbp, &loc);
    if (rc)
    {
        return rc;
    }
    return 0;
}

int fcb_walk_cb_test(struct fcb_entry_ctx *loc_ctx, void *arg)
{
    int test_type = *(int *)arg;
    char test_str_buf[100];
    struct fcb_entry *loc = &loc_ctx->loc;

    int rc = fcb_flash_read(&g_fcb, loc->fe_sector, loc->fe_data_off, test_str_buf, loc->fe_data_len);
    if (rc)
    {
        return -__LINE__;
    }

    if (test_type == 0)
    {
        if (loc->fe_sector == 0 || loc->fe_sector == (g_fcb.f_sector_cnt - 1) * g_fcb.f_sector_size)
        {
            printf("%s\n", test_str_buf);
        }
    }
    else if (test_type == 1)
    {
        printf("%s\n", test_str_buf);
    }
    return 0;
}

int write_read()
{
    int len, rc;
    int i = 0, r_cnt = 0;
    char test_str_buf[100];

    printf("read_write test!!!!!!\n");

    int cycle = 5; // test 5cycle write
    int rotate_cnt = (cycle - 1) * (sizeof(flash_sim) / g_fcb.f_sector_size);
    do
    {
        do
        {
            len = sprintf(test_str_buf, "hello test %d", i);
            i++;
        } while (fcb_append_data(&g_fcb, test_str_buf, len + 1) == 0);
        if (r_cnt >= rotate_cnt)
            break;

        rc = fcb_rotate(&g_fcb);
        if (rc)
            return rc;
        r_cnt += 1;
    } while (true);

    int test_type = 0;
    rc = fcb_walk(&g_fcb, -1, fcb_walk_cb_test, &test_type);
    if (rc)
        return rc;

    return 0;
}

int test_pop()
{
    char test_str_buf[100];
    struct fcb_entry loc;

    printf("pop test!@!@!!!!!!\n");
    while (fcb_pop(&g_fcb, &loc) == 0)
    {
        int rc = fcb_flash_read(&g_fcb, loc.fe_sector, loc.fe_data_off, test_str_buf, loc.fe_data_len);
        if (rc)
        {
            return -__LINE__;
        }
    };

    int len;
    for (int i = 0; i < 700000; i++)
    {
        len = sprintf(test_str_buf, "hello test %d", i);
        if (fcb_append_data(&g_fcb, test_str_buf, len + 1))
            return -__LINE__;

        if (i >= 5)
        {
            if (fcb_pop(&g_fcb, &loc))
                return -__LINE__;
        }
    }

    int test_type = 1;
    int rc = fcb_walk(&g_fcb, -1, fcb_walk_cb_test, &test_type);
    if (rc)
        return rc;

    return 0;
}

TEST(FCBTest, fcbTest)
{
    EXPECT_EQ(fcb_init(&g_fcb), 0);
    EXPECT_EQ(write_read(), 0);
    EXPECT_EQ(test_pop(), 0);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}