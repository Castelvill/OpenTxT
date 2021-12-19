# OpenTxT
Assembler but bad, but with easy way to work with txt files. It's a mess.

To execute txt file in a normal mode use: "boot (name without .txt)".
To execute txt file in a debug mode use: "debug (name without .txt)".

You can write and read from four registers (integers) using 'a', 'b', 'c', 'd' (or 'reg1', 'reg2', 'reg3', 'reg4', 'r1', 'r2', 'r3', 'r4').
You can also read from four "system registers" (integers) using "sreg1", "sreg2", "sreg3", "sreg4" (or "sr1", "sr2", "sr3", "sr4").
System registers are updated with registers' values when exiting the file with "return" instruction.
Labels are made from the name and a semicolon, ex.: "label:".
By (number) I mean integer, register or system register. 


Instructions:
- open (name) - open the file, push it to the file stack and execute its instructions;
- remove (name) - remove the file;
- write (name) start (…) end write - open or create a text file, put cursor on the end of this file and start writing into it with text found between the "start" and the "end write" instructions;
- rewrite (name) start…end write - open or create a text file, remove its content and start writing into it with text found between the “start” and the "end write" instructions;
- "(text)" - write text found in the quotation marks on the stdout;
- add (reg) (number) - add <number> to the register;
- sub (reg) (number) - substract <number> from the register;
- mul (reg) (number) - multiply the register by the <number>;
- div (reg) (number) - divide the register by the <number>;
- set (reg) (number) - set value of the register to the <value>;
- neg (reg) - negate the value in the register;
- print (reg) - return register's value to the stdout;
- return - close the file and set system registers to the values of closed file’s registers;
- input (reg) - wait for user's input and save it to the register;
- jmp (label) - jump to the label;
- jge (reg) (number) (label) - jump to the <label> if the register's value is greater or equal to <number>;
- jle (reg) (number) (label) - jump to the <label> if the register's value is lower or equal to <number>;
- jg (reg) (number) (label) - jump to the <label> if the register's value is greater from <number>;
- jl (reg) (number) (label) - jump to the <label> if the register's value is lower than <number>;
- jex (file) (label) - jump to the <label> if the file exists;
- jnex (file) (label) - jump to the <label> if the file doesn’t exist.

  
Example code is in "examples" file.
