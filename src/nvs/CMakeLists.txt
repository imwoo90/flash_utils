# SPDX-License-Identifier: Apache-2.0
target_compile_definitions(flash_utils PUBLIC
  CONFIG_NVS_LOOKUP_CACHE
  CONFIG_NVS_LOOKUP_CACHE_SIZE=256
  CONFIG_NVS_DATA_CRC
)

target_sources(flash_utils PUBLIC
  nvs.c
)
