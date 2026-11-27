/* A switch's body, 6.8.5.3: fall through between cases, a break to the
   label past the body, a continue to the loop around the switch, a body
   of no block, of no case, or whose every case returns, a declaration
   before the first case, whose initializer is no path's, and a case label
   in a block or a loop inside the body. */
int o;
extern int e;

int
fall_through(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1: b = 1;
        case 2: b = b + 2; break;
        }
        return b;
}

int
break_last(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1: b = 1; break;
        case 2: b = 2; break;
        case 3: b = 3; break;
        }
        return b;
}

int
break_in_if(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1:
                if (b)
                        break;
                b = 1;
                break;
        case 2: b = 2;
        }
        return b;
}

int
break_in_if_else(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1:
                if (b)
                        break;
                else
                        b = 1;
        case 2: b = 2;
        }
        return b;
}

int
continue_in_while(int a)
{
        int b = 0;
        int c = 0;
        while (a) {
                switch (b) {
                case 1: a = a - 1; break;
                case 2: continue;
                }
                c = c + 1;
        }
        return c;
}

int
continue_in_for(int a)
{
        int b = 0;
        int c = 0;
        for (a = 0; a < 3; a = a + 1) {
                switch (b) {
                case 1: continue;
                case 2: break;
                }
                c = c + 1;
        }
        return c;
}

int
continue_in_do(int a)
{
        int b = 0;
        int c = 0;
        do {
                switch (b) {
                case 1: continue;
                default: c = c + 1;
                }
        } while (a);
        return c;
}

int
if_continue_in_for(int a)
{
        int b = 0;
        int c = 0;
        for (int i = 0; i < a; i = i + 1) {
                switch (i) {
                case 1:
                        if (b)
                                continue;
                        c = c + 1;
                }
        }
        return c;
}

int
switch_in_loop_break(int a)
{
        int b = 0;
        int c = 0;
        while (a) {
                switch (b) {
                case 1: break;
                }
                break;
        }
        return c;
}

int
no_block(int a)
{
        int b = 0;
        int c = 0;
        switch (a)
        case 1: b = 1; return b;
}

int
no_case(int a)
{
        int b = 0;
        int c = 0;
        switch (a)
                b = 1;
        return b;
}

int
default_only(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        default: b = 1;
        }
        return b;
}

int
default_only_broken(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        default: b = 1; break;
        }
        return b;
}

int
case_broken_only(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1: break;
        }
        return b;
}

int
empty_case(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1:
        }
        return b;
}

int
declaration(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
                int d;
        case 1:
                d = 1;
                b = d;
                break;
        }
        return b;
}

int
declaration_initialized(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
                int d = 5;
        case 1: b = d;
        }
        return b;
}

int
statement_before_case(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
                b = 1;
        case 1: c = 1;
        }
        return c;
}

int
case_in_block(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1: {
        case 2: b = 2;
        }
        }
        return b;
}

int
case_in_loop(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 0: do { b = b + 1;
                case 1: c = c + 1;
                } while (b < 3);
        }
        return c;
}

int
every_case_returns(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1: return 1;
        case 2: return 2;
        default: return 3;
        }
}

int
static_in_case(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1: {
                static int s = 4;
                b = s;
                break;
        }
        case 2: {
                static int s;
                s = a;
                b = s;
        }
        }
        return b;
}

int
object_in_case(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1: o = 1; break;
        case 2: b = e;
        }
        return b;
}
