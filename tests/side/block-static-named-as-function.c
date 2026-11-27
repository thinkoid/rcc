/* A static object named as its function: f.f, and the name hides the
   function in the block. */
int
f(void)
{
        static int f = 7;
        return f;
}
