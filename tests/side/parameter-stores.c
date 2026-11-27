/* A parameter of each width stored and never read: the store from its
   argument register at its width all the same, movw %di, -2(%rbp) and
   strh w0, [x29, #16]; and a constant stored to it at its width, movw
   $5, -2(%rbp), and mov w0, #5 and strh w0, [x29, #16]. */
int
unread_char(char a)
{
        return 0;
}

int
unread_short(short a)
{
        return 0;
}

int
unread_int(int a)
{
        return 0;
}

int
unread_long(long a)
{
        return 0;
}

int
stored_char(char a)
{
        a = 5;
        return 0;
}

int
stored_short(short a)
{
        a = 5;
        return 0;
}

int
stored_int(int a)
{
        a = 5;
        return 0;
}

int
stored_long(long a)
{
        a = 5;
        return 0;
}
