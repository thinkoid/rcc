/* An alias of a function the unit declares and does not define: the
   alias is a symbol of the target's address, which the assembler gives
   it from the target's definition in the unit, and with none rcc does
   not write it yet.  It is refused where it is declared, though no use
   names it. */
int fgcmd(void);
int bgcmd(void) __attribute__((__alias__("fgcmd")));
