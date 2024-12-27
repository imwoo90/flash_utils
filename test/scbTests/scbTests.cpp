#include <gtest/gtest.h>
#include <scb.h>

static uint8_t _buffer[4096]; // 4KB

struct scb g_scb;

int write_read()
{
    const int cnt_items = 5;
    int len, i = 0;
    uint8_t test_data[100];
    for (int i = 0; i < 999999; i++)
    {
        len = sprintf((char *)test_data, "hello test %d", i);
        if (scb_push(&g_scb, test_data, len + 1))
            return -1;

        if (i >= cnt_items)
        {
            int read_data_len = scb_pop(&g_scb, test_data, sizeof(test_data));
            if (read_data_len <= 0)
                return -1;
        }
    }

    return g_scb.num_items == cnt_items;
}

TEST(NVSTest, nvsTest)
{
    EXPECT_EQ(scb_init(&g_scb, _buffer, sizeof(_buffer)), 0);
    EXPECT_EQ(write_read(), 1);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}