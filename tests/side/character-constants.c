/* The character constants of no prefix, A1.4 of the survey, 6.4.4.5:
   an int holding the value a char object holding the character has,
   so '\377' is -1 where plain char is signed, on x86-64, and 255 where
   it is unsigned, on aarch64. */
int
letter(void)
{
        return 'a';
}

int
null(void)
{
        return '\0';
}

int
newline(void)
{
        return '\n';
}

int
top_byte(void)
{
        return '\377';
}

int
hexadecimal(void)
{
        return '\x80';
}

int
quote(void)
{
        return '\'';
}

int
stored(void)
{
        char c;
        c = '\xff';
        return c;
}

int
compared(void)
{
        char c = 1;
        return c == '\xff';
}

long
widened(void)
{
        return '\377';
}
