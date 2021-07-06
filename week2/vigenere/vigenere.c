#include <cs50.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, string argv[])
{    
    if (argc != 2)
    {
        printf("Usage: ./vigenere keyword\n");
        
        return 1;
    }
    else 
    {
        for (int ind = 0, n = strlen(argv[1]); ind < n; ind++)  //strlen() function calculates the length of a given string
        {
            if (!isalpha(argv[1][ind]))   //isalpha() function checks whether a character is an alphabet (a to z and A-Z) or not
            {
                printf("Usage: ./vigenere keyword\n");
                
                return 1;
            }    
        }
    }
    
    string k = argv[1];
    int kLen = strlen(k);  //strlen() function calculates the length of a given string
    
    string Plaintext = get_string("Plaintext: ");
    char cipher[100];
    for (int ind = 0, j = 0, n = strlen(Plaintext); ind < n; ind++)
    {            
       int letterKey = tolower(k[j % kLen]) - 'a';  //tolower() it returns the same character
        
        if (isupper(Plaintext[ind]))     //isupper() function checks whether a character is an uppercase alphabet (A-Z) or not
        {
            cipher[ind] = 'A' + ((Plaintext[ind] - 'A' + letterKey) % 26);
            
            j++;
        }
        else if (islower(Plaintext[ind]))    //islower() function checks whether a character is lowercase alphabet (a-z) or not
        {
            cipher[ind] = 'a' + ((Plaintext[ind] - 'a' + letterKey) % 26);
            j++;
        }
        else
        {
            cipher[ind] = Plaintext[ind];
        }
    }
    printf("ciphertext: %s", cipher);
    printf("\n");
    
    return 0;
}
