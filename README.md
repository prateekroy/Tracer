                                         ######## Tracer  #########
Naive debugger to trace executables : Fully functional debugger for linux executables

make\
./run <executable-to-debug>\
break <inst-address>\
cont

DONE :
- able to break at multiple breakpoints based on address of instruction (objdump -d <executable>)

TODO : 
- add support to breakpoint at line number
- add support to breakpoint by funtion name
- print all register contents after hitting a breakpoint
- delete a breakpoint
- clear all breakpoint
