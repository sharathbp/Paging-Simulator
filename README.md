# Paging-Simulator

An OpenGL application that demonstrate various paging algorithm such as FIFO, LRU and Optimal Page Replacement.

## Requirements

For Windows:
- Visual Studio or MinGW Compiler

For Linux:
- gcc compiler and make

## Dependencies

- freeglut (https://www.transmissionzero.co.uk/software/freeglut-devel/)
- glui (https://www.cs.unc.edu/~rademach/glui/)

## Installation

For Ubuntu : 
 - sudo apt install freeglut3 freeglut3-dev 
 - sudo apt install libglui2c2 libglui-dev             

Tested on Ubuntu (gcc), Visual Studio (vc++) and MinGW Compiler

## Build commands

For windows : make -f makefile.win

For linux: make -f Makefile

## Preview
![Paging](https://user-images.githubusercontent.com/32369302/67699668-81f0c880-f9d2-11e9-93ce-a0121d600840.gif)

## Controls
After Pressing start use:

'enter' : moves to next page in reference id

's' : searches for replacement page


