/* A call of a function a marking attribute among the declaration
   specifiers marks, as an aarch64 math.h declares its vector functions:
   the declaration is accepted, and the use is unsupported. */
__attribute__((__aarch64_vector_pcs__)) int twice(int x);

int
main(void)
{
        return twice(1);
}
