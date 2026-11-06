/* Chapter 3, ++p and p -= n.  ++p is a gep by the i64 constant 1 and
   p -= 2 a gep by the negated index, the constant 2 taken as an i64
   by the parser and subtracted from 0 by the emitter.  The listing
   must show ++p as add $0x4; and p -= 2 as the 2 negated by xor and
   sub $0x2, scaled by shl $0x2 and added: the constant is an
   immediate, and the negation the source wrote happens at run
   time. */
int *step(int *p)
{
    ++p;
    p -= 2;
    return p;
}
