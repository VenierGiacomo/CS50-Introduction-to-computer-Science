#include<stdio.h>
#include <cs50.h>

int main(void)
{
    int n;
do
{ 
    n=get_int("Height: ");
} while(n<1 || n>8);
{
	for(int i=0;i<n;i++)
	 {
	 	for(int j=0;j<n-i-1;j++)
	 	printf(" ");
	 	{
	 	   for(int k=0;k<1+i;k++)
		    printf("#");
		    printf("  ");
	        {
                for(int m=0;m<1+i;m++)
  	            printf("#");
  	           
  	                //for(int l=0;l<n-i-1;l++)
	               // printf(" ");	
	           
	            printf("\n");              	
	        }
        }
    }
}
}
