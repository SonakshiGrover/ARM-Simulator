# ARM-Simulator
I have designed a function simulator in C for a subset of ARM instructions. Function simulator takes instruction encoding in MEM file as input. MEM file has the following format:  #First number is address of the instruction, followed by space, followed by instruction encoding.  
For example:  0x0 0xE3A0200A  0x4 0xE3A03000   
All the instruction in the given input MEM file is executed as per the functional behavior of the instructions. 
Each instruction goes through the following phases:  
-          Fetch 
-          Decode 
-          Execute  
-          Memory  
-          Writeback.  
Along with execution of instruction stages, simulator also provide messages what it is doing in each stage,Implements an additional instruction which exits the simulator, writes the data memory in MEM file before exiting.  
