/* A #pragma pack where a list opens changes its layout, which rcc does
   not honour yet, refused at the first use that needs the layout. */
#pragma pack(1)
struct s
{
        char c;
        int i;
};
#pragma pack()
int
f(struct s *p)
{
        return p != 0;
}

unsigned long
g(void)
{
        return sizeof(struct s);
}
