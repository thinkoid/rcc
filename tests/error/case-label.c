/* A case label outside a switch, 6.8.2p2: an error. */
int main(void)
{
    case 1:
    return 0;
}
