/* A typedef through a marking attribute among the declaration
   specifiers, as an aarch64 bits/math-vector.h declares its vector
   types: the typedef is accepted and names a type of its own, and a
   local of it is unsupported.  A dropped attribute there changes
   nothing. */
typedef __attribute__((__neon_vector_type__(4))) float f32x4_t;

__attribute__((__nothrow__)) int count(int n);

int
main(void)
{
        f32x4_t v;

        return count(0);
}
