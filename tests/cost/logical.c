/* Chapter 3, a && b and a || b: branches and conditional jumps.  The
   listing must show the left operand compared with 0 and a
   conditional jump past the right operand, the result for that case
   set before the jump; then the right operand loaded and compared in
   a block of its own, its setne the result.  It is the row that pins
   -disable-machine-sink: machine sinking would move the result set
   before the jump onto an edge of its own. */
int both(int a, int b)
{
    return a && b;
}

int either(int a, int b)
{
    return a || b;
}
