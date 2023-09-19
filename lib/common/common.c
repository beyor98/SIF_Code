/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          common.c
 *
 * @author        wen.liu
 *
 * @version       1.1.0
 *
 * @date          2016/11/09
 *
 * @brief         Debug printf library.
 *
 * @note
 *    2015/09/08, wen.liu, V1.0.0
 *        Initial version.
 *    2016/11/09, wen.liu, V1.1.0
 *        Remove functions which have been included in microlib.
 *        Assert with printf and break point.
 */
#include "common.h"


#ifdef DEBUG
void assert_failed(uint8_t *file, uint32_t line)
{
#if ASSERT_WITH_PRINTF
    printf("ASSERT ERROR: file \"%s\" Line \"%d\" \n", file, line);
    /* Infinite loop */
    while (1)
    {}
#else
    __asm("bkpt #0");
#endif
}
#endif /* DEBUG */

