/* backend.md chapter 8, arguments past the sixth: each argument is
   stored to its slot as evaluated, and the arguments past the sixth
   are pushed from their slots last first.  The native listing must
   show seven() with one stack argument over an 8-byte pad, subq $8,
   %rsp, the seventh pushed, the first six loaded into edi, esi, edx,
   ecx, r8d and r9d, the call and addq $16, %rsp; and eight() with two
   stack arguments and no pad, the eighth pushed before the seventh,
   and addq $16, %rsp.  An int slot is 4 bytes, so each stack argument
   is loaded into eax and pushed from rax. */
int f7(int, int, int, int, int, int, int);
int f8(int, int, int, int, int, int, int, int);

int seven(void)
{
    return f7(1, 2, 3, 4, 5, 6, 7);
}

int eight(void)
{
    return f8(1, 2, 3, 4, 5, 6, 7, 8);
}
