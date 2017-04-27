/*=========================================================================*//**
@file    clk_flags.h

@author  Daniel Zorychta

@brief   PLL module configuration flags.

@note    Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as suffix
 *       and prefix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */
#ifndef _CLK_FLAGS_H_
#define _CLK_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 3, "Home > Microcontroller > PLL",
               function() this:LoadFile("arch/arch_flags.h") end)

this:SetEvent("PostDiscard", function() this:CalculateFreq() end)
++*/

//==============================================================================
// Oscillators Configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_OSCILLATORS", "Oscillators", -1, "bold")
this:AddExtraWidget("Void", "VoidOsc1")
this:AddExtraWidget("Void", "VoidOsc2")
++*/
/*--
this:AddWidget("Combobox", "LSI oscillator")
this:AddItem("Enable", "ENABLE")
this:AddItem("Disable", "DISABLE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_LSI_ON", "")
--*/
#define __CLK_LSI_ON__ DISABLE

/*--
this:AddWidget("Combobox", "LSE oscillator")
this:AddItem("Disable", "RCC_LSE_OFF")
this:AddItem("Enable", "RCC_LSE_ON")
this:AddItem("Baypass", "RCC_LSE_Bypass")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_LSE_ON", "")
--*/
#define __CLK_LSE_ON__ RCC_LSE_OFF

/*--
this:AddWidget("Combobox", "HSE oscillator")
this:AddItem("Disable", "RCC_HSE_OFF")
this:AddItem("Enable", "RCC_HSE_ON")
this:AddItem("Baypass", "RCC_HSE_Bypass")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_HSE_ON", "")
--*/
#define __CLK_HSE_ON__ RCC_HSE_ON

//==============================================================================
// PLL Configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_PLL", "\nMain PLL Configuration", -1, "bold")
this:AddExtraWidget("Void", "VoidPLL1")
this:AddExtraWidget("Void", "VoidPLL2")
++*/
/*--
this:AddWidget("Combobox", "PLL")
this:AddItem("Enable",  "ENABLE")
this:AddItem("Disable", "DISABLE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_ON", "")
--*/
#define __CLK_PLL_ON__ ENABLE

/*--
this:AddWidget("Combobox", "Clock source")
this:AddItem("HSI",  "RCC_PLLSource_HSI")
this:AddItem("HSE", "RCC_PLLSource_HSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_SRC", "")
--*/
#define __CLK_PLL_SRC__ RCC_PLLSource_HSE

/*--
this:AddWidget("Spinbox", 2, 63, "Clock divider (PLLM)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_M", "")
--*/
#define __CLK_PLL_M__ 4

/*--
this:AddWidget("Spinbox", 50, 432, "Clock multiplier (PLLN)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_N", "")
--*/
#define __CLK_PLL_N__ 168

/*--
this:AddWidget("Combobox", "Main clock divider (PLLP)")
this:AddItem("VCO divided by 2",  "2")
this:AddItem("VCO divided by 4",  "4")
this:AddItem("VCO divided by 6",  "6")
this:AddItem("VCO divided by 8",  "8")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_P", "")
--*/
#define __CLK_PLL_P__ 2

/*--
this:AddWidget("Spinbox", 2, 15, "USB, SDIO clock divider (PLLQ)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_Q", "")
--*/
#define __CLK_PLL_Q__ 7

/*--
this:AddWidget("Spinbox", 2, 7, "I2S, SAI clock divider (PLLR)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_R", "")
--*/
#define __CLK_PLL_R__ 7

//==============================================================================
// PLLI2S Configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_PLLI2S", "\nI2S PLL Configuration", -1, "bold")
this:AddExtraWidget("Void", "VoidPLLI2S1")
this:AddExtraWidget("Void", "VoidPLLI2S2")
++*/
/*--
this:AddWidget("Combobox", "PLLI2S")
this:AddItem("Enable",  "ENABLE")
this:AddItem("Disable", "DISABLE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLI2S_ON", "")
--*/
#define __CLK_PLLI2S_ON__ ENABLE

/*--
this:AddWidget("Spinbox", 2, 63, "Clock divider (PLLM)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLI2S_M", "")
--*/
#define __CLK_PLLI2S_M__ 4

/*--
this:AddWidget("Spinbox", 50, 432, "Clock multiplier (PLLN)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLI2S_N", "")
--*/
#define __CLK_PLLI2S_N__ 192

/*--
this:AddWidget("Combobox", "48MHz clock divider (PLLP)")
this:AddItem("VCO divided by 2",  "2")
this:AddItem("VCO divided by 4",  "4")
this:AddItem("VCO divided by 6",  "6")
this:AddItem("VCO divided by 8",  "8")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLI2S_P", "")
--*/
#define __CLK_PLLI2S_P__ 2

/*--
this:AddWidget("Spinbox", 2, 15, "SAI1 clock divider (PLLQ)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLI2S_Q", "")
--*/
#define __CLK_PLLI2S_Q__ 2

/*--
this:AddWidget("Spinbox", 2, 7, "I2S clock divider (PLLR)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLI2S_R", "")
--*/
#define __CLK_PLLI2S_R__ 2


//==============================================================================
// PLLSAI Configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_PLLSAI", "\nSAI PLL Configuration", -1, "bold")
this:AddExtraWidget("Void", "VoidPLLSAI1")
this:AddExtraWidget("Void", "VoidPLLSAI2")
++*/
/*--
this:AddWidget("Combobox", "PLLSAI")
this:AddItem("Enable",  "ENABLE")
this:AddItem("Disable", "DISABLE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLSAI_ON", "")
--*/
#define __CLK_PLLSAI_ON__ ENABLE

/*--
this:AddWidget("Spinbox", 2, 63, "Clock divider (PLLM)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLSAI_M", "")
--*/
#define __CLK_PLLSAI_M__ 4

/*--
this:AddWidget("Spinbox", 50, 432, "Clock multiplier (PLLN)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLSAI_N", "")
--*/
#define __CLK_PLLSAI_N__ 192

/*--
this:AddWidget("Combobox", "48MHz clock divider (PLLP)")
this:AddItem("VCO divided by 2",  "2")
this:AddItem("VCO divided by 4",  "4")
this:AddItem("VCO divided by 6",  "6")
this:AddItem("VCO divided by 8",  "8")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLSAI_P", "")
--*/
#define __CLK_PLLSAI_P__ 2

/*--
this:AddWidget("Spinbox", 2, 15, "SAI1 clock divider (PLLQ)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLSAI_Q", "")
--*/
#define __CLK_PLLSAI_Q__ 2

/*--
this:AddWidget("Spinbox", 2, 7, "LTDC clock divider (PLLR)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLSAI_R", "")
--*/
#define __CLK_PLLSAI_R__ 2

//==============================================================================
// System Clock Sources Configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_ClkSrc", "\nClock sources", -1, "bold")
this:AddExtraWidget("Void", "VoidClkSrc1")
this:AddExtraWidget("Void", "VoidClkSrc2")
++*/
/*--
this:AddWidget("Combobox", "System clock source")
this:AddItem("HSI clock", "RCC_SYSCLKSource_HSI")
this:AddItem("HSE clock", "RCC_SYSCLKSource_HSE")
this:AddItem("PLL clock", "RCC_SYSCLKSource_PLLCLK")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_SYSCLK_SRC", "")
--*/
#define __CLK_SYS_CLK_SRC__ RCC_SYSCLKSource_PLLCLK

/*--
this:AddWidget("Combobox", "RTC Clock source")
this:AddItem("LSE clock", "RCC_RTCCLKSource_LSE")
this:AddItem("LSI clock", "RCC_RTCCLKSource_LSI")
for i = 2, 31 do this:AddItem("HSE clock divided by "..i, "RCC_RTCCLKSource_HSE_Div"..i) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_RTCCLK_SRC", "")
--*/
#define __CLK_RTC_CLK_SRC__ RCC_RTCCLKSource_LSE

/*--
this:AddWidget("Combobox", "MCO1 Clock source")
this:AddItem("HSI", "RCC_MCO1Source_HSI")
this:AddItem("HSE", "RCC_MCO1Source_HSE")
this:AddItem("LSE", "RCC_MCO1Source_LSE")
this:AddItem("PLLCLK", "RCC_MCO1Source_PLLCLK")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_MCO1_SRC", "")
--*/
#define __CLK_MC01_CLK_SRC__ RCC_MCO1Source_HSI

/*--
this:AddWidget("Combobox", "MCO1 Clock divisor (output)")
for i = 1, 5 do this:AddItem("MCO divided by "..i, tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_MCO1_DIV", "")
--*/
#define __CLK_MC01_CLK_DIV__ 1

/*--
this:AddWidget("Combobox", "MCO2 Clock source")
this:AddItem("SYSCLK", "RCC_MCO2Source_SYSCLK")
this:AddItem("PLLI2SCLK", "RCC_MCO2Source_PLLI2SCLK")
this:AddItem("HSE", "RCC_MCO2Source_HSE")
this:AddItem("PLLCLK", "RCC_MCO2Source_PLLCLK")
if uC.NAME:match("STM32F410") then this:AddItem("I2SCLK", "RCC_MCO2Source_I2SCLK") end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_MCO2_SRC", "")
--*/
#define __CLK_MC02_CLK_SRC__ RCC_MCO2Source_HSE

/*--
this:AddWidget("Combobox", "MCO2 Clock divisor (output)")
for i = 1, 5 do this:AddItem("MCO divided by "..i, tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_MCO2_DIV", "")
--*/
#define __CLK_MC02_CLK_DIV__ 1


//==============================================================================
// System Clocks Prescallers Configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_SysPre", "\nSystem prescalers", -1, "bold")
this:AddExtraWidget("Void", "VoidSysPre1")
this:AddExtraWidget("Void", "VoidSysPre2")
++*/
/*--
this:AddWidget("Combobox", "AHB prescaler")
this:AddItem("SYSCLK / 1",  "RCC_SYSCLK_Div1")
this:AddItem("SYSCLK / 2",  "RCC_SYSCLK_Div2")
this:AddItem("SYSCLK / 4",  "RCC_SYSCLK_Div4")
this:AddItem("SYSCLK / 8",  "RCC_SYSCLK_Div8")
this:AddItem("SYSCLK / 16",  "RCC_SYSCLK_Div16")
this:AddItem("SYSCLK / 64",  "RCC_SYSCLK_Div64")
this:AddItem("SYSCLK / 128",  "RCC_SYSCLK_Div128")
this:AddItem("SYSCLK / 256",  "RCC_SYSCLK_Div256")
this:AddItem("SYSCLK / 512",  "RCC_SYSCLK_Div512")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_AHB_PRE", "")
--*/
#define __CLK_AHB_PRE__ RCC_SYSCLK_Div1

/*--
this:AddWidget("Combobox", "APB1 prescaler")
this:AddItem("HCLK / 1",  "RCC_HCLK_Div1")
this:AddItem("HCLK / 2",  "RCC_HCLK_Div2")
this:AddItem("HCLK / 4",  "RCC_HCLK_Div4")
this:AddItem("HCLK / 8",  "RCC_HCLK_Div8")
this:AddItem("HCLK / 16",  "RCC_HCLK_Div16")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_APB1_PRE", "")
--*/
#define __CLK_APB1_PRE__ RCC_HCLK_Div2

/*--
this:AddWidget("Combobox", "APB2 prescaler")
this:AddItem("HCLK / 1",  "RCC_HCLK_Div1")
this:AddItem("HCLK / 2",  "RCC_HCLK_Div2")
this:AddItem("HCLK / 4",  "RCC_HCLK_Div4")
this:AddItem("HCLK / 8",  "RCC_HCLK_Div8")
this:AddItem("HCLK / 16",  "RCC_HCLK_Div16")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_APB2_PRE", "")
--*/
#define __CLK_APB2_PRE__ RCC_HCLK_Div2


//==============================================================================
// Flash Latency Configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_FlashLatency", "\nFlash latency", -1, "bold")
this:AddExtraWidget("Void", "VoidFL1")
this:AddExtraWidget("Void", "VoidFL2")
++*/
/*--
this:AddWidget("Combobox", "CPU voltage")
this:AddItem("2.7V - 3.6V",  "3p6V")
this:AddItem("2.4V - 2.7V",  "2p7V")
this:AddItem("2.1V - 2.4V",  "2p4V")
this:AddItem("1.8V - 2.1V",  "2p1V")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:SetToolTip("Select power voltage level to calculate FLASH latency.")
this:AddExtraWidget("Label", "LABEL_FLASH_LATENCY", "")
--*/
#define __CLK_CPU_VOLTAGE__ 3p6V

/*--
-- Value calculated automatically
this:AddWidget("Value")
--*/
#define __CLK_FLASH_LATENCY__ 5


//==============================================================================
// Calculation script
//==============================================================================
/*--
this.CalculateFreq = function(self)
    local PLLON      = iff(this:GetFlagValue("__CLK_PLL_ON__") == "ENABLE", 1, 0)
    local PLLSRC     = this:GetFlagValue("__CLK_PLL_SRC__")
    local PLLM       = this:GetFlagValue("__CLK_PLL_M__")
    local PLLN       = this:GetFlagValue("__CLK_PLL_N__")
    local PLLP       = this:GetFlagValue("__CLK_PLL_P__")
    local PLLQ       = this:GetFlagValue("__CLK_PLL_Q__")
    local PLLR       = this:GetFlagValue("__CLK_PLL_R__")

    local PLLI2SON   = iff(this:GetFlagValue("__CLK_PLLI2S_ON__") == "ENABLE", 1, 0)
    local PLLI2SSRC  = this:GetFlagValue("__CLK_PLLI2S_SRC__")
    local PLLI2SM    = this:GetFlagValue("__CLK_PLLI2S_M__")
    local PLLI2SN    = this:GetFlagValue("__CLK_PLLI2S_N__")
    local PLLI2SP    = this:GetFlagValue("__CLK_PLLI2S_P__")
    local PLLI2SQ    = this:GetFlagValue("__CLK_PLLI2S_Q__")
    local PLLI2SR    = this:GetFlagValue("__CLK_PLLI2S_R__")

    local PLLSAION   = iff(this:GetFlagValue("__CLK_PLLSAI_ON__") == "ENABLE", 1, 0)
    local PLLSAISRC  = this:GetFlagValue("__CLK_PLLSAI_SRC__")
    local PLLSAIM    = this:GetFlagValue("__CLK_PLLSAI_M__")
    local PLLSAIN    = this:GetFlagValue("__CLK_PLLSAI_N__")
    local PLLSAIP    = this:GetFlagValue("__CLK_PLLSAI_P__")
    local PLLSAIQ    = this:GetFlagValue("__CLK_PLLSAI_Q__")
    local PLLSAIR    = this:GetFlagValue("__CLK_PLLSAI_R__")

    local HSEON      = iff(this:GetFlagValue("__CLK_HSE_ON__") == "RCC_HSE_OFF", 0, 1)
    local LSEON      = iff(this:GetFlagValue("__CLK_LSE_ON__") == "RCC_LSE_OFF", 0, 1)
    local LSION      = iff(this:GetFlagValue("__CLK_LSI_ON__") == "ENABLE", 1, 0)

    local SYSCLKSRC  = this:GetFlagValue("__CLK_SYS_CLK_SRC__")
    local RTCSRC     = this:GetFlagValue("__CLK_RTC_CLK_SRC__")

    local AHBPRE     = this:GetFlagValue("__CLK_AHB_PRE__")
    local APB1PRE    = this:GetFlagValue("__CLK_APB1_PRE__")
    local APB2PRE    = this:GetFlagValue("__CLK_APB2_PRE__")

    local MCO1SRC    = this:GetFlagValue("__CLK_MC01_CLK_SRC__")
    local MCO1DIV    = this:GetFlagValue("__CLK_MC01_CLK_DIV__")
    local MCO2SRC    = this:GetFlagValue("__CLK_MC02_CLK_SRC__")
    local MCO2DIV    = this:GetFlagValue("__CLK_MC02_CLK_DIV__")
    local CPUVOLTAGE = this:GetFlagValue("__CLK_CPU_VOLTAGE__")

    local freq        = {}
    freq.HSE          = uC.OSCFREQ * HSEON
    freq.HSI          = 16e6
    freq.LSI          = 32e3 * LSION
    freq.LSE          = 32768 * LSEON
    freq.RTCCLK       = 0
    freq.SYSCLK       = 0
    freq.PLLCLK       = 0
    freq.PLLVCO       = 0
    freq.PLL48CLK     = 0
    freq.PLLR         = 0
    freq.PLLM         = 0
    freq.PLLI2SM      = 0
    freq.PLLI2SVCO    = 0
    freq.PLLI2SP      = 0
    freq.PLLI2SQ      = 0
    freq.PLLI2SR      = 0
    freq.PLLSAIM      = 0
    freq.PLLSAIVCO    = 0
    freq.PLLSAIP      = 0
    freq.PLLSAIQ      = 0
    freq.PLLSAIR      = 0
    freq.HCLK         = 0
    freq.FCLK         = 0
    freq.PCLK1        = 0
    freq.PCLK2        = 0
    freq.TIMCLK1      = 0
    freq.TIMCLK2      = 0
    freq.MCO1CLK      = 0
    freq.MCO1CLKDIV   = 0
    freq.MCO2CLK      = 0
    freq.MCO2CLKDIV   = 0
    freq.FLASHLATENCY = 0

    -- calculate RTCCLK --------------------------------------------------------
    if RTCSRC == "RCC_RTCCLKSource_LSI" then
        freq.RTCCLK = freq.LSI
    elseif RTCSRC == "RCC_RTCCLKSource_LSE" then
        freq.RTCCLK = freq.LSE
    else
        freq.RTCCLK = freq.HSE / RTCSRC:gsub("RCC_RTCCLKSource_HSE_Div", "")
    end

    -- calculate PLL Clk frequency ---------------------------------------------
    if PLLON == 1 then
        if PLLSRC == "RCC_PLLSource_HSI" then
           this:SetFlagValue("LABEL_PLL_SRC", PrintFrequency(freq.HSI))
           freq.PLLM = freq.HSI / PLLM
        else
           this:SetFlagValue("LABEL_PLL_SRC", PrintFrequency(freq.HSE))
           freq.PLLM = freq.HSE / PLLM
        end

        if freq.PLLM < 1e6 or freq.PLLM > 2e6 then
           this:SetFlagValue("LABEL_PLL_M", PrintFrequency(freq.PLLM).." [Keep in range 1-2MHz]")
        else
           this:SetFlagValue("LABEL_PLL_M", PrintFrequency(freq.PLLM))
        end

        freq.PLLVCO = freq.PLLM * PLLN
        if freq.PLLVCO < 100e6 or freq.PLLVCO > 432e6 then
           this:SetFlagValue("LABEL_PLL_N", PrintFrequency(freq.PLLVCO).." [Keep in range 100-432MHz]")
        else
           this:SetFlagValue("LABEL_PLL_N", PrintFrequency(freq.PLLVCO))
        end

        freq.PLLCLK = freq.PLLVCO / PLLP
        this:SetFlagValue("LABEL_PLL_P", PrintFrequency(freq.PLLCLK).." (PLLCLK)")

        freq.PLL48CLK = freq.PLLVCO / PLLQ
        this:SetFlagValue("LABEL_PLL_Q", PrintFrequency(freq.PLL48CLK).." (PLL48CLK)")

        freq.PLLR = freq.PLLVCO / PLLR
        this:SetFlagValue("LABEL_PLL_R", PrintFrequency(freq.PLLR))
    else
        this:SetFlagValue("LABEL_PLL_SRC", "")
        this:SetFlagValue("LABEL_PLL_M", "")
        this:SetFlagValue("LABEL_PLL_N", "")
        this:SetFlagValue("LABEL_PLL_P", "")
        this:SetFlagValue("LABEL_PLL_Q", "")
        this:SetFlagValue("LABEL_PLL_R", "")
    end

    -- calculate PLLI2S Clk frequency ------------------------------------------
    if PLLI2SON == 1 then
        if PLLSRC == "RCC_PLLSource_HSI" then
           this:SetFlagValue("LABEL_PLL_SRC", PrintFrequency(freq.HSI))
           freq.PLLI2SM = freq.HSI / PLLI2SM
        else
           this:SetFlagValue("LABEL_PLL_SRC", PrintFrequency(freq.HSE))
           freq.PLLI2SM = freq.HSE / PLLI2SM
        end

        if freq.PLLI2SM < 1e6 or freq.PLLI2SM > 2e6 then
           this:SetFlagValue("LABEL_PLLI2S_M", PrintFrequency(freq.PLLI2SM).." [Keep in range: 1-2MHz]")
        else
           this:SetFlagValue("LABEL_PLLI2S_M", PrintFrequency(freq.PLLI2SM))
        end

        freq.PLLI2SVCO = freq.PLLI2SM * PLLI2SN
        if freq.PLLI2SVCO < 100e6 or freq.PLLI2SVCO > 432e6 then
           this:SetFlagValue("LABEL_PLLI2S_N", PrintFrequency(freq.PLLI2SVCO).." [Keep in range 100-432MHz]")
        else
           this:SetFlagValue("LABEL_PLLI2S_N", PrintFrequency(freq.PLLI2SVCO))
        end

        freq.PLLI2SP = freq.PLLI2SVCO / PLLI2SP
        this:SetFlagValue("LABEL_PLLI2S_P", PrintFrequency(freq.PLLI2SP))

        freq.PLLI2SQ = freq.PLLI2SVCO / PLLI2SQ
        this:SetFlagValue("LABEL_PLLI2S_Q", PrintFrequency(freq.PLLI2SQ).." (PLLSAICLK)")

        freq.PLLI2SR = freq.PLLI2SVCO / PLLI2SR
        if freq.PLLI2SR > 192e6 then
            this:SetFlagValue("LABEL_PLLI2S_R", PrintFrequency(freq.PLLI2SR).." (PLLI2SCLK) [Max is 192 MHz!]")
        else
            this:SetFlagValue("LABEL_PLLI2S_R", PrintFrequency(freq.PLLI2SR).." (PLLI2SCLK)")
        end
    else
        this:SetFlagValue("LABEL_PLLI2S_M", "")
        this:SetFlagValue("LABEL_PLLI2S_N", "")
        this:SetFlagValue("LABEL_PLLI2S_P", "")
        this:SetFlagValue("LABEL_PLLI2S_Q", "")
        this:SetFlagValue("LABEL_PLLI2S_R", "")
    end

    -- calculate PLLSAI Clk frequency ------------------------------------------
    if PLLSAION == 1 then
        if PLLSRC == "RCC_PLLSource_HSI" then
           this:SetFlagValue("LABEL_PLL_SRC", PrintFrequency(freq.HSI))
           freq.PLLSAIM = freq.HSI / PLLSAIM
        else
           this:SetFlagValue("LABEL_PLL_SRC", PrintFrequency(freq.HSE))
           freq.PLLSAIM = freq.HSE / PLLSAIM
        end

        if freq.PLLSAIM < 1e6 or freq.PLLSAIM > 2e6 then
           this:SetFlagValue("LABEL_PLLSAI_M", PrintFrequency(freq.PLLSAIM).." [Keep in range: 1-2MHz]")
        else
           this:SetFlagValue("LABEL_PLLSAI_M", PrintFrequency(freq.PLLSAIM))
        end

        freq.PLLSAIVCO = freq.PLLSAIM * PLLSAIN
        if freq.PLLSAIVCO < 100e6 or freq.PLLSAIVCO > 432e6 then
           this:SetFlagValue("LABEL_PLLSAI_N", PrintFrequency(freq.PLLSAIVCO).." [Keep in range 100-432MHz]")
        else
           this:SetFlagValue("LABEL_PLLSAI_N", PrintFrequency(freq.PLLSAIVCO))
        end

        freq.PLLSAIP = freq.PLLSAIVCO / PLLSAIP
        this:SetFlagValue("LABEL_PLLSAI_P", PrintFrequency(freq.PLLSAIP))

        freq.PLLSAIQ = freq.PLLSAIVCO / PLLSAIQ
        this:SetFlagValue("LABEL_PLLSAI_Q", PrintFrequency(freq.PLLSAIQ).." (PLLSAICLK)")

        freq.PLLSAIR = freq.PLLSAIVCO / PLLSAIR
        this:SetFlagValue("LABEL_PLLSAI_R", PrintFrequency(freq.PLLSAIR).." (PLLLCDCLK)")
    else
        this:SetFlagValue("LABEL_PLLSAI_M", "")
        this:SetFlagValue("LABEL_PLLSAI_N", "")
        this:SetFlagValue("LABEL_PLLSAI_P", "")
        this:SetFlagValue("LABEL_PLLSAI_Q", "")
        this:SetFlagValue("LABEL_PLLSAI_R", "")
    end

    -- calculate SYSCLK --------------------------------------------------------
    if SYSCLKSRC == "RCC_SYSCLKSource_HSE" then
        freq.SYSCLK = freq.HSE
    elseif SYSCLKSRC == "RCC_SYSCLKSource_HSI" then
        freq.SYSCLK = freq.HSI
    else
        freq.SYSCLK = freq.PLLCLK
    end

    -- calculate MCO1 frequency ------------------------------------------------
    if MCO1SRC == "RCC_MCO1Source_HSI" then
        freq.MCO1CLK = freq.HSI
    elseif MCO1SRC == "RCC_MCO1Source_HSE" then
        freq.MCO1CLK = freq.HSE
    elseif MCO1SRC == "RCC_MCO1Source_LSE" then
        freq.MCO1CLK = freq.LSE
    else
        freq.MCO1CLK = freq.PLLCLK
    end
    freq.MCO1CLKDIV = freq.MCO1CLK / MCO1DIV

    -- calculate MCO2 frequency ------------------------------------------------
    if MCO2SRC == "RCC_MCO2Source_SYSCLK" then
        freq.MCO2CLK = freq.SYSCLK
    elseif MCO2SRC == "RCC_MCO2Source_PLLI2SCLK" then
        freq.MCO2CLK = freq.PLLI2SR
    elseif MCO2SRC == "RCC_MCO2Source_I2SCLK" then
        freq.MCO2CLK = 0 -- only STM32F410xx TODO
    elseif MCO2SRC == "RCC_MCO2Source_HSE" then
        freq.MCO2CLK = freq.HSE
    elseif MCO2SRC == "RCC_MCO2Source_PLLCLK" then
        freq.MCO2CLK = freq.PLLCLK
    else
        freq.MCO2CLK = freq.SYSCLK
    end
    freq.MCO2CLKDIV = freq.MCO2CLK / MCO2DIV

    -- calculate output clocks -------------------------------------------------
    freq.HCLK     = freq.SYSCLK / AHBPRE:gsub("RCC_SYSCLK_Div", "")
    freq.FCLK     = freq.HCLK
    freq.PCLK1    = freq.HCLK / APB1PRE:gsub("RCC_HCLK_Div", "")
    freq.PCLK2    = freq.HCLK / APB2PRE:gsub("RCC_HCLK_Div", "")
    freq.TIMxCLK1 = iff(APB1PRE == "RCC_HCLK_Div1", freq.PCLK1, freq.PCLK1 * 2)
    freq.TIMxCLK2 = iff(APB2PRE == "RCC_HCLK_Div1", freq.PCLK1, freq.PCLK1 * 2)

    -- calculate flash latency -------------------------------------------------
    local FLV = 0
    if CPUVOLTAGE == "3p6V" then
        FLV = 30e6
    elseif CPUVOLTAGE == "2p7V" then
        FLV = 24e6
    elseif CPUVOLTAGE == "2p4V" then
        FLV = 22e6
    else --CPUVOLTAGE == "2p1V"
        FLV = 20e6
    end

    freq.FLASHLATENCY = math.ceil(freq.SYSCLK / FLV) - 1
    if freq.FLASHLATENCY < 0 then freq.FLASHLATENCY = 0 end

    -- Results -----------------------------------------------------------------
    this:SetFlagValue("LABEL_LSI_ON", PrintFrequency(freq.LSI).." (LSI)")
    this:SetFlagValue("LABEL_LSE_ON", PrintFrequency(freq.LSE).." (LSE)")
    this:SetFlagValue("LABEL_HSE_ON", PrintFrequency(freq.HSE).." (HSE)")
    this:SetFlagValue("LABEL_SYSCLK_SRC", PrintFrequency(freq.SYSCLK).." (SYSCLK)")
    this:SetFlagValue("LABEL_RTCCLK_SRC", PrintFrequency(freq.RTCCLK).." (RTCCLK)")
    this:SetFlagValue("LABEL_AHB_PRE", PrintFrequency(freq.HCLK).." (HCLK)")
    this:SetFlagValue("LABEL_APB1_PRE", PrintFrequency(freq.PCLK1).." (PCLK1)")
    this:SetFlagValue("LABEL_APB2_PRE", PrintFrequency(freq.PCLK2).." (PCLK2)")
    this:SetFlagValue("LABEL_MCO1_SRC", PrintFrequency(freq.MCO1CLK))
    this:SetFlagValue("LABEL_MCO1_DIV", PrintFrequency(freq.MCO1CLKDIV).." (MCO1 pin)")
    this:SetFlagValue("LABEL_MCO2_SRC", PrintFrequency(freq.MCO2CLK))
    this:SetFlagValue("LABEL_MCO2_DIV", PrintFrequency(freq.MCO2CLKDIV).." (MCO2 pin)")
    this:SetFlagValue("LABEL_FLASH_LATENCY", freq.FLASHLATENCY.." Flash delay cycles")
    this:SetFlagValue("__CLK_FLASH_LATENCY__", tostring(freq.FLASHLATENCY))
end

-- trigger frequency calculation after loading all flags
this.CalculateFreq()
++*/

#endif /* _CLK_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
