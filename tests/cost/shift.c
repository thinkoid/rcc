/* backend.md 6.3 and aarch64.md 6.3, a << n and a >> n: a left shift,
   an arithmetic right shift for a signed left operand and a logical one
   for an unsigned one, each by a count in a register, the count masked
   by the instruction and by nothing else (G3).  backend.md 6.1 and
   6.5, the count computed in the register the shift names:
   count_extended() shifts by a call's char result, which the caller
   extends, count_compound() the same for <<= on a local, and
   count_postfix() by n++, whose old value is the count; beside them
   count_called() by a call's int result, and count_held() by a sum
   converted to char while rcx holds a live value.

   On x86-64 the listing must show a loaded into eax and the count into
   ecx, the register the instruction wants, then one shll %cl, %eax,
   sarl %cl, %eax or shrl %cl, %eax per function.  It must show the
   extension written into ecx, movsbl %al, %ecx, after the call in
   count_extended(), before a's reload and shll %cl, %eax, and in
   count_compound(), before shll %cl, -4(%rbp); in count_postfix() n
   loaded into edx and its old value copied into ecx, movl %edx, %ecx,
   before addl $1, %edx; and the moves kept, movl %eax, %ecx after the
   call in count_called(), and in count_held() b moved out of rcx,
   movq %rcx, %r8, after the extension in edi, movsbl %dil, %edi, and
   movl %edi, %ecx.

   On aarch64 the listing must show a loaded into w0 and the count
   into w1, the next free register, then one lsl w0, w0, w1,
   asr w0, w0, w1 or lsr w0, w0, w1 per function, and the count where
   its row leaves it: uxtb w0, w0 after bl narrow in count_extended()
   and count_compound(), w1 with add w2, w1, #1 in count_postfix(), w0
   after bl wide in count_called(), and uxtb w4, w4 in count_held(). */
int shl(int a, int n)
{
    return a << n;
}

int sar(int a, int n)
{
    return a >> n;
}

unsigned shr(unsigned a, int n)
{
    return a >> n;
}

char narrow(int);
int wide(int);

int
count_extended(int a, int x)
{
        return a << narrow(x);
}

int
count_compound(int a, int x)
{
        a <<= narrow(x);
        return a;
}

int
count_postfix(int a, int n)
{
        return a << n++;
}

int
count_called(int a, int x)
{
        return a << wide(x);
}

long
count_held(long a, long b, long c, int x, int n)
{
        return a + (b + (c + (x << (char) (n + 1))));
}
