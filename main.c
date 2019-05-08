#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int t = 0;
    int y = 0;
    char rules[64][64];
    for (int j = 0; j < argc; j++)
    {
        if (!strncmp(argv[j], "-f", 2) || !strncmp(argv[j], "-h", 2))
            t++;
        if (argv[j -1] && argv[j - 1][0] != '-')
        {
            strncpy(rules[y], argv[j], 64);
            y++;
        }
    }
    if (t != 0)
    {
        printf("This is the help for minimake\n");
        return 0;
    }
    for (int i = 0; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
            case 'f':
                if (argv[i][2] == 'h')
                {
                    printf("This is the help for minimake\n");
                    return 0;
                }
                return parse(argv[i + 1], rules);
                break;
            case 'h':
                printf("This is the help for minimake\n");
                return 0;
                break;
            default:
                int return_token = parse("makefile", rules);
                if (return_token != 0)
                    return parse("Makefile", rules);
                break;
            }
        }
    }
}
