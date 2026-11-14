/* A cast to an enum type, which is not yet. */
enum color { red, green };

int main(void)
{
    int c = 1;

    return (int) (enum color) c;
}
