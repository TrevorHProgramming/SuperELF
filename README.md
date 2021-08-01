# MagicELF v0.2
A PS2 .ELF Modding Tool

Magic ELF (MELF) is a MIPS editor currently intended for use with Playstation 2 ELF files. MELF was made using QT libraries. 

<b>HOW TO USE</b>
MELF was designed using QT Creator 4.15, but should be buildable in any version from 4.15 onward. 

1. Enter the path of the ELF file you intend to edit in the "File in" box.
  2. Hit "Load" and the code should be visible in the main box to the left.
  3. Scroll through the code and find the section you plan to change.
  4. Type the new instruction in the box to the right. Syntax is as follows: 
    
    Instruction reg reg reg
  
  with no punctuation between registers. The Zero register qill be recognized as "zero". <b> NOTE: </b> MELF does not currently support adding more lines to a given function than there were before. Doing so will break address references in both the code and data sections of your file. 
  
  5. Because of the above, for each line of code you add, delete a line by selecting it in the code browser and clicking "Delete."
  6. When your changes are complete, enter the path of your output file in the "File out" box. This can be the same as your input file if you want to overwrite it.
  
<b>PLANNED MAJOR UPDATES</b>
1. Add VU instructions.
2. Add system to update all function and address references when adding or removing code.

<b>PLANNED MINOR UPDATES</b>
1. Improve the UI to be resizable, more akin to a full IDE. 
2. Add a settings menu to keep preferences between sessions.
