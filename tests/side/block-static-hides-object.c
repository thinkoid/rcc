/* A block's static hides an object at file scope of its name: s and
   f.s are two objects. */
int s = 1;
int
f(void)
{
        static int s = 2;
        return s;
}
