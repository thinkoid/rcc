/* A continue statement, which is not yet. */
int main(void)
{
    int i = 0;

    while (i < 3) {
        i = i + 1;
        continue;
    }

    return i;
}
