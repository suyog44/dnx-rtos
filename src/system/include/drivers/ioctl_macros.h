/*=========================================================================*//**
@file    ioctl_macros.h

@author  Daniel Zorychta

@brief   ioctl build macros.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


*//*==========================================================================*/

#ifndef _IOCTL_MACROS_H_
#define _IOCTL_MACROS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "ioctl_groups.h"

/*==============================================================================
  Exported macros
==============================================================================*/
#define _IO(g, n)               (((u32_t)(_IO_GROUP_##g) << 16) | ((u32_t)(n) & 0xFFFF))
#define _IOR(g, n, t)           (((u32_t)(_IO_GROUP_##g) << 16) | ((u32_t)(n) & 0xFFFF))
#define _IOW(g, n, t)           (((u32_t)(_IO_GROUP_##g) << 16) | ((u32_t)(n) & 0xFFFF))
#define _IOWR(g, n, t)          (((u32_t)(_IO_GROUP_##g) << 16) | ((u32_t)(n) & 0xFFFF))

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _IOCTL_MACROS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
