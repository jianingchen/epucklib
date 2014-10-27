
Jianing Chen

j.n.chen@sheffield.ac.uk

==== embedded system library for e-puck (Work In Progress) ====

 - This is a library for the embedded system of the e-puck robot. 
 
 - This library currently only works on e-puck with the camera model PO6030K but not PO3030K.  
 
 - All source files starting with "e_" are from the official e-puck library, and attribute to whom ever own the authorship. 
 
 - Unlike the official e-puck lib (has several modules), this library used all-in-one design (one header path, one .a file). 
 
 - Support pretty much everything on the e-puck but the sound related and fft functionalities. 
 
 - A more user-friendly and potentially more powerful (40x15 @ ~20 fps) interface for the camera on the e-puck. 
 
 - Organize the development using three object-oriented concepts: _Timer_, _Process_ and _Trigger_. 

 - _Timer_ can be used to produce delay, as well as creating periodic callbacks (similar to the agenda in the official library)

 - _Process_ let you multi-tasking. So several loop with time delays can run concurrently (this achieved using cooperative multi-tasking > https://github.com/jianingchen/cmt ). 

 - A _Trigger_ can automatically launch a _Process_ once something happened, which let you do things only interrupts can do without dealing with hardware interrupts. 

 - The project file in the distribution is for MPLABX with xc16 compiler.
 
 