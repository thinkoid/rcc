/* A frame of 33 ints, 132 bytes, past the red zone's 128 on x86-64:
   the prologue moves rsp below the slots, rounded up to 16, subq $144,
   %rsp, and leave puts it back.  A frame of 32 ints, 128 bytes, stays
   in the red zone and writes no subq.  On aarch64 the frame is the
   16 bytes of the frame record and the slots, rounded up to 16: 144
   and 160. */
int
in_red_zone(void)
{
        int v0 = 0;
        int v1 = 1;
        int v2 = 2;
        int v3 = 3;
        int v4 = 4;
        int v5 = 5;
        int v6 = 6;
        int v7 = 7;
        int v8 = 8;
        int v9 = 9;
        int v10 = 10;
        int v11 = 11;
        int v12 = 12;
        int v13 = 13;
        int v14 = 14;
        int v15 = 15;
        int v16 = 16;
        int v17 = 17;
        int v18 = 18;
        int v19 = 19;
        int v20 = 20;
        int v21 = 21;
        int v22 = 22;
        int v23 = 23;
        int v24 = 24;
        int v25 = 25;
        int v26 = 26;
        int v27 = 27;
        int v28 = 28;
        int v29 = 29;
        int v30 = 30;
        int v31 = 31;
        return v31;
}

int
past_red_zone(void)
{
        int v0 = 0;
        int v1 = 1;
        int v2 = 2;
        int v3 = 3;
        int v4 = 4;
        int v5 = 5;
        int v6 = 6;
        int v7 = 7;
        int v8 = 8;
        int v9 = 9;
        int v10 = 10;
        int v11 = 11;
        int v12 = 12;
        int v13 = 13;
        int v14 = 14;
        int v15 = 15;
        int v16 = 16;
        int v17 = 17;
        int v18 = 18;
        int v19 = 19;
        int v20 = 20;
        int v21 = 21;
        int v22 = 22;
        int v23 = 23;
        int v24 = 24;
        int v25 = 25;
        int v26 = 26;
        int v27 = 27;
        int v28 = 28;
        int v29 = 29;
        int v30 = 30;
        int v31 = 31;
        int v32 = 32;
        return v32;
}
