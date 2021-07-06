from cs50 import get_string

# gets the input inputText
inputText = get_string("Text: \n")

letters = 0
words = 0
sentences = 0
count = 0

for i in inputText:
    count += 1

for i in range(count):
    # counts the letters u
    if (ord(inputText[i]) >= 65 and ord(inputText[i]) <= 122):
        letters += 1

    # counts words splitting at spaces
    elif (ord(inputText[i]) == 32 and (ord(inputText[i - 1]) != 33 and ord(inputText[i - 1]) != 46 and ord(inputText[i - 1]) != 63)):
        words += 1

    # counts words splitting at spaces and other end frase charactes
    elif (ord(inputText[i]) == 33 or ord(inputText[i]) == 46 or ord(inputText[i]) == 63):
        sentences += 1
        words += 1

L = letters * 100 / words
S = sentences * 100 / words
# Use Coleman-Liau ind formula
ind = round(0.0588 * L - 0.296 * S - 15.8)

# Output the result
if (ind < 1):
    print("Before Grade 1")

elif (ind >= 16):
    print("Grade 16+")

else:
    print(f"Grade {ind}")
