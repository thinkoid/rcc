/* backend.md 7.3 and aarch64.md 7.3: fall-through, a default in the
   middle and a break.  The cases are 1, 2, 4 and 5: ncase 4, range 4,
   the direct form.  The body is written as it stands, each label where
   it is written and nothing between a case's statements and the next
   label: control falls from one case into the next, through the
   default's label, and leaves at the break by a jump to the label past
   the body.  The table sends 3, which no case holds, to the default,
   .Lscore.4, and the bound check sends what is outside 1 to 5 there as
   well.

   On x86-64 the listing must show the table .Lscore.2, .Lscore.3,
   .Lscore.4, .Lscore.5, .Lscore.6; the body's labels .Lscore.2 to
   .Lscore.6 in that order, each case's addl on r's slot after its
   label, with no jump between them; after .Lscore.5's addl $8, the
   break's jmp .Lscore.7; and .Lscore.7 before the load of r.

   On aarch64 the listing must show the same table and the same labels;
   each case's load, add and store of r after its label, with no branch
   between them; and after the add of 8 the break's b .Lscore.7. */
int
score(int n)
{
        int r = 0;

        switch (n) {
        case 1: r += 1;
        case 2: r += 2;
        default: r += 4;
        case 4: r += 8; break;
        case 5: r += 16;
        }

        return r;
}
