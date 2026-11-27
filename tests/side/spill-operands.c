/* A spilled value as an operand.  On x86-64 the instruction of + * &
   | ^ reads a spilled left operand in its slot and writes the right
   operand's register, xorl -16(%rbp), %ecx, where the consumer names no
   register, as an assignment's and an expression statement's do; a
   return names eax, and the spill is reloaded there.  - does not
   commute, so its left operand is reloaded: two spills of a chain of
   differences are reloaded in turn, movq -24(%rbp), %rcx and movq
   -16(%rbp), %rax, the right operand of the outermost one generated
   with eax left for last, so that the first reload takes ecx and the
   second eax.  A spill under - is reloaded for the chain's last
   addition, which the negation then works on: negl %eax; on aarch64,
   after two spills, neg w0, w1 writes the register a return names from
   the second reload's.  A left operand of & that a shift's count
   divides after it is reloaded, movq -24(%rbp), %rax, and not read in
   its slot, since its right operand stands in edx, which the division
   ahead would move: the instruction keeps its destination out of the
   registers a pinned instruction generated later would move a value
   out of.

   Each function shows one, its name what it shows. */

int
read_in_slot(void)
{
        int a = 3;
        int b;
        b = a ^ (a + (a + (a + (a + (a + (a + (a + (a + (a + a)))))))));
        return b;
}

int
reloaded_difference(void)
{
        int a = 3;
        return a -
               (a - (a - (a - (a - (a - (a - (a - (a - (a - (a - a))))))))));
}

int
negated(void)
{
        int a = 3;
        return -(
            a +
            (a +
             (a +
              (a +
               (a +
                (a +
                 (a +
                  (a +
                   (a + (a + (a + (a + (a + (a + (a + (a + a))))))))))))))));
}

int
discarded(void)
{
        int a = 3;
        a + (a + (a + (a + (a + (a + (a + (a + (a + (a + a)))))))));
        return a;
}

int
pinned_ahead(void)
{
        int a = 3;
        int b = 5;
        int c = 7;
        return (a & (a + (a + (a + (a + (a + (a + (a + (a + (a + a)))))))))) >>
               -(b % c);
}
