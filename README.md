# Muyuchina Code
This code is to aid in the automatic translation of the Quechua Verb Wheel, or Muyuchina. 
There are three executables that this code can build:

### data_collector
This executable is meant to be run first, and initializes the sensors to be able to read input from the wheel. 
It will also report the accuracy of its readings, so the user can judge if there is an acceptable margin of error.

### driver
This is the main program. After the data_collector is finished, this can be run to continuously display translations from the wheel.

### tester
This executable can be compiled outside of the Raspberry Pi or on it, (it does not depend on the WiringPi library)
It will generate a random Quechua word and display its translation. It does not guarantee a translatable word, and so running it a few
times can give the user a sense of the chance of getting a translatable word at random.

