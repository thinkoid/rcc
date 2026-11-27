/* The controlling expression of a switch is a value, 6.8.5.3p1: a void
   call is none. */
void v(void);

int
f(void)
{
        int a = 0;
        switch (v()) {
        case 1: a = 1;
        }
        return a;
}
