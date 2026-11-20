/* A struct declared while #pragma pack(1) sets a packing, which
   changes its layout, as rcc does not honour yet: clang lays it out in
   5 bytes, and rcc refuses its size by the pragma's words.  pack()
   sets none, so the struct after it is laid out, and its size, taken
   first, is no error. */
#pragma pack(1)
struct header
{
        char tag;
        int length;
};
#pragma pack()

struct trailer
{
        char tag;
        int length;
};

int
main(void)
{
        return sizeof(struct trailer) - sizeof(struct header);
}
