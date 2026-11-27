/* !g() of each result type as a condition: the jump of g() on
   the other sense, at its own type. */
char result_char(void);
signed char result_signed_char(void);
unsigned char result_unsigned_char(void);
short result_short(void);
unsigned short result_unsigned_short(void);
int result_int(void);
unsigned result_unsigned(void);
long result_long(void);
unsigned long result_unsigned_long(void);
long long result_long_long(void);
unsigned long long result_unsigned_long_long(void);

int
negated_conditions_char(void)
{
        if (!result_char())
                return 1;
        return 0;
}

int
negated_conditions_signed_char(void)
{
        if (!result_signed_char())
                return 1;
        return 0;
}

int
negated_conditions_unsigned_char(void)
{
        if (!result_unsigned_char())
                return 1;
        return 0;
}

int
negated_conditions_short(void)
{
        if (!result_short())
                return 1;
        return 0;
}

int
negated_conditions_unsigned_short(void)
{
        if (!result_unsigned_short())
                return 1;
        return 0;
}

int
negated_conditions_int(void)
{
        if (!result_int())
                return 1;
        return 0;
}

int
negated_conditions_unsigned(void)
{
        if (!result_unsigned())
                return 1;
        return 0;
}

int
negated_conditions_long(void)
{
        if (!result_long())
                return 1;
        return 0;
}

int
negated_conditions_unsigned_long(void)
{
        if (!result_unsigned_long())
                return 1;
        return 0;
}

int
negated_conditions_long_long(void)
{
        if (!result_long_long())
                return 1;
        return 0;
}

int
negated_conditions_unsigned_long_long(void)
{
        if (!result_unsigned_long_long())
                return 1;
        return 0;
}
