# audioRead
Simple audio read with real time fft computation

This simple script collects audio from an input Mic pin 
performs real-time FFT and stores the spectrum in a buffer. 
This uses Particle framework (but should work with arduino (though i haven't tested it yet). 

In order to run the code:

from terminal go to the directory
`cd /audioRead`

For compiling:
`particle flash test .`

(where test is the particle short name, and . (the dot) means flash the current folder)
