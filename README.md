# OpenTxT
Interpreter for OpenTxT "programming language". OpenTxT is my simplified take on Assembly language. It focuses on .txt files.

To execute .txt file in a normal mode use: "open (name without .txt)".
To execute .txt file in a debug mode use: "debug (name without .txt)".

You can write and read from four registers (integers) using 'a', 'b', 'c', 'd' (or 'reg1', 'reg2', 'reg3', 'reg4', 'r1', 'r2', 'r3', 'r4').
You can also read from four "system registers" (integers) using 'e', 'f', 'g', 'h' (or 'sreg1', 'sreg2', 'sreg3', 'sreg4', 'sr1', 'sr2', 'sr3', 'sr4').
System registers are updated with registers' values when exiting the file with "return" instruction.
Labels are made from the name and a semicolon, ex.: "label:".
'(number)' means an integer, register or system register.
Token is a type of a label created during program's execution and stored on the stack. Using 'jmpt' you can jump to the last token. You can create a token by adding 't' or 'token' argument to the end of each jump instruction line, for example "jge a b label token".
While writing to the opened file you can use '$' in front of register's name in order to implant the value of existing register into the file, ex.: "set a $a".


Instructions:
- open (name) - open the file, push it to the file stack and execute its instructions;
- remove (name) - remove the file;
- write (name) start (…) end write - open or create a text file, put cursor on the end of this file and start writing into it with text found between the "start" and the "end write" instructions;
- rewrite (name) start…end write - open or create a text file, remove its content and start writing into it with text found between the “start” and the "end write" instructions;
- "(text)" - print text found in the quotation marks on the console;
- endl - go to a new line;
- add (reg) (number) - add (number) to the register;
- sub (reg) (number) - substract (number) from the register;
- mul (reg) (number) - multiply the register by the (number);
- div (reg) (number) - divide the register by the (number);
- set (reg) (number) - set the value of the register to the (number);
- rand (reg) - set the value of the register to a random value;
- neg (reg) - negate the value in the register;
- print (reg) - return register's value to the stdout;
- return - close the file and set system registers to the values of closed file’s registers;
- input (reg) - wait for user's input and save it to the register;
- jmp (label) - jump to the (number);
- jge (reg) (number) (label) - jump to the (label) if the register's value is greater or equal to (number);
- jle (reg) (number) (label) - jump to the (label) if the register's value is lower or equal to (number);
- jg (reg) (number) (label) - jump to the (label) if the register's value is greater from (number);
- jl (reg) (number) (label) - jump to the (label) if the register's value is lower than (number);
- jex (file) (label) - jump to the (label) if the file exists;
- jnex (file) (label) - jump to the (label) if the file doesn’t exist;
- jmpt - jump to the last token on stack;
- sleep (number) - stop program for (number) miliseconds.
  

  
Example codes (.txt files) are in "examples" file.
