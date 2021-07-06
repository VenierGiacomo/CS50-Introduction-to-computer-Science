from cs50 import get_float
from math import floor


def main():
    while True:
        int_owed = get_float("Change owed: ")
        cent_owed = floor(int_owed * 100)

        if cent_owed > 0:
            break

    quarters = cent_owed // 25
    dimes = (cent_owed % 25) // 10
    nickels = ((cent_owed % 25) % 10) // 5
    pennies = ((cent_owed % 25) % 10) % 5

    print(f"{quarters + dimes + nickels + pennies}")


if __name__ == "__main__":
    main()
