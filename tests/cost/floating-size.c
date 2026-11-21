/* shape.md chapter 3: a real floating type carries clang's size and
   alignment, though an object of it is not yet, so sizeof and _Alignof
   of the type, or of an array of it, fold to constants: 4 for float, 8
   for double, 16 for long double, 24 for double [3].  sizeof of a
   member of the type, though its access is not yet, folds the same: 8
   for a member of type double; and so does sizeof of a named object of
   the type, though its use is not yet: 8 for an extern double.

   On x86-64 the listing must show each function as the constant
   moved into %rax and no other instruction of the body.

   On aarch64 the listing must show each function as mov x0 of the
   constant and no other instruction of the body. */
unsigned long
float_size(void)
{
        return sizeof(float);
}

unsigned long
double_size(void)
{
        return sizeof(double);
}

unsigned long
long_double_size(void)
{
        return sizeof(long double);
}

unsigned long
double_array_size(void)
{
        return sizeof(double[3]);
}

unsigned long
double_alignment(void)
{
        return _Alignof(double);
}

unsigned long
long_double_alignment(void)
{
        return _Alignof(long double);
}

struct sample
{
        int count;
        double d;
};

extern struct sample sample;

unsigned long
double_member_size(void)
{
        return sizeof sample.d;
}

extern double reading;

unsigned long
double_object_size(void)
{
        return sizeof reading;
}
