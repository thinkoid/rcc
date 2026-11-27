/* A goto ends at its ';', which is said missing at the token found in
   its place, here the next label's name. */
int
main(void)
{
        goto L L: return 0;
}
