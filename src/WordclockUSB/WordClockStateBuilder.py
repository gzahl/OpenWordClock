#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# =============================================================================
# Filename:     Wordclock.py
# Author:       Jacob Nürnberg
# Date:         06.06.2020
# Description:  Layout of the wordclock
# =============================================================================

# Copyright © 2022 Jacob Nuernberg

# Permission is hereby granted, free of charge, to any person obtaining a copy 
# of this software and associated documentation files (the “Software”), to deal 
# in the Software without restriction, including without limitation the rights 
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
# copies of the Software, and to permit persons to whom the Software is 
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all 
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
# SOFTWARE.

import wx
import pyperclip
from wx.lib.mixins.listctrl import ListCtrlAutoWidthMixin

thelayout = ('\u25cf\u25cf'
             'ESKISTLFÜNF'
             'ZEHNZWANZIG'
             'DREIVIERTEL'
             'TGNACHVORJM'
             'HALBQZWÖLFP'
             'ZWEINSIEBEN'
             'KDREIRHFÜNF'
             'ELFNEUNVIER'
             'WACHTZEHNRS'
             'BSECHSFMUHR'
             '\u25cf\u25cf')

theindices = [3, 2,
              4, 23, 24, 43, 44, 63, 64, 83, 84, 103, 104,
              5, 22, 25, 42, 45, 62, 65, 82, 85, 102, 105,
              6, 21, 26, 41, 46, 61, 66, 81, 86, 101, 106,
              7, 20, 27, 40, 47, 60, 67, 80, 87, 100, 107,
              8, 19, 28, 39, 48, 59, 68, 79, 88, 99, 108,
              9, 18, 29, 38, 49, 58, 69, 78, 89, 98, 109,
              10, 17, 30, 37, 50, 57, 70, 77, 90, 97, 110,
              11, 16, 31, 36, 51, 56, 71, 76, 91, 96, 111,
              12, 15, 32, 35, 52, 55, 72, 75, 92, 95, 112,
              13, 14, 33, 34, 53, 54, 73, 74, 93, 94, 113,
              0, 1]


def generate(thelayout):
    for i in range(len(thelayout)):
        yield thelayout[i]


class Wordblock(wx.Panel):
    def __init__(self, parent, letter, index):
        super(Wordblock, self).__init__(parent)
        self.parent = parent
        self.letter = letter
        self.index = index
        self.isOn = False

        font = wx.Font(32,
                       wx.FONTFAMILY_SWISS,
                       wx.FONTSTYLE_NORMAL,
                       wx.FONTWEIGHT_BOLD)

        self.txt = wx.StaticText(self, label=letter, style=wx.ALIGN_CENTER)
        self.txt.SetFont(font)
        self.txt.SetForegroundColour('#444444')

        box = wx.BoxSizer(wx.HORIZONTAL)
        box.Add(self.txt, wx.ID_ANY, wx.CENTER)

        self.SetSizer(box)

        self.txt.Bind(wx.EVT_LEFT_DOWN, self.OnClick)

    def OnClick(self, e):
        if self.isOn:
            self.txt.SetForegroundColour('#444444')
        else:
            self.txt.SetForegroundColour('#ffffff')
        self.isOn = not self.isOn
        self.parent.update(self.index)


class TheInfoPanel(wx.Panel):
    def __init__(self, parent):
        super(TheInfoPanel, self).__init__(parent)

        # InfoPanel
        InfoSizer = wx.BoxSizer(wx.VERTICAL)

        self.InfoTable = AutoWidthListCtrl(self, wx.ID_ANY, style=wx.LC_REPORT)
        self.InfoTable.InsertColumn(0, 'Index', wx.LIST_FORMAT_CENTER,)
        self.InfoTable.InsertColumn(1, 'Letter', wx.LIST_FORMAT_CENTER)
        self.InfoTable.InsertColumn(2, 'Status', wx.LIST_FORMAT_CENTER)

        for i, item in enumerate(thelayout):
            self.InfoTable.InsertItem(i, repr(i))
            self.InfoTable.SetItem(i, 1, item)
            self.InfoTable.SetItem(i, 2, 'off')

        InfoSizer.Add(self.InfoTable, 3, flag=wx.EXPAND | wx.ALL, border=10)

        box = wx.StaticBox(self, -1, 'Output')
        boxsizer = wx.StaticBoxSizer(box, wx.HORIZONTAL)
        flexgrid = wx.FlexGridSizer(7, 3, 5, 5)

        label_bin = wx.StaticText(box, label='Bin:')
        self.disp_bin = wx.TextCtrl(box, style=wx.TE_LEFT)
        button_bin = wx.Button(box, wx.ID_ANY, label='Copy')
        self.Bind(wx.EVT_BUTTON,
                  lambda event: self.copytoclip(event, self.disp_bin),
                  button_bin)

        label_hex = wx.StaticText(box, label='Hex:')
        self.disp_hex = wx.TextCtrl(box, style=wx.TE_LEFT)
        button_hex = wx.Button(box, wx.ID_ANY, label='Copy')
        self.Bind(wx.EVT_BUTTON,
                  lambda event: self.copytoclip(event, self.disp_hex),
                  button_hex)

        label_idx = wx.StaticText(box, label='Idx:')
        self.disp_idx = wx.TextCtrl(box, style=wx.TE_LEFT)
        button_idx = wx.Button(box, wx.ID_ANY, label='Copy')
        self.Bind(wx.EVT_BUTTON,
                  lambda event: self.copytoclip(event, self.disp_idx),
                  button_idx)

        label_4int = wx.StaticText(box, label='4-int:')
        self.disp_4int = wx.TextCtrl(box, style=wx.TE_LEFT)
        button_4int = wx.Button(box, wx.ID_ANY, label='Copy')
        self.Bind(wx.EVT_BUTTON,
                  lambda event: self.copytoclip(event, self.disp_4int),
                  button_4int)

        label_4hex = wx.StaticText(box, label='4x32-bit:')
        self.disp_4hex = wx.TextCtrl(box, style=wx.TE_LEFT)
        button_4hex = wx.Button(box, wx.ID_ANY, label='Copy')
        self.Bind(wx.EVT_BUTTON,
                  lambda event: self.copytoclip(event, self.disp_4hex),
                  button_4hex)

        label_8x16b = wx.StaticText(box, label='8x16-bit:')
        self.disp_8x16b = wx.TextCtrl(box, style=wx.TE_LEFT)
        button_8x16b = wx.Button(box, wx.ID_ANY, label='Copy')
        self.Bind(wx.EVT_BUTTON,
                  lambda event: self.copytoclip(event, self.disp_8x16b),
                  button_8x16b)

        label_16B = wx.StaticText(box, label='16x8-bit:')
        self.disp_16B = wx.TextCtrl(box, style=wx.TE_LEFT)
        button_16B = wx.Button(box, wx.ID_ANY, label='Copy')
        self.Bind(wx.EVT_BUTTON,
                  lambda event: self.copytoclip(event, self.disp_16B),
                  button_16B)

        flexgrid.AddMany(
            [(label_bin), (self.disp_bin, 1, wx.EXPAND), (button_bin),
             (label_hex), (self.disp_hex, 1, wx.EXPAND), (button_hex),
             (label_idx), (self.disp_idx, 1, wx.EXPAND), (button_idx),
             (label_4int), (self.disp_4int, 1, wx.EXPAND), (button_4int),
             (label_4hex), (self.disp_4hex, 1, wx.EXPAND), (button_4hex),
             (label_8x16b), (self.disp_8x16b, 1, wx.EXPAND), (button_8x16b),
             (label_16B), (self.disp_16B, 1, wx.EXPAND), (button_16B)])

        flexgrid.AddGrowableCol(1)

        boxsizer.Add(flexgrid, 1, flag=wx.EXPAND, border=5)
        InfoSizer.Add(boxsizer, flag=wx.EXPAND | wx.ALL, border=5)

        self.SetSizer(InfoSizer)

    def copytoclip(self, event, source):
        pyperclip.copy(repr(source.GetValue()))


class AutoWidthListCtrl(wx.ListCtrl, ListCtrlAutoWidthMixin):
    def __init__(self, parent, *args, **kw):
        wx.ListCtrl.__init__(self, parent, wx.ID_ANY, style=wx.LC_REPORT)
        ListCtrlAutoWidthMixin.__init__(self)


class Wordclock(wx.Frame):

    def __init__(self, parent, title):
        super(Wordclock, self).__init__(parent, title=title, size=(1200, 800))

        self.state = 0

        self.InitUI()
        self.Center()
        self.Show()

    def InitUI(self):

        MainSizer = wx.BoxSizer(wx.HORIZONTAL)

        # Clock Display
        Clockgrid = wx.GridSizer(12, 13, 0, 0)
        letter = generate(thelayout)
        index = generate(theindices)
        for row in range(12):
            for col in range(13):
                if not (row % 11) and not (col % 12):
                    subpanel = Wordblock(self, next(letter), next(index))
                    subpanel.SetBackgroundColour('#000000')
                elif (row % 11) and (col % 12):
                    subpanel = Wordblock(self, next(letter), next(index))
                    subpanel.SetBackgroundColour('#000000')
                else:
                    subpanel = wx.Panel(self)
                    subpanel.SetBackgroundColour('#000000')
                Clockgrid.Add(subpanel, 0, wx.EXPAND | wx.ALL, border=0)
        MainSizer.Add(Clockgrid,
                      proportion=4,
                      flag=wx.ALL | wx.EXPAND,
                      border=10)

        self.infopanel = TheInfoPanel(self)
        # infopanel.SetBackgroundColour('#bbbbbb')
        MainSizer.Add(self.infopanel, proportion=1,
                      flag=wx.ALL | wx.EXPAND, border=10)

        self.SetSizer(MainSizer)

    def update(self, index):
        self.state ^= (1 << index)
        self.infopanel.disp_bin.ChangeValue(bin(self.state))
        self.infopanel.disp_hex.ChangeValue(hex(self.state))
        indexlist = []
        for i in range(114):
            if self.state & (1 << i):
                indexlist.append(i)
        self.infopanel.disp_idx.ChangeValue(repr(indexlist))

        intlist = []
        hexlist = []
        bit16list = []
        bytelist = []
        for i in range(4):
            temp = int('0b' + '1' * 32, 2)
            intlist.append(temp & self.state >> (32 * i))
            hexlist.append(hex(intlist[i]))
            for j in range(2):
                bit16list.append(hex(0xffff & intlist[i] >> 16 * j))
            for k in range(4):
                bytelist.append(hex(0xff & intlist[i] >> 8 * k))
        self.infopanel.disp_4int.ChangeValue(repr(intlist))
        self.infopanel.disp_4hex.ChangeValue(repr(hexlist))
        self.infopanel.disp_8x16b.ChangeValue(repr(bit16list))
        self.infopanel.disp_16B.ChangeValue(repr(bytelist))

    def OnQuit(self, e):
        self.Close()


if __name__ == '__main__':
    app = wx.App()
    Wordclock(None, 'Wordclock Simulator')
    app.MainLoop()
