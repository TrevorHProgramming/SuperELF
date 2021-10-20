# MagicELF v0.4
A PS2 .ELF Modding Tool

Magic ELF (MELF) is a MIPS editor currently intended for use with Playstation 2 ELF files. MELF was made using QT libraries. 

<b>HOW TO USE</b>
MELF was designed using QT Creator 4.15, but should be buildable in any version from 4.15 onward. 

  1. Hit "Load," select a file, and the code should be visible in the main box to the left.
  2. Scroll through the code and find the section you plan to change.
  3. Type the new instruction in the box to the right. Syntax is as follows: 
    
    Instruction reg reg reg
  
  with no punctuation between registers. The Zero register will be recognized as "zero". <b> NOTE: </b> MELF does not currently support adding more lines to a given function than there were before. Doing so will break address references in both the code and data sections of your file. 
  
  4. Because of the above, for each line of code you add, delete a line by selecting it in the code browser and clicking "Delete."
  5. When your changes are complete, hit "Save" and choose or create your output file. This can be the same as your input file if you want to overwrite it.

<b>SAVING TO AN ISO</b>
MELF can save your file directly to an ISO if it contains the original version of this ELF. The current version of MELF searches for the header information of the ELF file in the ISO and replaces it from that point forward. Later versions will find the specified ELF in the ISO's file directory, allowing you to make changes to any part of the file.

<b>MAKING AND LOADING MODS</b>
To create a mod:
1. Make changes to the code in the browser as described above. 
2. Enter the address of the earliest line changed in the first column of the table in the bottom right and the address of the last line changed in the second column.
3. Multiple address ranges can be used, so if changes are made in multiple areas of the code, you can split them up as needed. This will make the mod file smaller and increase compatibility with other mods.
4. Click "Save Mod," then name and create your output file.

To load mods:
1. Load the ELF you plan to modify.
2. Click "Load Mods" and select the mod files you want to apply. Multiple mods can be applied at once as long as they don't change the same region of code.
3. When the changes are complete, click "Save," then name and create the modified ELF. This can be the same as your input file if you want to overwrite it. 
  
<b>PLANNED MAJOR UPDATES</b>
1. Add VU instructions.
2. Add system to update all function and address references when adding or removing code.

<b>PLANNED MINOR UPDATES</b>
1. Improve the UI to be resizable.
2. Add a settings menu to keep preferences between sessions.
