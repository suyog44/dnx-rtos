/*=============================================================================================*//**
@file    usart.c

@author  Daniel Zorychta

@brief   This file support USART peripherals

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


*//*==============================================================================================*/

#ifdef __cplusplus
      extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "uart.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define PORT_FREE                   (u16_t)EMPTY_TASK

/** UART wake method: idle line (0) or address mark (1) */
#define SetAddressWakeMethod(enable)            \
      if (enable)                               \
            usartPtr->CR1 |= USART_CR1_WAKE;    \
      else                                      \
            usartPtr->CR1 &= ~USART_CR1_WAKE


/** parity enable (1) or disable (0) */
#define ParityCheckEnable(enable)               \
      if (enable)                               \
            usartPtr->CR1 |= USART_CR1_PCE;     \
      else                                      \
            usartPtr->CR1 &= ~USART_CR1_PCE


/** even parity (0) or odd parity (1) */
#define SetOddParity(enable)                    \
      if (enable)                               \
            usartPtr->CR1 |= USART_CR1_PS;      \
      else                                      \
            usartPtr->CR1 &= ~USART_CR1_PS


/** disable (0) or enable (1) UART transmitter */
#define TransmitterEnable(enable)               \
      if (enable)                               \
            usartPtr->CR1 |= USART_CR1_TE;      \
      else                                      \
            usartPtr->CR1 &= ~USART_CR1_TE


/** disable (0) or enable (1) UART receiver */
#define ReceiverEnable(enable)                  \
      if (enable)                               \
            usartPtr->CR1 |= USART_CR1_RE;      \
      else                                      \
            usartPtr->CR1 &= ~USART_CR1_RE


/** receiver wakeup: active mode (0) or mute mode (1) */
#define ReceiverWakeupMuteEnable(enable)        \
      if (enable)                               \
            usartPtr->CR1 |= USART_CR1_RWU;     \
      else                                      \
            usartPtr->CR1 &= ~USART_CR1_RWU


/** LIN mode disable (0) or enable (1) */
#define LINModeEnable(enable)                   \
      if (enable)                               \
            usartPtr->CR2 |= USART_CR2_LINEN;   \
      else                                      \
            usartPtr->CR2 &= ~USART_CR2_LINEN


/** 1 stop bit (0) or 2 stop bits (1) */
#define Set2StopBits(enable)                    \
      if (enable)                               \
      {                                         \
            usartPtr->CR2 &= USART_CR2_STOP;    \
            usartPtr->CR2 |= USART_CR2_STOP_1;  \
      }                                         \
      else                                      \
            usartPtr->CR2 &= USART_CR2_STOP


/** LIN break detector length: 10 bits (0) or 11 bits (1) */
#define LINBreakDet11Bits(enable)               \
      if (enable)                               \
            usartPtr->CR2 |= USART_CR2_LBDL;    \
      else                                      \
            usartPtr->CR2 &= ~USART_CR2_LBDL


/** address of the USART node (in the multiprocessor mode), 4-bit length */
#define SetAddressNode(adr)                     \
      usartPtr->CR2 &= ~USART_CR2_ADD;          \
      usartPtr->CR2 |= (adr & USART_CR2_ADD)    \


/** baud rate */
#define SetBaudRate(clk, baud)                  \
      usartPtr->BRR = (u16_t)((clk / baud) + 1)


/** CTS hardware flow control enable (1) or disable (0) */
#define CTSEnable(enable)                       \
      if (enable)                               \
            usartPtr->CR3 |= USART_CR3_CTSE;    \
      else                                      \
            usartPtr->CR3 &= ~USART_CR3_CTSE


/** RTS hardware flow control enable (1) or disable (0) */
#define RTSEnable(enable)                       \
      if (enable)                               \
            usartPtr->CR3 |= USART_CR3_RTSE;    \
      else                                      \
            usartPtr->CR3 &= ~USART_CR3_RTSE


/** enable UART */
#define UARTEnable()                            \
      usartPtr->CR1 |= USART_CR1_UE


/** disable UART */
#define UARTDisable()                           \
      usartPtr->CR1 &= ~UART_CR1_UE1


/** enable RX interrupt */
#define EnableRxIRQ()                           \
      usartPtr->CR1 |= USART_CR1_RXNEIE


/** enable RX interrupt */
#define DisableRxIRQ()                          \
      usartPtr->CR1 &= ~UART_CR1_RXNEIE


/** enable TXE interrupt */
#define EnableTXEIRQ()                          \
      usartPtr->CR1 |= USART_CR1_TXEIE


/** disable TXE interrupt */
#define DisableTXEIRQ()                         \
      usartPtr->CR1 &= ~USART_CR1_TXEIE


/** write data into RX FIFO buffer */
#define WriteRxFIFO(FIFO, data)                       \
      if (FIFO->Level < UART_RX_BUFFER_SIZE)          \
      {                                               \
            FIFO->Buffer[FIFO->TxIdx++] = data;       \
                                                      \
            if (FIFO->TxIdx >= UART_RX_BUFFER_SIZE)   \
                  FIFO->TxIdx = 0;                    \
                                                      \
            FIFO->Level++;                            \
      }



/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
/** Rx FIFO type */
typedef struct RxFIFO_struct
{
      u8_t  *Buffer;
      u16_t Level;
      u16_t RxIdx;
      u16_t TxIdx;
} RxFIFO_t;


/** type which contains tx buffer address and size */
typedef struct TxBuffer_struct
{
      u8_t   *TxSrcPtr;
      size_t Size;
} TxBuffer_t;


/** type which contain port information */
typedef struct PortHandle_struct
{
      USART_t *Address;             /* peripheral address */
      RxFIFO_t      RxFIFO;               /* Rx FIFO for IRQ */
      TxBuffer_t    TxBuffer;             /* Tx Buffer for IRQ */
      xTaskHandle   TaskHandle;           /* task handle variable for IRQ */
      u16_t         Lock;                 /* port reservation */
} PortHandle_t;


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
/** port localizations */
static PortHandle_t PortHandle[] =
{
      #ifdef RCC_APB2ENR_USART1EN
      #if (UART_1_ENABLE > 0)
      {
            .Address    = USART1,
            .RxFIFO     = {0},
            .TxBuffer   = {0},
            .TaskHandle = NULL,
            . Lock      = PORT_FREE
      },
      #endif
      #endif

      #ifdef RCC_APB1ENR_USART2EN
      #if (UART_2_ENABLE > 0)
      {
            .Address    = USART2,
            .RxFIFO     = {0},
            .TxBuffer   = {0},
            .TaskHandle = NULL,
            . Lock      = PORT_FREE
      },
      #endif
      #endif

      #ifdef RCC_APB1ENR_USART3EN
      #if (UART_3_ENABLE > 0)
      {
            .Address    = USART3,
            .RxFIFO     = {0},
            .TxBuffer   = {0},
            .TaskHandle = NULL,
            . Lock      = PORT_FREE
      },
      #endif
      #endif

      #ifdef RCC_APB1ENR_UART4EN
      #if (UART_4_ENABLE > 0)
      {
            .Address    = UART4,
            .RxFIFO     = {0},
            .TxBuffer   = {0},
            .TaskHandle = NULL,
            . Lock      = PORT_FREE
      },
      #endif
      #endif

      #ifdef RCC_APB1ENR_UART5EN
      #if (UART_5_ENABLE > 0)
      {
            .Address    = UART5,
            .RxFIFO     = {0},
            .TxBuffer   = {0},
            .TaskHandle = NULL,
            . Lock      = PORT_FREE
      },
      #endif
      #endif
};


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Initialize USART devices. Not used, initialization will done at port open
 */
//================================================================================================//
stdStatus_t UART_Init(void)
{
      return STD_STATUS_OK;
}


//================================================================================================//
/**
 * @brief Opens specified port and initialize default settings
 *
 * @param[in]  usartName                  USART name (number)
 */
//================================================================================================//
stdStatus_t UART_Open(dev_t usartName)
{
      stdStatus_t status    = UART_STATUS_PORTNOTEXIST;
      USART_t     *usartPtr = NULL;

      /* check port range */
      if ((unsigned)usartName < UART_DEV_LAST)
      {
            /* lock task switching */
            TaskSuspendAll();

            /* check that port is free */
            if (PortHandle[usartName].Lock == PORT_FREE)
            {
                  /* registered port for current task */
                  PortHandle[usartName].Lock = TaskGetPID();
                  TaskResumeAll();

                  /* set task handle for IRQs */
                  PortHandle[usartName].TaskHandle = TaskGetCurrentTaskHandle();

                  /* enable UART clock */
                  switch (usartName)
                  {
                        #ifdef RCC_APB2ENR_USART1EN
                        #if (UART_1_ENABLE > 0)
                        case UART_DEV_1:
                              RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_USART2EN
                        #if (UART_2_ENABLE > 0)
                        case UART_DEV_2:
                              RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_USART3EN
                        #if (UART_3_ENABLE > 0)
                        case UART_DEV_3:
                              RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_UART4EN
                        #if (UART_4_ENABLE > 0)
                        case UART_DEV_4:
                              RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_UART5EN
                        #if (UART_5_ENABLE > 0)
                        case UART_DEV_5:
                              RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
                              break;
                        #endif
                        #endif

                        default:
                              break;
                  }

                  /* set port address */
                  usartPtr = PortHandle[usartName].Address;

                  /* default settings */
                  if ((u32_t)usartPtr == USART1_BASE)
                        SetBaudRate(UART_PCLK2_FREQ, UART_DEFAULT_BAUDRATE);
                  else
                        SetBaudRate(UART_PCLK1_FREQ, UART_DEFAULT_BAUDRATE);

                  SetAddressWakeMethod(UART_DEFAULT_WAKE_METHOD);

                  ParityCheckEnable(UART_DEFAULT_PARITY_ENABLE);

                  SetOddParity(UART_DEFAULT_PARITY_SELECTION);

                  TransmitterEnable(UART_DEFAULT_TX_ENABLE);

                  ReceiverEnable(UART_DEFAULT_RX_ENABLE);

                  ReceiverWakeupMuteEnable(UART_DEFAULT_RX_WAKEUP_MODE);

                  LINModeEnable(UART_DEFAULT_LIN_ENABLE);

                  Set2StopBits(UART_DEFAULT_STOP_BITS);

                  LINBreakDet11Bits(UART_DEFAULT_LIN_BREAK_LEN_DET);

                  SetAddressNode(UART_DEFAULT_MULTICOM_ADDRESS);

                  CTSEnable(UART_DEFAULT_CTS_ENABLE);

                  RTSEnable(UART_DEFAULT_RTS_ENABLE);

                  UARTEnable();

                  /* allocate memory for RX buffer */
                  PortHandle[usartName].RxFIFO.Buffer = (u8_t*)Malloc(UART_RX_BUFFER_SIZE);

                  if (PortHandle[usartName].RxFIFO.Buffer == NULL)
                  {
                        status = UART_STATUS_NOFREEMEM;
                        goto UART_Open_End;
                  }

                  /* enable interrupts */
                  switch (usartName)
                  {
                        #ifdef RCC_APB2ENR_USART1EN
                        #if (UART_1_ENABLE > 0)
                        case UART_DEV_1: NVIC_EnableIRQ(USART1_IRQn); break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_USART2EN
                        #if (UART_2_ENABLE > 0)
                        case UART_DEV_2: NVIC_EnableIRQ(USART2_IRQn); break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_USART3EN
                        #if (UART_3_ENABLE > 0)
                        case UART_DEV_3: NVIC_EnableIRQ(USART3_IRQn); break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_UART4EN
                        #if (UART_4_ENABLE > 0)
                        case UART_DEV_4: NVIC_EnableIRQ(UART4_IRQn); break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_UART5EN
                        #if (UART_5_ENABLE > 0)
                        case UART_DEV_5: NVIC_EnableIRQ(UART5_IRQn); break;
                        #endif
                        #endif

                        default:
                              break;
                  }

                  EnableRxIRQ();

                  status = STD_STATUS_OK;
            }
            else
            {
                  TaskResumeAll();

                  if (PortHandle[usartName].Lock == TaskGetPID())
                        status = STD_STATUS_OK;
                  else
                        status = UART_STATUS_PORTLOCKED;
            }
      }

UART_Open_End:
      return status;
}


//================================================================================================//
/**
 * @brief Function close opened port
 *
 * @param[in]  usartName                  USART name (number)
 *
 * @retval STD_STATUS_OK                  operation success
 * @retval UART_STATUS_PORTLOCKED         port locked for other task
 * @retval UART_STATUS_PORTNOTEXIST       port number does not exist
 */
//================================================================================================//
stdStatus_t UART_Close(dev_t usartName)
{
      stdStatus_t status = UART_STATUS_PORTNOTEXIST;

      /* check port range */
      if (usartName <= UART_DEV_LAST)
      {
            /* check that port is reserved for this task */
            if (PortHandle[usartName].Lock == TaskGetPID())
            {
                  /* turn off device */
                  switch (usartName)
                  {
                        #ifdef RCC_APB2ENR_USART1EN
                        #if (UART_1_ENABLE > 0)
                        case UART_DEV_1:
                              NVIC_DisableIRQ(USART1_IRQn);
                              RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
                              RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;
                              RCC->APB2ENR  &= ~RCC_APB2ENR_USART1EN;
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_USART2EN
                        #if (UART_2_ENABLE > 0)
                        case UART_DEV_2:
                              NVIC_DisableIRQ(USART2_IRQn);
                              RCC->APB1RSTR |= RCC_APB1RSTR_USART2RST;
                              RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;
                              RCC->APB1ENR  &= ~RCC_APB1ENR_USART2EN;
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_USART3EN
                        #if (UART_3_ENABLE > 0)
                        case UART_DEV_3:
                              NVIC_DisableIRQ(USART3_IRQn);
                              RCC->APB1RSTR |= RCC_APB1RSTR_USART3RST;
                              RCC->APB1RSTR &= ~RCC_APB1RSTR_USART3RST;
                              RCC->APB1ENR  &= ~RCC_APB1ENR_USART3EN;
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_UART4EN
                        #if (UART_4_ENABLE > 0)
                        case UART_DEV_4:
                              NVIC_DisableIRQ(UART4_IRQn);
                              RCC->APB1RSTR |= RCC_APB1RSTR_UART4RST;
                              RCC->APB1RSTR &= ~RCC_APB1RSTR_UART4RST;
                              RCC->APB1ENR  &= ~RCC_APB1ENR_UART4EN;
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_UART5EN
                        #if (UART_5_ENABLE > 0)
                        case UART_DEV_5:
                              NVIC_DisableIRQ(UART5_IRQn);
                              RCC->APB1RSTR |= RCC_APB1RSTR_UART5RST;
                              RCC->APB1RSTR &= ~RCC_APB1RSTR_UART5RST;
                              RCC->APB1ENR  &= ~RCC_APB1ENR_UART5EN;
                              break;
                        #endif
                        #endif

                        default:
                              break;
                  }

                  /* free used memory for buffer */
                  Free(PortHandle[usartName].RxFIFO.Buffer);
                  PortHandle[usartName].RxFIFO.Buffer = NULL;
                  PortHandle[usartName].RxFIFO.Level  = 0;
                  PortHandle[usartName].RxFIFO.RxIdx  = 0;
                  PortHandle[usartName].RxFIFO.TxIdx  = 0;

                  /* unlock device */
                  PortHandle[usartName].Lock = PORT_FREE;

                  /* delete from task handle */
                  PortHandle[usartName].TaskHandle = NULL;

                  /* delete tx buffer */
                  PortHandle[usartName].TxBuffer.TxSrcPtr = NULL;

                  status = STD_STATUS_OK;
            }
            else
            {
                  status = UART_STATUS_PORTLOCKED;
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Write data to UART (ISR or DMA)
 *
 * @param[in]  usartName                  USART name (number)
 * @param[in]  *src                       source buffer
 * @param[in]  size                       buffer size
 * @param[in]  seek                       seek
 *
 * @retval STD_STATUS_OK                  operation success
 * @retval UART_STATUS_PORTLOCKED         port locked for other task
 * @retval UART_STATUS_PORTNOTEXIST       port number does not exist
 * @retval UART_STATUS_INCORRECTSIZE      incorrect size
 */
//================================================================================================//
stdStatus_t UART_Write(dev_t usartName, void *src, size_t size, size_t seek)
{
      (void)seek;

      stdStatus_t status    = UART_STATUS_PORTNOTEXIST;
      USART_t     *usartPtr = NULL;

      /* check port range */
      if (usartName <= UART_DEV_LAST)
      {
            /* check that port is reserved for this task */
            if (PortHandle[usartName].Lock == TaskGetPID())
            {
                  /* load data from FIFO */
                  if (size)
                  {
                        bool_t DMAEnabled = FALSE;

                        switch (usartName)
                        {
                              #ifdef RCC_APB2ENR_USART1EN
                              #if (UART_1_ENABLE > 0)
                              case UART_DEV_1: DMAEnabled = UART_1_DMA_TX_ENABLE; break;
                              #endif
                              #endif

                              #ifdef RCC_APB1ENR_USART2EN
                              #if (UART_2_ENABLE > 0)
                              case UART_DEV_2: DMAEnabled = UART_2_DMA_TX_ENABLE; break;
                              #endif
                              #endif

                              #ifdef RCC_APB1ENR_USART3EN
                              #if (UART_3_ENABLE > 0)
                              case UART_DEV_3: DMAEnabled = UART_3_DMA_TX_ENABLE; break;
                              #endif
                              #endif

                              #ifdef RCC_APB1ENR_UART4EN
                              #if (UART_4_ENABLE > 0)
                              case UART_DEV_4: DMAEnabled = UART_4_DMA_TX_ENABLE; break;
                              #endif
                              #endif

                              #ifdef RCC_APB1ENR_UART5EN
                              #if (UART_5_ENABLE > 0)
                              case UART_DEV_5: DMAEnabled = UART_5_DMA_TX_ENABLE; break;
                              #endif
                              #endif

                              default:
                                    break;
                        }

                        if (DMAEnabled)
                        {
                              /* TODO */
                        }
                        else
                        {
                              /* set buffer address and size */
                              PortHandle[usartName].TxBuffer.TxSrcPtr = (u8_t*)src;
                              PortHandle[usartName].TxBuffer.Size     = size;

                              /* set port address */
                              usartPtr = PortHandle[usartName].Address;

                              /* enable TXE interrupt */
                              EnableTXEIRQ();

                              TaskSuspend(THIS_TASK);

                              status = STD_STATUS_OK;
                        }
                  }
                  else
                  {
                        status = UART_STATUS_INCORRECTSIZE;
                  }
            }
            else
            {
                  status = UART_STATUS_PORTLOCKED;
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Read data from UART Rx buffer
 *
 * @param[in]  usartName                  USART name (number)
 * @param[out] *dst                       destination buffer
 * @param[in]  size                       buffer size
 * @param[in]  seek                       seek
 *
 * @retval STD_STATUS_OK                  operation success
 * @retval UART_STATUS_PORTLOCKED         port locked for other task
 * @retval UART_STATUS_PORTNOTEXIST       port number does not exist
 * @retval UART_STATUS_INCORRECTSIZE      incorrect size
 */
//================================================================================================//
stdStatus_t UART_Read(dev_t usartName, void *dst, size_t size, size_t seek)
{
      (void)seek;

      stdStatus_t status = UART_STATUS_PORTNOTEXIST;

      /* check port range */
      if (usartName <= UART_DEV_LAST)
      {
            /* check that port is reserved for this task */
            if (PortHandle[usartName].Lock == TaskGetPID())
            {
                  /* load data from FIFO */
                  if (size)
                  {
                        RxFIFO_t *RxFIFO = &PortHandle[usartName].RxFIFO;
                        u8_t     *dstPtr = (u8_t*)dst;

                        do
                        {
                              TaskEnterCritical();

                              if (RxFIFO->Level > 0)
                              {
                                    *dstPtr = RxFIFO->Buffer[RxFIFO->RxIdx++];

                                    if (RxFIFO->RxIdx >= UART_RX_BUFFER_SIZE)
                                          RxFIFO->RxIdx = 0;

                                    RxFIFO->Level--;

                                    size--;

                                    TaskExitCritical();
                              }
                              else
                              {
                                    TaskExitCritical();
                                    TaskSuspend(THIS_TASK);
                              }
                        }
                        while (size);

                        status = STD_STATUS_OK;
                  }
                  else
                  {
                        status = UART_STATUS_INCORRECTSIZE;
                  }
            }
            else
            {
                  status = UART_STATUS_PORTLOCKED;
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief
 */
//================================================================================================//
stdStatus_t UART_IOCtl(dev_t usartName, IORq_t ioRQ, void *data)
{
      return STD_STATUS_ERROR;
}


//================================================================================================//
/**
 * @brief USART1 Interrupt
 */
//================================================================================================//
#ifdef RCC_APB2ENR_USART1EN
#if (UART_1_ENABLE > 0)
void USART1_IRQHandler(void)
{
      if (USART1->SR & USART_SR_RXNE)
      {
            RxFIFO_t *RxFIFO = &PortHandle[UART_DEV_1].RxFIFO;

            if (RxFIFO->Buffer)
            {
                  WriteRxFIFO(RxFIFO, USART1->DR);

                  if (PortHandle[UART_DEV_1].TaskHandle)
                        TaskResumeFromISR(PortHandle[UART_DEV_1].TaskHandle);
            }
      }

      if ((USART1->SR & USART_SR_TXE) && PortHandle[UART_DEV_1].TxBuffer.Size)
      {
            USART1->DR = *(PortHandle[UART_DEV_1].TxBuffer.TxSrcPtr++);
            PortHandle[UART_DEV_1].TxBuffer.Size--;

            if (PortHandle[UART_DEV_1].TxBuffer.Size == 0)
            {
                  USART_t *usartPtr = USART1;

                  DisableTXEIRQ();

                  if (PortHandle[UART_DEV_1].TaskHandle)
                        TaskResumeFromISR(PortHandle[UART_DEV_1].TaskHandle);
            }
      }
}
#endif
#endif


//================================================================================================//
/**
 * @brief USART2 Interrupt
 */
//================================================================================================//
#ifdef RCC_APB1ENR_USART2EN
#if (UART_2_ENABLE > 0)
void USART2_IRQHandler(void)
{
      if (USART2->SR & USART_SR_RXNE)
      {
            RxFIFO_t *RxFIFO = &PortHandle[UART_DEV_2].RxFIFO;

            if (RxFIFO->Buffer)
            {
                  WriteRxFIFO(RxFIFO, USART2->DR);

                  if (PortHandle[UART_DEV_2].TaskHandle)
                        TaskResumeFromISR(PortHandle[UART_DEV_2].TaskHandle);
            }
      }

      if (USART2->SR & USART_SR_TXE)
      {
            USART2->DR = *(PortHandle[UART_DEV_2].TxBuffer.TxSrcPtr++);
            PortHandle[UART_DEV_2].TxBuffer.Size--;

            if (PortHandle[UART_DEV_2].TxBuffer.Size == 0)
            {
                  USART_t *usartPtr = USART2;

                  DisableTXEIRQ();

                  if (PortHandle[UART_DEV_2].TaskHandle)
                        TaskResumeFromISR(PortHandle[UART_DEV_2].TaskHandle);
            }
      }
}
#endif
#endif


//================================================================================================//
/**
 * @brief USART3 Interrupt
 */
//================================================================================================//
#ifdef RCC_APB1ENR_USART3EN
#if (UART_3_ENABLE > 0)
void USART3_IRQHandler(void)
{
      if (USART3->SR & USART_SR_RXNE)
      {
            RxFIFO_t *RxFIFO = &PortHandle[UART_DEV_3].RxFIFO;

            if (RxFIFO->Buffer)
            {
                  WriteRxFIFO(RxFIFO, USART3->DR);

                  if (PortHandle[UART_DEV_3].TaskHandle)
                        TaskResumeFromISR(PortHandle[UART_DEV_3].TaskHandle);
            }
      }


      if (USART3->SR & USART_SR_TXE)
      {
            USART3->DR = *(PortHandle[UART_DEV_3].TxBuffer.TxSrcPtr++);
            PortHandle[UART_DEV_3].TxBuffer.Size--;

            if (PortHandle[UART_DEV_3].TxBuffer.Size == 0)
            {
                  USART_t *usartPtr = USART3;

                  DisableTXEIRQ();

                  if (PortHandle[UART_DEV_3].TaskHandle)
                        TaskResumeFromISR(PortHandle[UART_DEV_3].TaskHandle);
            }
      }
}
#endif
#endif


//================================================================================================//
/**
 * @brief UART4 Interrupt
 */
//================================================================================================//
#ifdef RCC_APB1ENR_UART4EN
#if (UART_4_ENABLE > 0)
void UART4_IRQHandler(void)
{
      if (UART4->SR & USART_SR_RXNE)
      {
            RxFIFO_t *RxFIFO = &PortHandle[UART_DEV_4].RxFIFO;

            if (RxFIFO->Buffer)
            {
                  WriteRxFIFO(RxFIFO, UART4->DR);

                  if (PortHandle[UART_DEV_4].TaskHandle)
                        TaskResumeFromISR(PortHandle[UART_DEV_4].TaskHandle);
            }
      }

      if (UART4->SR & USART_SR_TXE)
      {
            UART4->DR = *(PortHandle[UART_DEV_4].TxBuffer.TxSrcPtr++);
            PortHandle[UART_DEV_4].TxBuffer.Size--;

            if (PortHandle[UART_DEV_4].TxBuffer.Size == 0)
            {
                  USART_t *usartPtr = UART4;

                  DisableTXEIRQ();

                  if (PortHandle[UART_DEV_4].TaskHandle)
                        TaskResumeFromISR(PortHandle[UART_DEV_4].TaskHandle);
            }
      }
}
#endif
#endif


//================================================================================================//
/**
 * @brief UART5 Interrupt
 */
//================================================================================================//
#ifdef RCC_APB1ENR_UART5EN
#if (UART_5_ENABLE > 0)
void UART5_IRQHandler(void)
{
      if (UART5->SR & USART_SR_RXNE)
      {
            RxFIFO_t *RxFIFO = &PortHandle[UART_DEV_5].RxFIFO;

            if (RxFIFO->Buffer)
            {
                  WriteRxFIFO(RxFIFO, UART5->DR);

                  if (PortHandle[UART_DEV_5].TaskHandle)
                        TaskResumeFromISR(PortHandle[UART_DEV_5].TaskHandle);
            }
      }

      if (UART5->SR & USART_SR_TXE)
      {
            UART5->DR = *(PortHandle[UART_DEV_5].TxBuffer.TxSrcPtr++);
            PortHandle[UART_DEV_5].TxBuffer.Size--;

            if (PortHandle[UART_DEV_5].TxBuffer.Size == 0)
            {
                  USART_t *usartPtr = UART5;

                  DisableTXEIRQ();

                  if (PortHandle[UART_DEV_5].TaskHandle)
                        TaskResumeFromISR(PortHandle[UART_DEV_5].TaskHandle);
            }
      }
}
#endif
#endif


#ifdef __cplusplus
   }
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
