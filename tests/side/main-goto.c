/* main returns 0 where a path reaches the end of its body, 5.1.2.3.4p1:
   a label is reached by the goto before it, so movl $0, %eax and mov
   w0, #0 are written after it. */
int
main(void)
{
        int a;
        goto L;
L:;
}
