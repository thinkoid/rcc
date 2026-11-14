/* A call with 513 arguments past the eighth, whose outgoing area of
   4112 bytes the 12-bit immediate of aarch64's sub does not encode:
   not yet there.  x86-64 compiles it. */
#define A8 0, 0, 0, 0, 0, 0, 0, 0
#define A64 A8, A8, A8, A8, A8, A8, A8, A8
#define A512 A64, A64, A64, A64, A64, A64, A64, A64

int f(int a, ...);

int main(void)
{
    return f(0, A8, A512);
}
