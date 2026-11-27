/* The destinations of a comparison's value, and of !'s: the value is in
   the register its row took, and each destination reads it there.  A
   return finds it in the result register; x = a < b; and int x = a <
   b; store it from its register, and a < 5 is compared in place, so
   the setl takes the first free register; a < b; and !a; compute it
   and drop it.  A remainder compared under a return is in edx, and
   the setl writes eax, the register the return names, setl %al after
   cmpl -12(%rbp), %edx; stored, it writes edx.

   returned(), assigned(), initialized() and discarded() are each
   destination of a < b; returned_in_place() and assigned_in_place()
   of a < 5; returned_store() returns an assignment of a comparison;
   negation_assigned(), negation_initialized() and
   negation_discarded() are the destinations of !a; and
   remainder_returned() and remainder_assigned() compare a remainder. */
int
returned(void)
{
        int a = 1;
        int b = 2;
        return a < b;
}

int
assigned(void)
{
        int a = 1;
        int b = 2;
        int x;
        x = a < b;
        return x;
}

int
initialized(void)
{
        int a = 1;
        int b = 2;
        int x = a >= b;
        return x;
}

int
discarded(void)
{
        int a = 1;
        int b = 2;
        a < b;
        a == 5;
        return 0;
}

int
returned_in_place(void)
{
        int a = 1;
        return a < 5;
}

int
assigned_in_place(void)
{
        int a = 1;
        int x;
        x = a != 5;
        return x;
}

int
returned_store(void)
{
        int a = 1;
        int b = 2;
        int x;
        return x = a > b;
}

int
negation_assigned(void)
{
        int a = 1;
        int x;
        x = !a;
        return x;
}

int
negation_initialized(void)
{
        int a = 1;
        int x = !a;
        return x;
}

int
negation_discarded(void)
{
        int a = 1;
        !a;
        return a;
}

int
remainder_returned(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return a % b < c;
}

int
remainder_assigned(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int x;
        x = a % b < c;
        return x;
}
