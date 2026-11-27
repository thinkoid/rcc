/* A switch's labels, backend.md 7.2: one for each case and default
   label in the order they are written, then the one past the body, all
   taken on reaching the switch, before the controlling expression's; the
   table's after the bounds check, and the buckets' after the hash's
   division; the body's own labels after all of these, a nested switch
   numbering its own when it is reached.  A C label and a goto in a switch
   are numbered as anywhere. */
int
truth_value_numbering(int a)
{
        int b = 0;
        int c = 0;
        switch (a && b) {
        case 0: c = 1; break;
        case 1: c = 2; break;
        }
        return c;
}

int
default_first(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        default: b = 0; break;
        case 1: b = 1; break;
        case 2: b = 2; break;
        }
        return b;
}

int
default_between(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1: b = 1; break;
        default: b = 0; break;
        case 2: b = 2; break;
        }
        return b;
}

int
if_in_case(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1:
                if (b)
                        c = 1;
                else
                        c = 2;
                break;
        case 2: c = 3;
        }
        return c;
}

int
loop_in_case(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1:
                while (b) {
                        if (c)
                                break;
                        b = b - 1;
                }
                break;
        case 2: b = 7;
        }
        return b;
}

int
nested(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1:
                switch (b) {
                case 1: c = 1; break;
                case 2: c = 2; break;
                }
                break;
        case 2: c = 3; break;
        }
        return c;
}

int
nested_same_value(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1:
                switch (c) {
                case 1: c = 2;
                }
        case 2: c = 3;
        }
        return c;
}

int
nested_defaulted(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1:
                switch (b) {
                default: c = 1;
                }
        case 2: c = 2;
        }
        return c;
}

int
goto_out_of_case(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1: goto out;
        case 2: b = 2;
        }
out:
        return b;
}

int
goto_into_case(int a)
{
        int b = 0;
        int c = 0;
        goto in;
        switch (a) {
        case 1:
                b = 1;
        in:
                b = b + 1;
                break;
        case 2: b = 2;
        }
        return b;
}

int
label_before_case(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        L:
        case 1:
                b = b + 1;
                if (b < 3)
                        goto L;
                break;
        default: b = 9;
        }
        return b;
}

int
goto_to_case_label(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1: b = 1; break;
        case 2: goto done;
        default: b = 3;
        }
done:
        return b;
}

int
two_switches(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1: b = 1; break;
        case 2: b = 2;
        }
        switch (b) {
        case 1: c = 1; break;
        case 2: c = 2;
        }
        return c;
}
