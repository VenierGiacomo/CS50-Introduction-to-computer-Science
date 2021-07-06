#include <stdio.h>
#include<cs50.h>
#include<math.h>

int main(void) 
{
  long int card_num ;
  card_num=get_long("Number: ") ;
  int digit=0;
  int count=0;
  int sum_first=0;
  int sum_second=0;
  long long temp_no=card_num ;
  long long company=card_num ;
  while(card_num!=0)
    {
      digit=card_num%10;
      if(count%2==0)
      sum_first=sum_first+digit ;
      else
        {
      	  int m=digit*2 ;
		  if(m>=10)
      	  sum_second=sum_second+(m%10)+1 ;
		  else
		  sum_second=sum_second+m ;
        }
      card_num=(card_num-digit)/10;
      count++;
    }
      int total_sum=sum_first+sum_second ;
      if(total_sum%10!=0)
        {
          printf("INVALID\n");
        }
      if(total_sum%10==0)
        {
        	while(company>100)
            {
              company=company-(company%10) ;
              company = company/10;
            }
          if((company > 50 && company < 56) && (count==16))
            {
              printf("MASTERCARD\n");
            }
          else if((company >39 && company < 50) && (count==13 || count==16))
            {
              printf("VISA\n");
            }
          else if((company==34 || company==37) && (count==15))
            {
              printf("AMEX\n");
            }
          else 
            {
              printf("INVALID\n");
            }
        }
      return 0;
}
