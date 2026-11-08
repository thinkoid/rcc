/* G11: a misaligned access is performed. c holds 1 to 8, from argc;
   the int at c + 1 is read through a cast, at an odd address, by one
   movl: x86 has one mov for every alignment, so the guarantee takes
   no instruction of its own. Little endian, its low byte is c[1], 2,
   and its high byte c[4], 5: exit status 52. clang -O2 gives 52 as
   well: it forwards the stored bytes to the read and computes the
   value from argc, with no load. Not a divergence: on x86 a claimed
   alignment changes no scalar access, so G11 removes no fold a status
   can show. The program stays as the guarantee's only entry: it shows
   the load is made at the odd address and reads the bytes there. */
int main(int argc)
{
    char c[8];
    int i;
    int v;

    for (i = 0; i < 8; i++)
        c[i] = i + argc;

    v = *(int *) (c + 1);

    return (v >> 24) * 10 + (v & 255);
}
