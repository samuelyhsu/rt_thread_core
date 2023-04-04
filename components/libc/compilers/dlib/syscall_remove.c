/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-01-28     Bernard      first version
 */

#include <LowLevelIOInterface.h>
#include <compiler_private.h>
#include <rtthread.h>
#include <unistd.h>

#define DBG_TAG "dlib.syscall.remove"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

/*
 * The "remove" function should remove the file named "filename".  It
 * should return 0 on success and nonzero on failure.
 */

#pragma module_name = "?remove"

int remove(const char *filename) {
#ifdef DFS_USING_POSIX
  return unlink(filename);
#else
  LOG_W(_WARNING_WITHOUT_FS);
  return _LLIO_ERROR;
#endif /* DFS_USING_POSIX */
}
