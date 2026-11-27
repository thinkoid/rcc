/* Two parameters of each pair of widths: each slot of its type's size
   at its type's alignment, in parameter order, by the locals' rule:
   char then long is -1 and -16 on x86-64, #16 and #24 on aarch64. */
int
pair_c_c(char a, char b)
{
        return 0;
}

int
pair_c_s(char a, short b)
{
        return 0;
}

int
pair_c_i(char a, int b)
{
        return 0;
}

int
pair_c_l(char a, long b)
{
        return 0;
}

int
pair_s_c(short a, char b)
{
        return 0;
}

int
pair_s_s(short a, short b)
{
        return 0;
}

int
pair_s_i(short a, int b)
{
        return 0;
}

int
pair_s_l(short a, long b)
{
        return 0;
}

int
pair_i_c(int a, char b)
{
        return 0;
}

int
pair_i_s(int a, short b)
{
        return 0;
}

int
pair_i_i(int a, int b)
{
        return 0;
}

int
pair_i_l(int a, long b)
{
        return 0;
}

int
pair_l_c(long a, char b)
{
        return 0;
}

int
pair_l_s(long a, short b)
{
        return 0;
}

int
pair_l_i(long a, int b)
{
        return 0;
}

int
pair_l_l(long a, long b)
{
        return 0;
}
