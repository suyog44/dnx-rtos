--[[============================================================================
@file    stm32f1-spi.lua

@author  Daniel Zorychta

@brief   Configuration script for SDSPI driver module

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


==============================================================================]]
module(..., package.seeall)


--==============================================================================
-- EXTERNAL MODULES
--==============================================================================
require("wx")
require("wizcore")


--==============================================================================
-- GLOBAL OBJECTS
--==============================================================================
-- module's main object
spi = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
-- local objects
local ui = {}
local ID = {}
local number_of_cs = 8


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  x
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_controls()
        local enable     = wizcore:get_module_state("SPI")
        local dummy_byte = wizcore:key_read()


        ui.CheckBox_enable:SetValue(enable)
        ui.Panel1:Enable(enable)
end


--------------------------------------------------------------------------------
-- @brief  x
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function on_button_save_click()
        local enable = ui.CheckBox_enable:GetValue()


        ui.Button_save:Enable(false)
end


--------------------------------------------------------------------------------
-- @brief  x
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function checkbox_enable_updated(this)
        ui.Button_save:Enable(true)
        ui.Panel1:Enable(this:IsChecked())
end


--------------------------------------------------------------------------------
-- @brief  x
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function checkbox_device_enable_updated(this)
        ui.Panel2:Enable(this:IsChecked())
end


--------------------------------------------------------------------------------
-- @brief  x
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function value_updated()
        ui.Button_save:Enable(true)
end


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function spi:create_window(parent)
        ui = {}
        ui.StaticText_cspin = {}
        ui.Choice_cspin     = {}

        ID = {}
        ID.CHECKBOX_ENABLE = wx.wxNewId()
        ID.TEXTCTRL_DUMMY_BYTE = wx.wxNewId()
        ID.CHOICE_CLKDIV = wx.wxNewId()
        ID.CHOICE_MODE = wx.wxNewId()
        ID.CHOICE_BITORDER = wx.wxNewId()
        ID.CHOICE_DEVICE = wx.wxNewId()
        ID.CHECKBOX_DEVICE_ENABLE = wx.wxNewId()
        ID.CHOICE_IRQPRIO = wx.wxNewId()
        ID.CHOICE_CSNUM = wx.wxNewId()
        ID.CHOICE_CSPIN = {}
        ID.PANEL2 = wx.wxNewId()
        ID.PANEL1 = wx.wxNewId()
        ID.STATICLINE1 = wx.wxNewId()
        ID.BUTTON_SAVE = wx.wxNewId()


        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
        local this = ui.window

        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.CheckBox_enable = wx.wxCheckBox(this, ID.CHECKBOX_ENABLE, "Enable module", wx.wxDefaultPosition, wx.wxSize(wizcore.CONTROL_X_SIZE, -1), 0, wx.wxDefaultValidator, "ID.CHECKBOX_ENABLE")
        ui.FlexGridSizer1:Add(ui.CheckBox_enable, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Panel1 = wx.wxPanel(this, ID.PANEL1, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL, "ID.PANEL1")
        ui.FlexGridSizer2 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.StaticBoxSizer1 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel1, "Default settings")
        ui.FlexGridSizer3 = wx.wxFlexGridSizer(0, 2, 0, 0)
        ui.StaticText1 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Dummy byte", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.TextCtrl_dummy_byte = wx.wxTextCtrl(ui.Panel1, ID.TEXTCTRL_DUMMY_BYTE, "", wx.wxDefaultPosition, wx.wxSize(40,-1), 0, wx.wxDefaultValidator, "ID.TEXTCTRL_DUMMY_BYTE")
        ui.FlexGridSizer3:Add(ui.TextCtrl_dummy_byte, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText2 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Clock divider", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_clkdiv = wx.wxChoice(ui.Panel1, ID.CHOICE_CLKDIV, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_CLKDIV")
        ui.Choice_clkdiv:Append("/2")
        ui.Choice_clkdiv:Append("/4")
        ui.Choice_clkdiv:Append("/8")
        ui.Choice_clkdiv:Append("/16")
        ui.Choice_clkdiv:Append("/32")
        ui.Choice_clkdiv:Append("/64")
        ui.Choice_clkdiv:Append("/128")
        ui.Choice_clkdiv:Append("/256")
        ui.FlexGridSizer3:Add(ui.Choice_clkdiv, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText3 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "SPI mode", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_mode = wx.wxChoice(ui.Panel1, ID.CHOICE_MODE, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_MODE")
        ui.Choice_mode:Append("Mode 0 (SCK Low at idle; capture on leading edge)")
        ui.Choice_mode:Append("Mode 1 (SCK Low at idle; capture on trailing edge)")
        ui.Choice_mode:Append("Mode 2 (SCK High at idle; capture on leading edge)")
        ui.Choice_mode:Append("Mode 3 (SCK High at idle; capture on trailing edge)")
        ui.FlexGridSizer3:Add(ui.Choice_mode, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText4 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Bit order", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_bitorder = wx.wxChoice(ui.Panel1, ID.CHOICE_BITORDER, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_BITORDER")
        ui.Choice_bitorder:Append("MSb first")
        ui.Choice_bitorder:Append("LSb first")
        ui.FlexGridSizer3:Add(ui.Choice_bitorder, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticBoxSizer1:Add(ui.FlexGridSizer3, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.FlexGridSizer2:Add(ui.StaticBoxSizer1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticBoxSizer2 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel1, "Device settings")
        ui.FlexGridSizer4 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.Choice_device = wx.wxChoice(ui.Panel1, ID.CHOICE_DEVICE, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_DEVICE")
        ui.FlexGridSizer4:Add(ui.Choice_device, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Panel2 = wx.wxPanel(ui.Panel1, ID.PANEL2, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL, "ID.PANEL2")
        ui.FlexGridSizer5 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.CheckBox_device_enable = wx.wxCheckBox(ui.Panel1, ID.CHECKBOX_DEVICE_ENABLE, "Enable device", wx.wxDefaultPosition, wx.wxSize(wizcore.CONTROL_X_SIZE, -1), 0, wx.wxDefaultValidator, "ID.CHECKBOX_DEVICE_ENABLE")
        ui.FlexGridSizer4:Add(ui.CheckBox_device_enable, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.FlexGridSizer6 = wx.wxFlexGridSizer(0, 2, 0, 0)
        ui.StaticText5 = wx.wxStaticText(ui.Panel2, wx.wxID_ANY, "IRQ priority", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer6:Add(ui.StaticText5, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_irqprio = wx.wxChoice(ui.Panel2, ID.CHOICE_IRQPRIO, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_IRQPRIO")
        ui.FlexGridSizer6:Add(ui.Choice_irqprio, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText6 = wx.wxStaticText(ui.Panel2, wx.wxID_ANY, "Number of Chip Selects", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer6:Add(ui.StaticText6, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_csnum = wx.wxChoice(ui.Panel2, ID.CHOICE_CSNUM, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_CSNUM")
        ui.Choice_csnum:Append("1 (CS0 only)")
        ui.Choice_csnum:Append("2 (CS0..1)")
        ui.Choice_csnum:Append("3 (CS0..2)")
        ui.Choice_csnum:Append("4 (CS0..3)")
        ui.Choice_csnum:Append("5 (CS0..4)")
        ui.Choice_csnum:Append("6 (CS0..5)")
        ui.Choice_csnum:Append("7 (CS0..6)")
        ui.Choice_csnum:Append("8 (CS0..7)")
        ui.FlexGridSizer6:Add(ui.Choice_csnum, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        for i = 1, number_of_cs do
            ui.StaticText_cspin[i] = wx.wxStaticText(ui.Panel2, wx.wxID_ANY, "Pin for Chip Select "..i-1, wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
            ui.FlexGridSizer6:Add(ui.StaticText_cspin[i], 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
            ID.CHOICE_CSPIN[i] = wx.wxNewId()
            ui.Choice_cspin[i] = wx.wxChoice(ui.Panel2, ID.CHOICE_CSPIN[i], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_CSPIN")
            ui.FlexGridSizer6:Add(ui.Choice_cspin[i], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        end
        ui.FlexGridSizer5:Add(ui.FlexGridSizer6, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.Panel2:SetSizer(ui.FlexGridSizer5)
        ui.FlexGridSizer4:Add(ui.Panel2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.StaticBoxSizer2:Add(ui.FlexGridSizer4, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.FlexGridSizer2:Add(ui.StaticBoxSizer2, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Panel1:SetSizer(ui.FlexGridSizer2)
        ui.FlexGridSizer2:Fit(ui.Panel1)
        ui.FlexGridSizer2:SetSizeHints(ui.Panel1)
        ui.FlexGridSizer1:Add(ui.Panel1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.StaticLine1 = wx.wxStaticLine(this, ID.STATICLINE1, wx.wxDefaultPosition, wx.wxSize(10,-1), wx.wxLI_HORIZONTAL, "ID.STATICLINE1")
        ui.FlexGridSizer1:Add(ui.StaticLine1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Button_save = wx.wxButton(this, ID.BUTTON_SAVE, "Save", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.BUTTON_SAVE")
        ui.FlexGridSizer1:Add(ui.Button_save, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)

        --
        this:SetSizer(ui.FlexGridSizer1)
        this:SetScrollRate(50, 50)

        --
        this:Connect(ID.CHECKBOX_ENABLE,        wx.wxEVT_COMMAND_CHECKBOX_CLICKED, checkbox_enable_updated       )
        this:Connect(ID.CHECKBOX_DEVICE_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED, checkbox_device_enable_updated)
--         this:Connect(ID.CHECKBOX_LOCK,   wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_updated          )
--         this:Connect(ID.CHECKBOX_DEBUG,  wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_updated          )
--         this:Connect(ID.CHOICE_TIMEOUT,  wx.wxEVT_COMMAND_CHOICE_SELECTED,  value_updated          )
--         this:Connect(ID.BUTTON_SAVE,     wx.wxEVT_COMMAND_BUTTON_CLICKED,   on_button_save_click   )

        --
        load_controls()
        ui.Button_save:Enable(false)

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function spi:get_window_name()
        return "SPI"
end


--------------------------------------------------------------------------------
-- @brief  Function is called by parent when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function spi:selected()
end


--------------------------------------------------------------------------------
-- @brief  Function returns modify status
-- @param  None
-- @return If data is modified true is returned, otherwise false
--------------------------------------------------------------------------------
function spi:is_modified()
        return ui.Button_save:IsEnabled()
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @param  None
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return spi
end
