/* Where a spill's slot lies.  The frame's allocator gives a local its
   4 bytes and a spill its 8 at an alignment of 8, each past every slot
   taken before it, and gives none back.  So after b, at -8(%rbp), a
   spill takes -16, and c, declared after the spilling statement, takes
   -20; on aarch64, after b at #20, a spill takes #24 and c #32, and the
   frame grows to 48.  A second spilling statement takes new slots past
   the first one's, and c lands past both: -108 after two statements of
   six spills each, and #40 on aarch64.  An initializer's slot is taken
   before its value, so x lies at -8 and the spill at -16.  Three
   statements of six spills each pass the red zone's 128 bytes, and
   x86-64 moves rsp, subq $160, %rsp. */

int
declared_after(void)
{
        int a = 3;
        int b;
        b = a + (a + (a + (a + (a + (a + (a + (a + (a + (a + a)))))))));
        int c = 1;
        return c;
}

int
declared_after_fifteen(void)
{
        int a = 3;
        int b;
        b = a +
            (a +
             (a +
              (a +
               (a +
                (a +
                 (a + (a + (a + (a + (a + (a + (a + (a + (a + a))))))))))))));
        int c = 1;
        return c;
}

int
spilled_twice(void)
{
        int a = 3;
        int b;
        b = a + (a + (a + (a + (a + (a + (a + (a + (a + (a + a)))))))));
        b = a + (a + (a + (a + (a + (a + (a + (a + (a + (a + a)))))))));
        int c = 1;
        return c;
}

int
spilled_twice_fifteen(void)
{
        int a = 3;
        int b;
        b = a +
            (a +
             (a +
              (a +
               (a +
                (a +
                 (a + (a + (a + (a + (a + (a + (a + (a + (a + a))))))))))))));
        b = a +
            (a +
             (a +
              (a +
               (a +
                (a +
                 (a + (a + (a + (a + (a + (a + (a + (a + (a + a))))))))))))));
        int c = 1;
        return c;
}

int
initialized(void)
{
        int a = 3;
        int x = a + (a + (a + (a + (a + (a + (a + (a + (a + (a + a)))))))));
        return x;
}

int
past_red_zone(void)
{
        int a = 3;
        int b;
        b = a +
            (a +
             (a +
              (a +
               (a +
                (a +
                 (a + (a + (a + (a + (a + (a + (a + (a + (a + a))))))))))))));
        b = a +
            (a +
             (a +
              (a +
               (a +
                (a +
                 (a + (a + (a + (a + (a + (a + (a + (a + (a + a))))))))))))));
        b = a +
            (a +
             (a +
              (a +
               (a +
                (a +
                 (a + (a + (a + (a + (a + (a + (a + (a + (a + a))))))))))))));
        return b;
}
