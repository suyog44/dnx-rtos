/*=========================================================================*//**
@file    tty_bfr.c

@author  Daniel Zorychta

@brief   Code in this file is responsible for buffer support.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the  Free Software  Foundation;  either version 2 of the License, or
         any later version.

         This  program  is  distributed  in the hope that  it will be useful,
         but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         You  should  have received a copy  of the GNU General Public License
         along  with  this  program;  if not,  write  to  the  Free  Software
         Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


*//*==========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "system/dnxmodule.h"
#include "tty.h"
#include "tty_cfg.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define BUFFER_VALIDATION                       (u32_t)0xFF49421D

/*==============================================================================
  Local object types
==============================================================================*/
struct ttybfr {
        char    *line[_TTY_DEFAULT_TERMINAL_HEIGHT];
        u32_t    valid;
        u16_t    write_index;
        u16_t    read_index;
        u16_t    new_line_cnt;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME("TTY");

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize buffer
 *
 * @param bfr           buffer address
 *
 * @return if success buffer object, NULL on error
 */
//==============================================================================
ttybfr_t *ttybfr_new()
{
        ttybfr_t *bfr = calloc(1, sizeof(ttybfr_t));
        if (bfr) {
                bfr->valid = BUFFER_VALIDATION;
        }

        return bfr;
}

//==============================================================================
/**
 * @brief Destroy buffer object
 *
 * @param bfr           buffer object
 */
//==============================================================================
void ttybfr_delete(ttybfr_t *bfr)
{
        if (bfr) {
                if (bfr->valid == BUFFER_VALIDATION) {
                        bfr->valid = 0;
                        free(bfr);
                }
        }
}

//==============================================================================
/**
 * @brief Add new line to buffer
 *
 * @param bfr           buffer object
 * @param src           source
 * @param len           length
 */
//==============================================================================
void ttybfr_add_line(ttybfr_t *bfr, const char *src, size_t len)
{

}

//==============================================================================
/**
 * @brief Clear whole terminal
 *
 * @param bfr           buffer object
 */
//==============================================================================
extern void ttybfr_clear(ttybfr_t *bfr)
{

}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
