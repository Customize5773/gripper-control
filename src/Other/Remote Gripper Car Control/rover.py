#!/usr/bin/env python
#
# For Linux, BSD or Mac OSX you can chmod +x on this script to make executable
#
###########
# Rover control app
#
# Written by Scott Beasley - 2015
# Free to use or modify. Enjoy.
#
###########

import sys
import serial
import time
from Tkinter import *
import tkFont
import tkMessageBox

# Create the window for the application
class App (Frame):
    # Make the window
    def createWidgets (self):
        self.connected = False
        self.message = StringVar ( )

        # Make a little font for the gripper set buttons.
        helv6 = tkFont.Font (family = 'Helvetica',
                             size = 6, weight = 'normal')

        self.frame = Frame (self.master)
        self.frame.pack ( )

        self.f1 = Frame (self.frame)
        self.l1 = Label (self.f1, text = "Comm Port: ")
        self.l1.pack (side = LEFT)
        self.comm_entry = Entry (self.f1, bd = 5, name = "comm_entry")
        self.comm_entry.pack (side = LEFT)
        self.connectButton = Button (self.f1, text = "Connect",
                                    command = self.SerialConnect,
                                    name = "b_connect")
        self.connectButton.pack (side = LEFT)
        self.disconnectButton = Button (self.f1, text = "Disconnect",
                                    command = self.SerialDisconnect,
                                    name = "b_disconnect")
        self.disconnectButton.pack (side = RIGHT)
        self.f1.grid (row = 0, column = 0)

        self.f2 = LabelFrame (self.frame, bd = 3, relief = "groove",
                              text="Ground Control")

        self.g_vert_fm = Frame (self.f2)
        self.grip_vert = Scale (self.g_vert_fm, from_ = 0, to = 180)
        self.grip_vert.grid (row = 0, column = 0, rowspan = 4, sticky = W)
        self.grip_vert_set = Button (self.g_vert_fm, text = "Set",
                                     command = self.GripperY,
                                     name = "b_grip_vert_set",
                                     width = 1, height = 2,
                                     font = helv6)
        self.grip_vert_set.grid (row = 5, column = 0, sticky = W)
        self.master.bind ("<y>", self.GripperY)
        self.g_vert_fm.grid (row = 0, column = 0, rowspan = 4, sticky = W)

        self.leftforwardButton = Button (self.f2, text = "\\",
                                 command = self.TurnLeft45,
                                 name = "b_left_forward")
        self.leftforwardButton.grid (row = 0, column = 1)
        self.master.bind ("<a>", self.TurnLeft45)

        self.leftButton = Button (self.f2, text = "<",
                                 command = self.TurnLeft, name = "b_left")
        self.leftButton.grid (row = 1, column = 1)
        self.master.bind ("<Left>", self.TurnLeft)

        self.rightforwardButton = Button (self.f2, text = "/",
                                 command = self.TurnRight45,
                                 name = "b_right_forward")
        self.rightforwardButton.grid (row = 0, column = 3)
        self.master.bind ("<s>", self.TurnRight45)

        self.haltButton = Button (self.f2, text = "Halt!",
                                 command = self.Halt, name = "b_halt")
        self.haltButton.grid (row = 1, column = 2)
        self.master.bind ("<h>", self.Halt)

        self.rightButton = Button (self.f2, text=">",
                                  command = self.TurnRight, name = "b_right")
        self.rightButton.grid(row = 1, column = 3)
        self.master.bind ("<Right>", self.TurnRight)

        self.upButton = Button (self.f2, text="^",
                               command=self.Forward, name = "b_forward")
        self.upButton.grid (row = 0, column = 2)
        self.master.bind ("<Up>", self.Forward)

        self.leftdownButton = Button (self.f2, text = "/",
                                      command = self.TurnRight45,
                                      name = "b_left_down")
        self.leftdownButton.grid (row = 2, column = 1)

        self.downButton = Button (self.f2, text="V",
                                  command=self.Reverse, name = "b_reverse")
        self.downButton.grid (row=2, column = 2)
        self.master.bind ("<Down>", self.Reverse)
        self.f2.grid (row = 1, column = 0, pady = 25)

        self.rightdownButton = Button (self.f2, text = "\\",
                                       command = self.TurnLeft45,
                                       name = "b_right_down")
        self.rightdownButton.grid (row = 2, column = 3)

        self.g_horz_fm = Frame (self.f2)
        self.grip_horz = Scale (self.g_horz_fm, from_ = 0, to = 180,
                                orient = HORIZONTAL)
        self.grip_horz.grid (row = 0, column = 0, columnspan = 7, sticky = E)

        self.grip_horz_set = Button (self.g_horz_fm, text = "Set",
                                     command = self.GripperX,
                                     name = "b_grip_horz_set",
                                     width = 1, height = 2,
                                     font = helv6)
        self.grip_horz_set.grid (row = 0, column = 8)
        self.master.bind ("<u>", self.GripperX)
        self.g_horz_fm.grid (row = 4, column = 0, columnspan = 7, sticky = E)
        self.master.bind ("<c>", self.GripperHome)

        self.f3 = Frame (self.frame)
        self.l2 = Label (self.f3, text = "Last action: ")
        self.l2.pack (side = LEFT)
        self.l3 = Label (self.f3, text=" ", textvariable = self.message)
        self.l3.pack (side = RIGHT)
        self.f3.grid (row = 3, column = 0, pady = 8)

    # Set the state of the bot control buttons. Enable when connected,
    # Disabled otherwise.
    def CtrlButtonsState (self, bstate):
        self.leftforwardButton.config (state = bstate)
        self.leftButton.config (state = bstate)
        self.rightforwardButton.config (state = bstate)
        self.rightButton.config (state = bstate)
        self.upButton.config (state = bstate)
        self.leftdownButton.config (state = bstate)
        self.downButton.config (state = bstate)
        self.rightdownButton.config (state = bstate)
        self.haltButton.config (state = bstate)
        self.disconnectButton.config (state = bstate)
        self.grip_horz.config (state = bstate)
        self.grip_vert.config (state = bstate)
        self.grip_horz_set.config (state = bstate)
        self.grip_vert_set.config (state = bstate)

    # Set the state of the comm port entry. Enable when not connected,
    # Disabled when the bot is connected.
    def ConnCtrlsState (self, bstate):
        self.connectButton.config (state = bstate)
        self.comm_entry.config (state = bstate)

    # Connect to the comm port typed in the comm entry field.
    def SerialConnect (self):
        try:
            # Change the baud rate here if diffrent then 57600
            self.ser = serial.Serial (self.comm_entry.get ( ), 57600)
        except IOError:
            tkMessageBox.showerror ("Invalid comm port", "Comm port not found.")
            return

        self.ConnCtrlsState (DISABLED)
        self.CtrlButtonsState (NORMAL)
        self.message.set ("SerialConnect")
        self.connected = True
        time.sleep (3) # Dwell a bit for the connection to happen

    # Disconnect from the bot (close the comm port).
    def SerialDisconnect (self):
        try:
            # Send a Halt command just in case the bot is still moving.
            self.send_cmd ('h', "Halt!")
            time.sleep (1)
            self.ser.close ( )
        except IOError:
            print "Could not close port..."

        self.message.set ("SerialDisconnect")
        self.ConnCtrlsState (NORMAL)
        self.CtrlButtonsState (DISABLED)
        self.connected = False
        time.sleep (2) # Dwell a bit for the disconnection to happen

    # Send the command to the open comm port
    def send_cmd (self, action, msg):
        if self.connected == True:
            for val in action:
                self.ser.write (val)
            self.ser.flush ( )
            self.message.set (msg)

    # Send the bot a turn-left command.
    def TurnLeft (self, event = None):
        self.send_cmd ('a', "Left")

    # Send the bot a turn-left-up command.
    def TurnLeft45 (self, event = None):
        self.send_cmd ('q', "Left45")

    # Send the bot a turn-right command.
    def TurnRight (self, event = None):
        self.send_cmd ('s', "Right")

    # Send the bot a turn-right-up command.
    def TurnRight45 (self, event = None):
        self.send_cmd ('e', "Right45")

    # Send the bot a Forward command.
    def Forward (self, event = None):
        self.send_cmd ('w', "Up")

    # Send the bot a Reverse command.
    def Reverse (self, event = None):
        self.send_cmd ('z', "Down")

    # Send the bot a Halt command.
    def Halt (self, event = None):
        self.send_cmd ('h', "Halt!")

    # Set the gripper (X).
    def GripperX (self, event = None):
        # Read the slider control and send the value to the bot controller
        # Note: 0 is all the way closed and 180 is all the way open
        grp_change = ('>', chr (self.grip_horz.get ( )), chr (255))
        self.send_cmd (grp_change, "Gripper X")

    # Set the gripper Y.
    def GripperY (self, event = None):
        # Read the slider control and send the value to the bot controller
        # Note: 0 is all the way up and 180 is all the way down
        grp_change = ('^', chr (self.grip_vert.get ( )), chr (255))
        self.send_cmd (grp_change, "Gripper Y")

    # Set the gripper to the "home" position.
    def GripperHome (self, event = None):
        self.send_cmd (('c', chr (255)), "Gripper Home")

    def __init__ (self, master = None):
        Frame.__init__ (self, master)
        self.pack ( )
        self.createWidgets ( )
        self.CtrlButtonsState (DISABLED)

# Kick off the GUI (Tk) then size and title the app window
def main ( ):
    root = Tk ( )
    root.geometry ("450x350")
    root.wm_title ("Rover Control Center (RCC)")
    app = App (master = root)
    app.mainloop ( )

if __name__ == '__main__':
    main ( )
