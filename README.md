
Jianing Chen

j.n.chen@sheffield.ac.uk

==== e-puck lib ====

 - This is a interfacing library for the e-puck robot. 
 
 - All source files start with "e_" are borrowed from the official e-puck library. 
 
 - Unlike the official e-puck lib (has several modules), this library used all-in-one design. 
 
 - Support pretty much everything on e-puck but the sound related and fft functionalities. 
 
 - Three advanced concepts in the API: _Timer_, _Process_ and _Trigger_. 

 - _Timer_ can be used to produce delay, as well as creating periodic callbacks (similar to the agenda in the official library)
 - _Process_ let you multi-tasking. So several loop with delays can run concurrently (this is borrowed from my simple multi-task system > https://github.com/jianingchen/cmt ). 
 - A _Trigger_ can automatically launch a _Process_ once something happened, which let you do things only interrupts can do without dealing with hardware interrupts. 

