/* #pragma pack(push) saves the packing in force, and pack(pop)
   restores it.  A header's push and pop around its own records, by
   the label a push names before its number or by none, leave the
   struct after them laid out as without the pragma; a packing of 1
   set before a push is in force again after the pop, and the struct
   declared there is refused by the pragma's words. */
#pragma pack(push, 2)
#pragma pack(pop)
#pragma pack(push, cryptoki, 1)
#pragma pack(pop, cryptoki)

struct plain
{
        char tag;
        int length;
};

#pragma pack(1)
#pragma pack(push, 4)
#pragma pack(pop)
struct header
{
        char tag;
        int length;
};

int
main(void)
{
        return sizeof(struct plain) - sizeof(struct header);
}
