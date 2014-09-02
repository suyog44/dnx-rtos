/*=========================================================================*//**
@file    irq.c

@author  Daniel Zorychta

@brief   This driver support external interrupts (EXTI).

@note    Copyright (C) 2014  Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include "core/module.h"
#include <dnx/misc.h>
#include <dnx/thread.h>
#include "stm32f1/irq_cfg.h"
#include "stm32f1/irq_def.h"
#include "stm32f1/stm32f10x.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define NUMBER_OF_IRQs          16

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
typedef struct {
        sem_t *irqsem[NUMBER_OF_IRQs];
        u32_t  expected_irq;
} IRQ_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static enum IRQn EXTI_IRQ_number_to_NVIC_IRQn(uint EXTI_IRQ_n);
static void change_EXTI_IRQ_state(uint EXTI_IRQ_n, bool state);
static void set_EXTI_IRQ_priority(uint EXTI_IRQ_n, u32_t priority);
static void set_EXTI_edge_detector(uint EXTI_IRQ_n, bool falling, bool rising);
static inline void disable_EXTI_IRQ(uint EXTI_IRQ_n);
static inline void enable_EXTI_IRQ(uint EXTI_IRQ_n);
static bool IRQ_handler(uint EXIT_IRQ_n);

/*==============================================================================
  Local object definitions
==============================================================================*/
MODULE_NAME("IRQ");

static const enum _IRQ_MODE default_config[NUMBER_OF_IRQs] = {
        _IRQ_LINE_0_MODE,
        _IRQ_LINE_1_MODE,
        _IRQ_LINE_2_MODE,
        _IRQ_LINE_3_MODE,
        _IRQ_LINE_4_MODE,
        _IRQ_LINE_5_MODE,
        _IRQ_LINE_6_MODE,
        _IRQ_LINE_7_MODE,
        _IRQ_LINE_8_MODE,
        _IRQ_LINE_9_MODE,
        _IRQ_LINE_10_MODE,
        _IRQ_LINE_11_MODE,
        _IRQ_LINE_12_MODE,
        _IRQ_LINE_13_MODE,
        _IRQ_LINE_14_MODE,
        _IRQ_LINE_15_MODE
};

static const u8_t EXTI_priority[NUMBER_OF_IRQs] = {
        _IRQ_LINE_0_PRIO,
        _IRQ_LINE_1_PRIO,
        _IRQ_LINE_2_PRIO,
        _IRQ_LINE_3_PRIO,
        _IRQ_LINE_4_PRIO,
        _IRQ_LINE_5_PRIO,
        _IRQ_LINE_6_PRIO,
        _IRQ_LINE_7_PRIO,
        _IRQ_LINE_8_PRIO,
        _IRQ_LINE_9_PRIO,
        _IRQ_LINE_10_PRIO,
        _IRQ_LINE_11_PRIO,
        _IRQ_LINE_12_PRIO,
        _IRQ_LINE_13_PRIO,
        _IRQ_LINE_14_PRIO,
        _IRQ_LINE_15_PRIO
};

static IRQ_t *IRQ;

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize device
 *
 * @param[out]          **device_handle        device allocated memory
 * @param[in ]            major                major device number
 * @param[in ]            minor                minor device number
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_INIT(IRQ, void **device_handle, u8_t major, u8_t minor)
{
        if (major == _IRQ_MAJOR_NUMBER && minor == _IRQ_MINOR_NUMBER) {
                IRQ_t *hdl = calloc(1, sizeof(IRQ_t));
                if (hdl) {
                        for (uint i = 0; i < NUMBER_OF_IRQs; i++) {
                                switch (default_config[i]) {
                                default:
                                case _IRQ_MODE_DISABLED:
                                        disable_EXTI_IRQ(i);
                                        break;

                                case _IRQ_MODE_FALLING_EDGE:
                                        set_EXTI_edge_detector(i, true, false);
                                        enable_EXTI_IRQ(i);
                                        break;

                                case _IRQ_MODE_RISING_EDGE:
                                        set_EXTI_edge_detector(i, false, true);
                                        enable_EXTI_IRQ(i);
                                        break;

                                case _IRQ_MODE_FALLING_AND_RISING_EDGE:
                                        set_EXTI_edge_detector(i, true, true);
                                        enable_EXTI_IRQ(i);
                                        break;
                                }

                                if (default_config[i] != _IRQ_MODE_DISABLED) {
                                        set_EXTI_IRQ_priority(i, EXTI_priority[i]);

                                        hdl->irqsem[i] = semaphore_new(1, 0);
                                        if (hdl->irqsem[i] == NULL) {
                                                for (int s = 0; s < NUMBER_OF_IRQs; s++) {
                                                        if (hdl->irqsem[s]) {
                                                                semaphore_delete(hdl->irqsem[s]);
                                                                hdl->irqsem[s] = NULL;
                                                        }
                                                }

                                                free(hdl);
                                                IRQ = NULL;
                                                return STD_RET_ERROR;
                                        }
                                }
                        }

                        *device_handle = hdl;

                        return STD_RET_OK;
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Release device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_RELEASE(IRQ, void *device_handle)
{
        UNUSED_ARG(device_handle);

        IRQ_t *hdl = device_handle;

        critical_section_begin();

        hdl->expected_irq = 0;

        for (uint i = 0; i < NUMBER_OF_IRQs; i++) {
                disable_EXTI_IRQ(i);

                if (hdl->irqsem[i]) {
                        semaphore_delete(hdl->irqsem[i]);
                        hdl->irqsem[i] = NULL;
                }
        }

        free(hdl);
        IRQ = NULL;

        critical_section_end();

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Open device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_OPEN(IRQ, void *device_handle, vfs_open_flags_t flags)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(flags);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Close device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_CLOSE(IRQ, void *device_handle, bool force)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(force);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Write data to device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 * @param[in ]           fattr                  file attributes
 *
 * @return number of written bytes, -1 if error
 */
//==============================================================================
API_MOD_WRITE(IRQ, void *device_handle, const u8_t *src, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(src);
        UNUSED_ARG(count);
        UNUSED_ARG(fpos);
        UNUSED_ARG(fattr);

        errno = EPERM;

        return 0;
}

//==============================================================================
/**
 * @brief Read data from device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[in ]           fattr                  file attributes
 *
 * @return number of read bytes, -1 if error
 */
//==============================================================================
API_MOD_READ(IRQ, void *device_handle, u8_t *dst, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(dst);
        UNUSED_ARG(count);
        UNUSED_ARG(fpos);
        UNUSED_ARG(fattr);

        errno = EPERM;

        return 0;
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return On success return 0 or 1. On error, -1 is returned, and errno set
 *         appropriately.
 */
//==============================================================================
API_MOD_IOCTL(IRQ, void *device_handle, int request, void *arg)
{
        UNUSED_ARG(device_handle);

        IRQ_t *hdl = device_handle;

        if (arg) {
                switch (request) {
                case IOCTL_IRQ__CATCH: {
                        const IRQ_number_t *irqn = arg;
                        if (irqn->number < NUMBER_OF_IRQs) {
                                if (hdl->irqsem[irqn->number]) {
                                        SET_BIT(hdl->expected_irq, 1 << irqn->number);
                                        return semaphore_wait(hdl->irqsem[irqn->number], irqn->timeout);
                                }
                        }

                        break;
                }

                case IOCTL_IRQ__TRIGGER: {
                        const IRQ_number_t *irqn = arg;
                        if (irqn->number < NUMBER_OF_IRQs) {
                                WRITE_REG(EXTI->SWIER, EXTI_SWIER_SWIER0 << irqn->number);
                                return 0;
                        }

                        break;
                }

                case IOCTL_IRQ__CONFIGURE: {
                        const IRQ_config_t *cfg = arg;
                        if (cfg->number < NUMBER_OF_IRQs) {
                                set_EXTI_edge_detector(cfg->number, cfg->falling_edge, cfg->rising_edge);
                                change_EXTI_IRQ_state(cfg->number, cfg->enabled);
                                return 0;
                        }

                        break;
                }

                default:
                        errno = EBADRQC;
                        return -1;
                }

                return 0;
        }

        errno = EINVAL;
        return -1;
}

//==============================================================================
/**
 * @brief Flush device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_FLUSH(IRQ, void *device_handle)
{
        UNUSED_ARG(device_handle);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Device information
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_STAT(IRQ, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG(device_handle);

        device_stat->st_size  = 0;
        device_stat->st_major = _IRQ_MAJOR_NUMBER;
        device_stat->st_minor = _IRQ_MINOR_NUMBER;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Converts EXTI IRQ number (0-15) to NVIC IRQ number
 *
 * @param EXTI_IRQ_n    EXTI IRQ number
 *
 * @return On success, NVIC IRQ number is returned, otherwise 0.
 */
//==============================================================================
static enum IRQn EXTI_IRQ_number_to_NVIC_IRQn(uint EXTI_IRQ_n)
{
        switch (EXTI_IRQ_n) {
        case 0  ... 4 : return EXTI0_IRQn + EXTI_IRQ_n;
        case 5  ... 9 : return EXTI9_5_IRQn;
        case 10 ... 15: return EXTI15_10_IRQn;
        default:        return 0;
        }
}

//==============================================================================
/**
 * @brief Function enable or disable selected EXTI interrupt
 *
 * @param EXTI_IRQ_n    EXTI IRQ number (0-15)
 * @param state         new interrupt state
 *
 * @return None
 */
//==============================================================================
static void change_EXTI_IRQ_state(uint EXTI_IRQ_n, bool state)
{
        enum IRQn IRQn = EXTI_IRQ_number_to_NVIC_IRQn(EXTI_IRQ_n);

        if (IRQn != 0) {
                if (state) {
                        NVIC_EnableIRQ(IRQn);
                        SET_BIT(EXTI->IMR, EXTI_IMR_MR0 << EXTI_IRQ_n);
                        SET_BIT(EXTI->EMR, EXTI_EMR_MR0 << EXTI_IRQ_n);
                } else {
                        NVIC_DisableIRQ(IRQn);
                        CLEAR_BIT(EXTI->IMR, EXTI_IMR_MR0 << EXTI_IRQ_n);
                        CLEAR_BIT(EXTI->EMR, EXTI_EMR_MR0 << EXTI_IRQ_n);
                }
        }
}

//==============================================================================
/**
 * @brief Function changes selected EXTI priority
 *
 * @param EXTI_IRQ_n    EXTI IRQ number
 * @param prio          new interrupt priority
 *
 * @return None
 */
//==============================================================================
static void set_EXTI_IRQ_priority(uint EXTI_IRQ_n, u32_t priority)
{
        enum IRQn IRQn = EXTI_IRQ_number_to_NVIC_IRQn(EXTI_IRQ_n);

        if (IRQn != 0) {
                NVIC_SetPriority(IRQn, priority);
        }
}

//==============================================================================
/**
 * @brief Changes interrupt detector sensitivity
 *
 * @param EXTI_IRQ_n    EXTI IRQ number
 * @param falling       interrupt triggered on falling edge
 * @param rising        interrupt triggered on rising edge
 *
 * @return None
 */
//==============================================================================
static void set_EXTI_edge_detector(uint EXTI_IRQ_n, bool falling, bool rising)
{
        if (EXTI_IRQ_n < NUMBER_OF_IRQs) {
                if (falling) {
                        SET_BIT(EXTI->FTSR, EXTI_FTSR_TR0 << EXTI_IRQ_n);
                } else {
                        CLEAR_BIT(EXTI->FTSR, EXTI_FTSR_TR0 << EXTI_IRQ_n);
                }

                if (rising) {
                        SET_BIT(EXTI->RTSR, EXTI_RTSR_TR0 << EXTI_IRQ_n);
                } else {
                        CLEAR_BIT(EXTI->RTSR, EXTI_RTSR_TR0 << EXTI_IRQ_n);
                }
        }
}

//==============================================================================
/**
 * @brief Disables selected EXTI interrupt
 *
 * @param EXTI_IRQ_n    EXTI IRQ number
 *
 * @return None
 */
//==============================================================================
static inline void disable_EXTI_IRQ(uint EXTI_IRQ_n)
{
        change_EXTI_IRQ_state(EXTI_IRQ_n, false);
}

//==============================================================================
/**
 * @brief Enables selected EXTI interrupt
 *
 * @param EXTI_IRQ_n    EXTI IRQ number
 *
 * @return None
 */
//==============================================================================
static inline void enable_EXTI_IRQ(uint EXTI_IRQ_n)
{
        change_EXTI_IRQ_state(EXTI_IRQ_n, true);
}

//==============================================================================
/**
 * @brief Handle interrupts for selected EXTI line
 *
 * @param EXTI_IRQ_n    EXTI IRQ number
 *
 * @return true if task woken, otherwise false
 */
//==============================================================================
static bool IRQ_handler(uint EXIT_IRQ_n)
{
        if (IRQ == NULL) {
                WRITE_REG(EXTI->PR, EXTI_PR_PR0 << EXIT_IRQ_n);
                return false;
        }

        bool woken = false;

        if (IRQ->expected_irq & (1 << EXIT_IRQ_n)) {
                semaphore_signal_from_ISR(IRQ->irqsem[EXIT_IRQ_n], &woken);
                CLEAR_BIT(IRQ->expected_irq, (1 << EXIT_IRQ_n));
        }

        WRITE_REG(EXTI->PR, EXTI_PR_PR0 << EXIT_IRQ_n);

        return woken;
}

//==============================================================================
/**
 * @brief EXTI0 IRQ Handler
 */
//==============================================================================
void EXTI0_IRQHandler(void)
{
        if (IRQ_handler(0)) {
                task_yield_from_ISR();
        }
}

//==============================================================================
/**
 * @brief EXTI1 IRQ Handler
 */
//==============================================================================
void EXTI1_IRQHandler(void)
{
        if (IRQ_handler(1)) {
                task_yield_from_ISR();
        }
}

//==============================================================================
/**
 * @brief EXTI2 IRQ Handler
 */
//==============================================================================
void EXTI2_IRQHandler(void)
{
        if (IRQ_handler(2)) {
                task_yield_from_ISR();
        }
}

//==============================================================================
/**
 * @brief EXTI3 IRQ Handler
 */
//==============================================================================
void EXTI3_IRQHandler(void)
{
        if (IRQ_handler(3)) {
                task_yield_from_ISR();
        }
}

//==============================================================================
/**
 * @brief EXTI4 IRQ Handler
 */
//==============================================================================
void EXTI4_IRQHandler(void)
{
        if (IRQ_handler(4)) {
                task_yield_from_ISR();
        }
}

//==============================================================================
/**
 * @brief EXTI5-9 IRQ Handler
 */
//==============================================================================
void EXTI9_5_IRQHandler(void)
{
        bool woken = false;

        for (uint i = 5; i <= 9; i++) {
                woken |= IRQ_handler(i);
        }

        if (woken) {
                task_yield_from_ISR();
        }
}

//==============================================================================
/**
 * @brief EXTI10-15 IRQ Handler
 */
//==============================================================================
void EXTI15_10_IRQHandler(void)
{
        bool woken = false;

        for (uint i = 10; i <= 15; i++) {
                woken |= IRQ_handler(i);
        }

        if (woken) {
                task_yield_from_ISR();
        }
}

/*==============================================================================
  End of file
==============================================================================*/
