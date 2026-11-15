/* clang's name of an SVE type, as an aarch64 bits/math-vector.h names
   it: the typedef is accepted, and a local, which needs the object's
   size, is not yet. */
typedef __SVBool_t sv_bool_t;

int
main(void)
{
        sv_bool_t b;

        return 0;
}
