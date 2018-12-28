"""
Code to be run on a Raspberry Pi.
Plays piano notes based on serial input.

Expects each serial line to be of the form:
cdefgabc

...where each character is either 0 or 1, with no
spaces or spacers in between.

This code expects 8 inputs; if you're e.g. running on
an Arduino Uno and only have 6 inputs, then you should change
NUM_PINS below as well as the letters array to point to
the appropriate sample files.
"""

import serial
import time
import os
import pygame.mixer

DEBUG = False

class PianoStairs():

  def __init__(self):
    self.NUM_PINS = 26
    self.MAX_PLAY = 4
    self.played = 0

    # Keep track of the previous values so that we can do smoothing
    self.previnputs = [False] * self.NUM_PINS

    if not DEBUG:
      self.ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
      
    letters = [
      "c0", "d1", "e1", "f1", "g1", "a1", "b1",
      "c1", "d2", "e2", "f2", "g2", "a2", "b2",
      "c2", "d3", "e3", "f3", "g3", "a3", "b3",
      "c3", "d4", "e4", "f4", "g4", "a4", "b4",
    ]

    # remove this line if you plugged the sensors in top-to-bottom ;)
    # letters = letters[::-1]

    self.piano_notes = ["/home/pi/Documents/piano-stairs/samples/"+letter+".wav" for letter in letters]
    # self.piano_notes = ["C:\\Users\\Modegi\\Documents\\ovio\\piano-stairs\\samples\\"+letter+".wav" for letter in letters]
    
    pygame.mixer.init(44000, -16, 1, 1024) 
    self.sounds = [pygame.mixer.Sound(note) for note in self.piano_notes]
    self.channels = [pygame.mixer.Channel(i) for i in range(7)]

  def piano(self, i):
    self.channels[i%7].play(self.sounds[i])

  def run(self):
    # Sleep while we wait for everything to boot up.
    time.sleep(3)

    while True:
      line = ""

      if DEBUG:
        line = raw_input()
      else:
        line = self.ser.readline()
        
      try:
        self.piano(int(line))
      except ValueError:
        pass

      continue

if __name__ == "__main__":
  pianoStairs = PianoStairs()
  pianoStairs.run()

