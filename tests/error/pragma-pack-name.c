/* clang expands a macro in a #pragma pack, and clang -E writes the
   pragma with the macro unexpanded: the name after push may be a
   macro for the packing, as PACKING is here, or a label.  clang lays
   the struct out in 5 bytes; rcc cannot tell which packing is in
   force, so from the pragma on every struct is refused by its words,
   the one after the pop among them. */
#define PACKING 1

#pragma pack(push, PACKING)
struct header
{
        char tag;
        int length;
};
#pragma pack(pop)

int
main(void)
{
        return sizeof(struct header);
}
