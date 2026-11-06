/* Chapter 3, ++p and p -= n.  ++p is a gep by the i64 constant 1 and
   p -= 2 a gep by the negated index, the int 2 widened and subtracted
   from 0 by the emitter.  The listing must show ++p as add $0x4; and
   p -= 2 as the 2 moved into a register and widened with movslq,
   negated by xor and sub, scaled by shl $0x2 and added: the constant
   is converted and negated at run time, like every constant
   operand. */
int *step(int *p)
{
    ++p;
    p -= 2;
    return p;
}
