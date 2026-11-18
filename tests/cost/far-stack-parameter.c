/* aarch64.md 5.2 and 5.3, a stack parameter past the add's reach: the
   prologue copies a parameter past the eighth from above the frame, at
   [x29, #T + 8(i - 8)].  A frame of 16777216 bytes less 16, the
   target's limit, puts the eleventh at 16 MiB, past the add's
   immediate of 12 bits shifted by 12, so the upper part of its
   displacement is held in x16 by the constant row and added.
   eleventh() returns its eleventh parameter, which the prologue copies
   to its slot; eleventh_register() takes it in a register variable,
   which the prologue loads straight into its register.  The program
   is compiled and assembled, not run: its frame is past the default
   stack limit.

   On x86-64 the listing must show the eleventh parameter read where
   the caller pushed it, movq 48(%rbp), %rax in eleventh() and
   movq 48(%rbp), %rbx in eleventh_register(), with no copy.

   On aarch64 the listing must show, after the frame is opened by
   sub sp, sp, #16773120 and sub sp, sp, #4080, the ninth and tenth
   parameters through add x16, x29, #16773120 and ldr x17, [x16, #4080]
   and [x16, #4088], and the eleventh through mov x16, #16777216 and
   add x16, x29, x16, then ldr x17, [x16] in eleventh() and
   ldr x19, [x16] in eleventh_register(). */
long
eleventh(long a,
         long b,
         long c,
         long d,
         long e,
         long f,
         long g,
         long h,
         long i,
         long j,
         long k)
{
        char pad[16777096];

        return k;
}

long
eleventh_register(long a,
                  long b,
                  long c,
                  long d,
                  long e,
                  long f,
                  long g,
                  long h,
                  long i,
                  long j,
                  register long k)
{
        char pad[16777096];

        return k;
}
