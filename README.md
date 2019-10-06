## Description
MISC F-4 processor implementation described http://www.dakeng.com/misc.html

## Installation
<code>make</code>

## Usage
### [C Library](f4.h) for embeded purposes
### Command line interface: **f4ci**
* Interactive mode 
  * [Commands](f4cli.c#L29) 
  * <code>./f4cli *code_size*</code> ; *code_size* - the size of memory allocated to code in [16 bit words](f4cli.c#L91).
* Non interactive mode <code> cat CPU_Instruction_File.ram | f4cli </code> 
