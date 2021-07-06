from cs50 import get_int
from math import floor


def main():
    digit1 = 0
    digit2 = 0
    count_digits = 0
    sum_of_double_odds = 0
    sum_of_evens = 0
    cc_code = get_int("Number: ")

    while cc_code > 0:
        digit2 = digit1
        digit1 = cc_code % 10

        if count_digits % 2 == 0:
            sum_of_evens += digit1
        else:
            multiple = 2 * digit1
            sum_of_double_odds += (multiple // 10) + (multiple % 10)

        cc_code //= 10
        count_digits += 1

    valid = (sum_of_evens + sum_of_double_odds) % 10 == 0
    first_two_digits = (digit1 * 10) + digit2

    if digit1 == 4 and count_digits >= 13 and count_digits <= 16 and valid:
        print("VISA\n")
    elif first_two_digits >= 51 and first_two_digits <= 55 and count_digits == 16 and valid:
        print("MASTERCARD\n")
    elif (first_two_digits == 34 or first_two_digits == 37) and count_digits == 15 and valid:
        print("AMEX\n")
    else:
        print("INVALID\n")


if __name__ == "__main__":
    main()
