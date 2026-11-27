/* A break after a switch is in no loop and no switch. */
int
f(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1: b = 1;
        }
        break;
}
