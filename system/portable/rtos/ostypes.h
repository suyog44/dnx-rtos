#ifndef OSTYPES_H_
#define OSTYPES_H_
/*=========================================================================*//**
@file    ostypes.h

@author  Daniel Zorychta

@brief   This file contains operating system types

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <stddef.h>             /* DNLFIXME czy na pewno potrzebne? */
#include "basic_types.h"        /* DNLFIXME czy na pewno potrzebne? */
#include "FreeRTOS.h"           /* DNLFIXME czy na pewno potrzebne? */
#include "task.h"               /* DNLFIXME czy na pewno potrzebne? */
#include "queue.h"              /* DNLFIXME czy na pewno potrzebne? */
#include "semphr.h"             /* DNLFIXME czy na pewno potrzebne? */

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
typedef void task_t;
typedef void sem_t;
typedef void mutex_t;

/*==============================================================================
   Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* OSTYPES_H_ */
/*==============================================================================
  End of file
==============================================================================*/
