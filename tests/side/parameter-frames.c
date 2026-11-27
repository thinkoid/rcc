/* The frames of functions that call nothing.  On x86-64 the frame is
   the slots below rbp, 4 bytes for each of the first six parameters
   and for each local, rounded up to 16: up to 128 bytes stay in the
   red zone with no subq, and 132 or 148 bytes move rsp, subq $144 and
   subq $160; the seventh and later parameters take no slot.  On
   aarch64 the frame is the record's 16 bytes and a slot for every
   parameter and local, rounded up to 16: 32 for one parameter, 48 for
   six and eight, 64 for nine and ten, 144 and 160 for six and 26 or
   27 locals, 176 for eight and 30 or 31. */
int
p1(int a)
{
        return a;
}

int
p6(int a, int b, int c, int d, int e, int f)
{
        return a;
}

int
p8(int a, int b, int c, int d, int e, int f, int g, int h)
{
        return h;
}

int
p10(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j)
{
        return a;
}

int
leaf6_26(int p1, int p2, int p3, int p4, int p5, int p6)
{
        int v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13;
        int v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25;
        int v26 = p1;
        return v26;
}

int
leaf6_27(int p1, int p2, int p3, int p4, int p5, int p6)
{
        int v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13;
        int v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25;
        int v26, v27 = p1;
        return v27;
}

int
leaf8_30(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8)
{
        int v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13;
        int v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25;
        int v26, v27, v28, v29, v30 = p8;
        return v30;
}

int
leaf8_31(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8)
{
        int v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13;
        int v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25;
        int v26, v27, v28, v29, v30, v31 = p8;
        return v31;
}
