/* Chapter 3, a string literal: a private global array, not constant
   (G17).  The listing must show the literal's address taken with a
   rip-relative lea whose relocation is against .data, the section,
   since a private global has no symbol in the object; the address
   stored to s's slot and loaded back; the index 1 widened with
   movslq; the byte loaded with movzbl through (%rax,%rcx,1) and
   sign-extended with movsbl.  objdump -h shows the literal in .data,
   without READONLY: a writable section. */
int second(void)
{
    char *s = "abc";

    return s[1];
}
