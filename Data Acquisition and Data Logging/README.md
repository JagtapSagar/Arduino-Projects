Assignment 3: Data Acquisition and Data Logging
===============================================


The objective of this project is to build a data logger for digital and analog measurements using a SD card to record four channels of data over time. The program will operate in one of three modes. Upon power up, the program will prompt for a key stroke (“Choose mode: 1 / 2 / 3:”) over a serial connection, to select operation mode:

**Mode 1 – data logger**

Data logging of a digital sensor and an analog signal (either from a signal generator, a potentiometer or your choice of analog source). The data must be sampled at 40 Hz for 10 seconds. You MUST use a timer INTERRUPT to create a constant time base. The data must be stored as a text file with the following format:
<p align="center">Channel 1 (Signal Generator) &nbsp; &nbsp; &nbsp; 	Channel 2 (Digital compass (I2C))<br/>##.###&nbsp; &nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; DX, DY, DZ<br/>##.###&nbsp; &nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; DX, DY, DZ</p>
Use the 10 bit on-chip ADC to acquire the analog channel. Store the converted values to the SD card as floating point numbers that can be read to a plotting software such as Matlab or Excel. At the end of data collection, the program goes back to the initial prompt (“Choose mode:  1 / 2 / 3 :”) 

**Mode 2 – volt meter**

Analog voltage data is acquired from your analog source. The readings will be sent to a display or terminal window to mimic a 5-digit voltmeter:
<p align="center">Channel 1 (Volts)<br/>##.###</p>
The refresh rate is 100 msec. The program remains in mode 2 until any key is pressed on the keyboard. After a key is pressed, the program goes back to the initial prompt: (“Choose mode:  1 / 2 / 3 :”). <br/><br/> 
**Mode 3 – data retrieval**

When selecting Mode 3, the data file saved to the SD card will be retrieved to a PC terminal window as a text file. The text file will be plotted using software such as Matlab or Excel. The plotting software will load the text file and produce a plot with four traces: voltage (Ch. 1) vs. time and digital data channels  X, Y, Z (Ch.2) vs. time 
