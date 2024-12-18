# SPDX-License-Identifier: Apache-2.0
target_compile_definitions(flash_utils PUBLIC
  #CONFIG_FCB_ALLOW_FIXED_ENDMARKER=y
)

target_sources(flash_utils PUBLIC
  fcb_append.c
  fcb.c
  fcb_elem_info.c
  fcb_getnext.c
  fcb_rotate.c
  fcb_walk.c
)