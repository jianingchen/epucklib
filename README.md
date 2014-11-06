
Jianing Chen

j.n.chen@sheffield.ac.uk

==== embedded system library for e-puck (Work In Progress) ====

 - This is a library for developing the embedded system of the e-puck robot. 
 
 - This library currently only works on e-puck with the camera model PO6030K but not PO3030K. 
 
 - All source files starting with "e_" are from the official e-puck library, and attribute to whom ever own the authorship. 
 
 - Unlike the official e-puck lib (has several modules), this library used all-in-one design (one header path, one .a file). 
 
 - The online documentation page of this library is http://jianingchen.github.io/epucklib/html/.
 
 - Support pretty much everything on the e-puck but the sound related and fft functionalities. 
 
 - A more user-friendly and potentially faster (40x15 @ ~18.4 fps) interface for e-puck's camera. 
 
 - Multi-tasking through _Process_ --- so several loop with time delays can run concurrently. 

 - Organize the development using two object-oriented concepts: _Timer_ and _Trigger_. 

 - _Timer_ can be used to produce delay, as well as creating periodic callbacks (similar to the agenda in the official library)

 - A _Trigger_ can automatically launch a _Process_ once something happened, which let you do things only interrupts can do without dealing with hardware interrupts. 

 - The project format in the distribution is for MPLABX.
 
 - This library has been developed using XC16 compiler. 
 