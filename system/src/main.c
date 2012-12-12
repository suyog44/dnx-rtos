/*=============================================================================================*//**
@file    mian.c

@author  Daniel Zorychta

@brief   This file provide system initialisation and RTOS start.

@note    Copyright (C) 2012  Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                             Include files
==================================================================================================*/
#include "initd.h"
#include "misc.h"


/*==================================================================================================
                                   Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static void basicConf(void);


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                         Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Main function
 */
//================================================================================================//
int main(void)
{
      /* basic configuration depending on architecture */
      basicConf();

      /* dynamic memory management initialization */
      mm_init();

      /* create idit task */
      TaskCreate(Initd, INITD_NAME, INITD_STACK_SIZE, NULL, 2, NULL);

      /* start OS */
      vTaskStartScheduler();

      return 0;
}


//================================================================================================//
/**
 * @brief Basic configuration
 * Function configure the basic of the basic configuration of CPU. This function is depending on
 * CPU architecture.
 */
//================================================================================================//
static void basicConf(void)
{
      /* set interrupt vectors and NVIC priority */
      NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
      NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                             End of file
==================================================================================================*/
