#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>

int key;
char a;
char c;
char m;

int main (int argc, string argv[])
{
    if(argc != 2)
    {
        printf("Usage: ./caesar key\n");
        return 1;
    }

    else
    {
        string plain = get_string("Plaintext: ");
        printf("ciphertext: ");
        key = atoi(argv[1]);
        for (int i = 0, n = strlen(plain); i < n; i++ )
          {
             if(plain[i] <'A' || (plain[i] >'Z' && plain[i] <'a') || plain[i] >'z')
              {
                  printf("%c", plain[i]);
              }
             else
              {
                 if(islower(plain[i]))
                  {
                      plain[i] = plain[i] - 32;
                      a = plain[i] - 65;
                      c =(a + key) % 26;
                      m = c + 65 + 32;
                      printf("%c", m);
                  }
                 else
                  {
                      a = plain[i] - 65;
                      c = (a + key) % 26;
                      m =c + 65;
                      printf("%c", m);
                  }
            }
        }
        printf("\n");
    }
}
