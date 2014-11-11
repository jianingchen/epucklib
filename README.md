
Jianing Chen

j.n.chen@sheffield.ac.uk

embedded system library for e-puck 
==================================

(Work In Progress)

![GitHub Logo](/el/el_logo_heading.png)

### Hightlights: 

 - This is a library for developing the embedded system of the e-puck robot. 
 
 - Unlike the standard e-puck lib (has several modules), this library used 
 all-in-one design (one header path, one .a file). 
 
 - The online documentation page of this library is 
 http://jianingchen.github.io/epucklib/html/.
 
 - Support pretty much everything on the e-puck but the sound related and fft 
 functionalities. 
 
 - A user-friendly and fast (40x15 @ ~18.4 fps) interface for e-puck's camera.  
 
 - Multi-tasking through _Process_ --- so several loop with time delays can run 
 concurrently. 

 - Organize the development using two object-oriented concepts: 
 _Timer_ and _Trigger_. 

 - _Timer_ can be used to produce delay, as well as creating periodic callbacks 
 (similar to the agenda in the official library)

 - A _Trigger_ can automatically launch a _Process_ once something happened, 
 which let you do things only interrupts can do without dealing with hardware 
 interrupts. 


### Environment:

 - The project format in the distribution is for MPLAB X.
 
 - This library has been developed and tested using XC16 compiler. 
 
 - This library currently only works on e-puck with the camera model PO6030K 
 but NOT PO3030K. 



### License: 

 - All source files starting with "e_" are from the official e-puck library, 
 and attribute to whom ever own the copyright of them. 
 
 - This library is released under the terms of the MIT license:

    The MIT License (MIT)

    Copyright (c) 2014 Jianing Chen

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

