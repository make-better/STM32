#ifndef __FATFS_TEST_H__
#define __FATFS_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "ff.h"
uint8_t fatfs_test_flash(void);
uint8_t fatfs_test_sdcard(void);

#ifdef __cplusplus
}
#endif

#endif