/* c ? y : n over objects of static storage: defined, g, read through
   its label; only declared, h, through the address the GOT holds,
   loaded on the arm's own path; and a block's static.  The arms reach
   the objects through their handles, so the join is as for locals. */
int g = 5;
extern int h;

int
arms_defined_and_declared(void)
{
        int a = 1;
        return a ? g : h;
}

int
arms_stored(void)
{
        int a = 1;
        int c = 3;
        c = a ? h : g;
        return c;
}

int
condition_declared(void)
{
        return h ? 1 : 2;
}

int
arms_static(void)
{
        static int s = 3;
        int a = 1;
        return a ? s : h + 1;
}

int
arms_compared(void)
{
        int a = 1;
        if (a ? h < g : g)
                return 1;
        return 0;
}

int
arms_stores(void)
{
        int a = 1;
        a ? (h = 1) : (g = 2);
        return 0;
}

int
arm_remainder(void)
{
        int a = 1;
        int c = 3;
        c = a ? h % g : c;
        return c;
}

int
held_beside(void)
{
        int a = 1;
        int b = 2;
        return b + (a ? h : g);
}

int
global_assigned(void)
{
        int a = 1;
        int b = 2;
        g = a ? b : h;
        return g;
}
