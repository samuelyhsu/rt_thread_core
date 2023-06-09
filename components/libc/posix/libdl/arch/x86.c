/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author      Notes
 * 2018/09/15     parai     first version
 */

#include "../dlelf.h"
#include "../dlmodule.h"

#ifdef __i386__

#define R_X86_64_GLOB_DAT 6  /* Create GOT entry */
#define R_X86_64_JUMP_SLOT 7 /* Create PLT entry */
#define R_X86_64_RELATIVE 8  /* Adjust by program base */
int dlmodule_relocate(struct rt_dlmodule *module, Elf32_Rel *rel,
                      Elf32_Addr sym_val) {
  Elf32_Addr *where, tmp;
  Elf32_Sword addend, offset;
  rt_uint32_t upper, lower, sign, j1, j2;

  where = (Elf32_Addr *)((rt_uint8_t *)module->mem_space + rel->r_offset -
                         module->vstart_addr);

  switch (ELF32_R_TYPE(rel->r_info)) {
  case R_X86_64_GLOB_DAT:
  case R_X86_64_JUMP_SLOT:
    *where = (Elf32_Addr)sym_val;

    RT_DEBUG_LOG(RT_DEBUG_MODULE, ("R_X86_64_JUMP_SLOT: 0x%x -> 0x%x 0x%x\n",
                                   (uint32_t)where, *where, sym_val));
    break;
  case R_X86_64_RELATIVE:
    *where = (Elf32_Addr)sym_val + *where;
    RT_DEBUG_LOG(RT_DEBUG_MODULE, ("R_X86_64_RELATIVE: 0x%x -> 0x%x 0x%x\n",
                                   (uint32_t)where, *where, sym_val));
    break;
  default:
    RT_DEBUG_LOG(RT_DEBUG_MODULE, ("X86ELF: invalid relocate TYPE %d\n",
                                   ELF32_R_TYPE(rel->r_info)));
    return -1;
  }

  return 0;
}
#endif
