/* backend.md chapter 5 and aarch64.md 5.3, a slot's displacement: the
   slots lie in declaration order, so a scalar declared after a large
   array lies far from the frame pointer, and whether the access
   encodes the displacement is the target's.  far() declares int
   pad[9216], 36864 bytes, before x, and near() declares x first: the
   order of declaration is the programmer's lever.

   On x86-64 the listing is one instruction per access in both
   functions, x at -36872(%rbp) in far() and at -8(%rbp) in near(),
   since a displacement takes any 32-bit number; the row is not
   reached there.

   On aarch64 the listing must show far()'s x at 36884, beyond the
   16380 bytes a 4-byte access reaches, each access through x16 in one
   more instruction, add x16, x29, #36864 and then str w0, [x16, #20]
   or ldr w0, [x16, #20]; and near()'s x at [x29, #20], one
   instruction each. */
int far(int a)
{
    int pad[9216];
    int x = a;

    return x + 1;
}

int near(int a)
{
    int x = a;
    int pad[9216];

    return x + 1;
}
