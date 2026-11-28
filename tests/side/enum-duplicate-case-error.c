/* Two cases of one value, the constants' values. */
enum e { A, B = 0 };

int
f(int x)
{
        switch (x) {
        case A: return 1;
        case B: return 2;
        }
        return 0;
}
