/* __STDC_NO_ATOMICS__ is defined, as 1 (6.10.10.4p1): rcc drops
   _Atomic and makes no access atomic, so a program's branch for an
   implementation without atomic types is the one compiled.  The
   listing has no_atomics(), which returns the macro's value, and no
   atomics(). */
#ifdef __STDC_NO_ATOMICS__
int
no_atomics(void)
{
        return __STDC_NO_ATOMICS__;
}
#else
int
atomics(void)
{
        return 0;
}
#endif
