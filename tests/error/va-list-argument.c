/* An argument for a parameter of type __builtin_va_list, the type
   behind va_list: the function's declaration is accepted, and the
   argument's conversion to the parameter's type is not yet. */
int vcount(__builtin_va_list ap);

int main(void)
{
    return vcount(0);
}
