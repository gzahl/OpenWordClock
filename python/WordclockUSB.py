#! /usr/bin/env python3
# -*- coding: utf-8 -*-

import wx
import wx.lib.inspection
import serial
import threading
from serial.tools import list_ports

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


def cvalue(index):
    index %= 102
    if index < 17:
        return index * 15
    elif index < 51:
        return 255
    elif index < 68:
        return (68 - index) * 15
    else:
        return 0


def ccode(index):
    if index == -1:
        return (255, 255, 255)
    else:
        return (cvalue(index), cvalue(index + 34), cvalue(index + 68))


def generate(thelayout):
    for i in range(len(thelayout)):
        yield thelayout[i]


class LetterBlock(wx.Panel):
    def __init__(self, parent, letter, index):
        super(LetterBlock, self).__init__(parent)
        self.parent = parent
        self.letter = letter
        self.index = index
        self.isOn = False

        self.SetBackgroundColour('#000000')
        font = wx.Font(24,
                       wx.FONTFAMILY_SWISS,
                       wx.FONTSTYLE_NORMAL,
                       wx.FONTWEIGHT_BOLD)

        self.txt = wx.StaticText(self, label=letter, style=wx.ALIGN_CENTER)
        self.txt.SetFont(font)
        self.txt.SetForegroundColour('#444444')
        box = wx.BoxSizer(wx.HORIZONTAL)
        box.Add(self.txt, wx.ID_ANY, wx.CENTER, 0)
        self.SetSizer(box)


class WordClockUSB(wx.Frame):

    def __init__(self, parent, title):
        super(WordClockUSB, self).__init__(parent, title=title)

        self.Listener = None
        self.ListenerAlive = threading.Event()
        self.SerialConnection = None

        self.letters = [None]*114

        mainsizer = self.BuildLayout()
        self.SetSizer(mainsizer)

        self.ports = [item.device for item in list_ports.comports()]
        self.portctrl.Set(self.ports)
        self.portctrl.SetSelection(0)

        self.startbtn.Bind(wx.EVT_BUTTON, self.toggleListener)
        self.clearbtn.Bind(wx.EVT_BUTTON, self.clearConsole)

        self.SetSize(1200, 800)
        self.Center()
        self.Show()

    def BuildLayout(self):
        mainsizer = wx.BoxSizer(wx.HORIZONTAL)

        Clockgrid = self.BuildClock()
        mainsizer.Add(Clockgrid, proportion=2,
                      flag=wx.EXPAND | wx.ALL, border=10)

        vbox = wx.BoxSizer(wx.VERTICAL)

        font = wx.Font(16,
                       wx.FONTFAMILY_SWISS,
                       wx.FONTSTYLE_NORMAL,
                       wx.FONTWEIGHT_BOLD)
        self.ClockDisplay = wx.StaticText(
            self, label='12:34:56\n1. April 1970', style=wx.ALIGN_CENTER)
        self.ClockDisplay.SetFont(font)
        vbox.Add(self.ClockDisplay, flag=wx.EXPAND
                 | wx.ALIGN_CENTER_HORIZONTAL | wx.ALL, border=30)

        fgrid = wx.FlexGridSizer(2, 2, 10, 10)

        fgrid.Add(wx.StaticText(self, label='Port:'))
        self.portctrl = wx.ComboBox(self)
        fgrid.Add(self.portctrl, 1, wx.EXPAND)
        fgrid.Add(wx.StaticText(self, label='Baudrate:'))
        baudrates = ['300', '1200', '2400', '4800', '9600',
                     '19200', '38400', '57600', '74880', '115200', '230400']
        self.baudctrl = wx.ComboBox(self, choices=baudrates)
        self.baudctrl.SetSelection(4)
        fgrid.Add(self.baudctrl, 1, wx.EXPAND)
        fgrid.AddGrowableCol(1, 1)
        vbox.Add(fgrid, 0, wx.EXPAND | wx.TOP | wx.BOTTOM, border=10)

        btnhbox = wx.BoxSizer(wx.HORIZONTAL)
        self.clearbtn = wx.Button(self, label='Clear console')
        btnhbox.Add(self.clearbtn, 1, wx.EXPAND | wx.LEFT | wx.RIGHT, 10)
        self.startbtn = wx.Button(self, label='Start listening')
        btnhbox.Add(self.startbtn, 1, wx.EXPAND | wx.LEFT | wx.RIGHT, 10)
        vbox.Add(btnhbox, 0, wx.EXPAND | wx.ALL, 0)

        self.Console = wx.TextCtrl(
            self, style=wx.TE_MULTILINE | wx.TE_READONLY)
        vbox.Add(self.Console, proportion=1,
                 flag=wx.EXPAND | wx.TOP, border=10)

        mainsizer.Add(vbox, proportion=1, flag=wx.EXPAND | wx.ALL, border=10)
        return mainsizer

    def BuildClock(self):
        Clockgrid = wx.GridSizer(12, 13, 0, 0)
        letter = generate(thelayout)
        index = generate(theindices)
        for row in range(12):
            for col in range(13):
                if not (row % 11) and not (col % 12):
                    subpanel = LetterBlock(self, next(letter), next(index))
                    self.letters[subpanel.index] = subpanel
                elif (row % 11) and (col % 12):
                    subpanel = LetterBlock(self, next(letter), next(index))
                    self.letters[subpanel.index] = subpanel
                else:
                    subpanel = wx.Panel(self)
                    subpanel.SetBackgroundColour('#000000')
                Clockgrid.Add(subpanel, 0, wx.EXPAND | wx.ALL, border=0)
        return Clockgrid

    def toggleListener(self, evt):
        if self.Listener is None:
            self.startbtn.SetLabel('Stop listening')
            self.portctrl.Disable()
            self.baudctrl.Disable()

            self.SerialConnection = serial.Serial(
                self.portctrl.GetStringSelection(),
                int(self.baudctrl.GetStringSelection()),
                timeout=2
            )

            self.Listener = threading.Thread(target=self.ListenerRoutine)
            self.Listener.setDaemon(1)
            self.ListenerAlive.set()
            self.Listener.start()

        else:
            self.startbtn.SetLabel('Start listening')
            self.portctrl.Enable()
            self.baudctrl.Enable()

            self.ListenerAlive.clear()
            self.Listener.join()
            self.Listener = None

            self.SerialConnection.close()
            self.SerialConnection = None

    def ListenerRoutine(self):
        while self.ListenerAlive.isSet():
            msg = self.SerialConnection.readline()
            if msg:
                self.Console.AppendText(msg)

                # self.Console.SetInsertionPoint(-1)
                # self.Console.ShowPosition(-1)
                parts = msg.split()  # FFFF
                if len(parts) == 9:
                    for partno, part in enumerate(msg.split()):
                        if partno == 0:
                            color = ccode(int(part))
                            continue
                        else:
                            for bit in range(16):
                                index = (partno - 1)*16 + bit
                                if index > 113:
                                    break
                                if (0x01 & int(part.decode('utf-8'), 16) >> bit):
                                    self.letters[index].txt.SetForegroundColour(
                                        color)
                                else:
                                    self.letters[index].txt.SetForegroundColour(
                                        '#444444')
        self.Console.SetScrollPos(
            wx.VERTICAL, self.Console.GetScrollRange(wx.VERTICAL))

    def clearConsole(self, evt):
        self.Console.Clear()

    def OnQuit(self, e):
        self.Close()


if __name__ == '__main__':
    app = wx.App()
    ex = WordClockUSB(None, 'WordClockUSB')
    ex.Show()
    # wx.lib.inspection.InspectionTool().Show()

    app.MainLoop()
