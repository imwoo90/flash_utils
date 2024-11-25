#include <gtest/gtest.h>
#include <fil.h>
#include <nvs.h>

static uint8_t flash_sim[4096*1024]; // 4M

int impl_read(off_t offset, void *data, size_t len) {
    memcpy(data, flash_sim + offset, len);
    return 0;
}
int impl_write(off_t offset, const void *data, size_t len) {
    memcpy(flash_sim + offset, data, len);
    return 0;
}
int impl_erase(off_t offset, size_t len) {
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

int init_before_test() {
    fil_init(&g_fil, &g_fp);
    memset(flash_sim, 0xff, sizeof(flash_sim));
    return 0;
}

struct nvs_fs g_nvs = {
    .offset = 0,
    .sector_size = 4096,
    .sector_count = 1024,
    .impl_init = init_before_test,
    .impl_mutex_lock_forever = NULL,
    .impl_mutex_unlock = NULL
};

int write_read() {
    char test_str_buf[100];
    for (int i = 0; i < 256; i++) {
        int len = sprintf(test_str_buf, "hello test %d", i);
        nvs_write(&g_nvs, i, test_str_buf, len+1);
    }

    char check_str_buf[100];
    for (int i = 0; i < 256; i++) {
        int len = sprintf(check_str_buf, "hello test %d", i);
        int read_len = nvs_read(&g_nvs, i, test_str_buf, sizeof(test_str_buf));
        
        if (len+1 != read_len)
            return -1;
        if (strcmp(check_str_buf, test_str_buf))
            return -2;
    }

    return 0;
}

TEST(NVSTest, nvsTest) {
    EXPECT_EQ(nvs_mount(&g_nvs), 0);
    EXPECT_EQ(write_read(), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}