/* The smallest chains nested on the right that spill.  Each level's
   left operand is held while its right one is generated; when every
   expression register holds a value, the oldest is stored, all 8
   bytes, to a new slot of the frame, and reloaded where it is consumed.
   x86-64 holds 9 values, and the innermost a + a one, its right a the
   operand in memory: 10 additions spill eax, movq %rax, -16(%rbp), and
   the outermost one reloads it into eax, the register a return names.
   15 spill six values, and the outer levels read five of them in their
   slots, addl -56(%rbp), %r9d.  aarch64 holds 15, and the innermost
   a + a two: 15 additions spill x0, str x0, [x29, #24]; 16 spill x0 and
   x1, and the last addition writes w0 from the second reload, add w0,
   w1, w0, w0 being the register a return names.  On x86-64 an
   instruction that pins a register spills sooner: a division under 7
   additions moves eax out to r11, the last free register, then spills
   it there to free edx; a shift under 7 spills eax to free ecx, and the
   outermost addition reads it in its slot, addl -16(%rbp), %eax.  A
   chain of divisions holds its dividends outside eax and edx and spills
   at 9 operators, the spilled dividend reloaded into eax, movq
   -16(%rbp), %rax; a chain of shifts holds its values outside ecx, and
   reloads a spilled one outside it.  On aarch64 a remainder takes a
   third register for its quotient: 13 additions over a % a spill x0
   for it, str x0, [x29, #24], and sdiv w0, w14, w15.

   Each function returns one such chain, its name what it shows. */

int
spill_ten(void)
{
        int a = 3;
        return a + (a + (a + (a + (a + (a + (a + (a + (a + (a + a)))))))));
}

int
spill_fifteen(void)
{
        int a = 3;
        return a +
               (a +
                (a +
                 (a +
                  (a +
                   (a +
                    (a +
                     (a + (a + (a + (a + (a + (a + (a + (a + a))))))))))))));
}

int
spill_sixteen(void)
{
        int a = 3;
        return a +
               (a +
                (a +
                 (a +
                  (a +
                   (a +
                    (a +
                     (a +
                      (a + (a + (a + (a + (a + (a + (a + (a + a)))))))))))))));
}

int
quotient_under_sums(void)
{
        int a = 3;
        return a + (a + (a + (a + (a + (a + (a + (a / a)))))));
}

int
shift_under_sums(void)
{
        int a = 3;
        return a + (a + (a + (a + (a + (a + (a + (a << a)))))));
}

int
remainder_under_sums(void)
{
        int a = 3;
        return a +
               (a +
                (a +
                 (a +
                  (a +
                   (a + (a + (a + (a + (a + (a + (a + (a + (a % a)))))))))))));
}

int
quotients(void)
{
        int a = 3;
        return a / (a / (a / (a / (a / (a / (a / (a / (a + a))))))));
}

int
shifts(void)
{
        int a = 3;
        return a << (a << (a << (a << (a << (a << (a << (a << (a << a))))))));
}
