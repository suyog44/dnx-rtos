/*=========================================================================*//**
@file    cpu_flags.h

@author  Daniel Zorychta

@brief   CPU module configuration.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#ifndef _CPU_H_
#define _CPU_H_

/* configuration to modify */
#define __CPU_NAME__ STM32F107RCxx
#define __CPU_FAMILY__ _STM32F10X_CL_

/* fixed configuration */
#define _CPU_START_FREQUENCY_           (8000000UL)
#define _CPU_HEAP_ALIGN_                (4)
#define _CPU_IRQ_RTOS_KERNEL_PRIORITY_  (0xFF)
#define _CPU_IRQ_RTOS_SYSCALL_PRIORITY_ (0xEF)
#define ARCH_stm32f1

/* current CPU family definitions */
#define _STM32F10X_LD_VL_ 0
#define _STM32F10X_MD_VL_ 1
#define _STM32F10X_HD_VL_ 2
#define _STM32F10X_LD_    3
#define _STM32F10X_MD_    4
#define _STM32F10X_HD_    5
#define _STM32F10X_XL_    6
#define _STM32F10X_CL_    7

#if   (__CPU_FAMILY__ == _STM32F10X_LD_VL_)
#define STM32F10X_LD_VL
#elif (__CPU_FAMILY__ == _STM32F10X_MD_VL_)
#define STM32F10X_MD_VL
#elif (__CPU_FAMILY__ == _STM32F10X_HD_VL_)
#define STM32F10X_HD_VL
#elif (__CPU_FAMILY__ == _STM32F10X_LD_)
#define STM32F10X_LD
#elif (__CPU_FAMILY__ == _STM32F10X_MD_)
#define STM32F10X_MD
#elif (__CPU_FAMILY__ == _STM32F10X_HD_)
#define STM32F10X_HD
#elif (__CPU_FAMILY__ == _STM32F10X_XL_)
#define STM32F10X_XL
#elif (__CPU_FAMILY__ == _STM32F10X_CL_)
#define STM32F10X_CL
#else
#error Wrong CPU family
#endif

#endif /* _CONFIG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
