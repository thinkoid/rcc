/* A bit-field's width is an integer constant expression, 6.7.3.2p4. */
int n;

struct s
{
        int f : n;
};
