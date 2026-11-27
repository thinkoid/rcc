/* A call's result of each type as the argument of a parameter of
   each type: the argument's slot taken first, then the call, its
   result's conversion with its extension where it widens, the store
   at the parameter's width and the load into its register. */
char result_char(void);
signed char result_signed_char(void);
unsigned char result_unsigned_char(void);
short result_short(void);
unsigned short result_unsigned_short(void);
int result_int(void);
unsigned result_unsigned(void);
long result_long(void);
unsigned long result_unsigned_long(void);
int takes_char(char x);
int takes_signed_char(signed char x);
int takes_unsigned_char(unsigned char x);
int takes_short(short x);
int takes_unsigned_short(unsigned short x);
int takes_int(int x);
int takes_long(long x);

int
char_passed_as_char(void)
{
        return takes_char(result_char());
}

int
char_passed_as_signed_char(void)
{
        return takes_signed_char(result_char());
}

int
char_passed_as_unsigned_char(void)
{
        return takes_unsigned_char(result_char());
}

int
char_passed_as_short(void)
{
        return takes_short(result_char());
}

int
char_passed_as_unsigned_short(void)
{
        return takes_unsigned_short(result_char());
}

int
char_passed_as_int(void)
{
        return takes_int(result_char());
}

int
char_passed_as_long(void)
{
        return takes_long(result_char());
}

int
signed_char_passed_as_char(void)
{
        return takes_char(result_signed_char());
}

int
signed_char_passed_as_signed_char(void)
{
        return takes_signed_char(result_signed_char());
}

int
signed_char_passed_as_unsigned_char(void)
{
        return takes_unsigned_char(result_signed_char());
}

int
signed_char_passed_as_short(void)
{
        return takes_short(result_signed_char());
}

int
signed_char_passed_as_unsigned_short(void)
{
        return takes_unsigned_short(result_signed_char());
}

int
signed_char_passed_as_int(void)
{
        return takes_int(result_signed_char());
}

int
signed_char_passed_as_long(void)
{
        return takes_long(result_signed_char());
}

int
unsigned_char_passed_as_char(void)
{
        return takes_char(result_unsigned_char());
}

int
unsigned_char_passed_as_signed_char(void)
{
        return takes_signed_char(result_unsigned_char());
}

int
unsigned_char_passed_as_unsigned_char(void)
{
        return takes_unsigned_char(result_unsigned_char());
}

int
unsigned_char_passed_as_short(void)
{
        return takes_short(result_unsigned_char());
}

int
unsigned_char_passed_as_unsigned_short(void)
{
        return takes_unsigned_short(result_unsigned_char());
}

int
unsigned_char_passed_as_int(void)
{
        return takes_int(result_unsigned_char());
}

int
unsigned_char_passed_as_long(void)
{
        return takes_long(result_unsigned_char());
}

int
short_passed_as_char(void)
{
        return takes_char(result_short());
}

int
short_passed_as_signed_char(void)
{
        return takes_signed_char(result_short());
}

int
short_passed_as_unsigned_char(void)
{
        return takes_unsigned_char(result_short());
}

int
short_passed_as_short(void)
{
        return takes_short(result_short());
}

int
short_passed_as_unsigned_short(void)
{
        return takes_unsigned_short(result_short());
}

int
short_passed_as_int(void)
{
        return takes_int(result_short());
}

int
short_passed_as_long(void)
{
        return takes_long(result_short());
}

int
unsigned_short_passed_as_char(void)
{
        return takes_char(result_unsigned_short());
}

int
unsigned_short_passed_as_signed_char(void)
{
        return takes_signed_char(result_unsigned_short());
}

int
unsigned_short_passed_as_unsigned_char(void)
{
        return takes_unsigned_char(result_unsigned_short());
}

int
unsigned_short_passed_as_short(void)
{
        return takes_short(result_unsigned_short());
}

int
unsigned_short_passed_as_unsigned_short(void)
{
        return takes_unsigned_short(result_unsigned_short());
}

int
unsigned_short_passed_as_int(void)
{
        return takes_int(result_unsigned_short());
}

int
unsigned_short_passed_as_long(void)
{
        return takes_long(result_unsigned_short());
}

int
int_passed_as_char(void)
{
        return takes_char(result_int());
}

int
int_passed_as_signed_char(void)
{
        return takes_signed_char(result_int());
}

int
int_passed_as_unsigned_char(void)
{
        return takes_unsigned_char(result_int());
}

int
int_passed_as_short(void)
{
        return takes_short(result_int());
}

int
int_passed_as_unsigned_short(void)
{
        return takes_unsigned_short(result_int());
}

int
int_passed_as_int(void)
{
        return takes_int(result_int());
}

int
int_passed_as_long(void)
{
        return takes_long(result_int());
}

int
unsigned_passed_as_char(void)
{
        return takes_char(result_unsigned());
}

int
unsigned_passed_as_signed_char(void)
{
        return takes_signed_char(result_unsigned());
}

int
unsigned_passed_as_unsigned_char(void)
{
        return takes_unsigned_char(result_unsigned());
}

int
unsigned_passed_as_short(void)
{
        return takes_short(result_unsigned());
}

int
unsigned_passed_as_unsigned_short(void)
{
        return takes_unsigned_short(result_unsigned());
}

int
unsigned_passed_as_int(void)
{
        return takes_int(result_unsigned());
}

int
unsigned_passed_as_long(void)
{
        return takes_long(result_unsigned());
}

int
long_passed_as_char(void)
{
        return takes_char(result_long());
}

int
long_passed_as_signed_char(void)
{
        return takes_signed_char(result_long());
}

int
long_passed_as_unsigned_char(void)
{
        return takes_unsigned_char(result_long());
}

int
long_passed_as_short(void)
{
        return takes_short(result_long());
}

int
long_passed_as_unsigned_short(void)
{
        return takes_unsigned_short(result_long());
}

int
long_passed_as_int(void)
{
        return takes_int(result_long());
}

int
long_passed_as_long(void)
{
        return takes_long(result_long());
}

int
unsigned_long_passed_as_char(void)
{
        return takes_char(result_unsigned_long());
}

int
unsigned_long_passed_as_signed_char(void)
{
        return takes_signed_char(result_unsigned_long());
}

int
unsigned_long_passed_as_unsigned_char(void)
{
        return takes_unsigned_char(result_unsigned_long());
}

int
unsigned_long_passed_as_short(void)
{
        return takes_short(result_unsigned_long());
}

int
unsigned_long_passed_as_unsigned_short(void)
{
        return takes_unsigned_short(result_unsigned_long());
}

int
unsigned_long_passed_as_int(void)
{
        return takes_int(result_unsigned_long());
}

int
unsigned_long_passed_as_long(void)
{
        return takes_long(result_unsigned_long());
}
