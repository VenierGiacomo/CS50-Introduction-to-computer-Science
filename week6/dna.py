from csv import reader, DictReader
from sys import argv

if len(argv) < 3:
    print("There was an error: (dna.py sequence.txt database.csv)")
    exit()

# read file
with open(argv[2]) as dnaSequence:
    dnaReader = reader(dnaSequence)
    for row in dnaReader:
        dnalist = row

# store it in a string
dna = dnalist[0]
# create a dictionary for the sequences we intend to count
sequences = {}

# extract the sequences from the db to list
with open(argv[1]) as peopleFile:
    people = reader(peopleFile)
    for row in people:
        dnaSequences = row
        dnaSequences.pop(0)
        break

# copy the list in a dictionary where the genes are the keys
for item in dnaSequences:
    sequences[item] = 1

#  look for repetitions of the values from sequence 
for key in sequences:
    l = len(key)
    tempMax = 0
    temp = 0
    for i in range(len(dna)):
        # after having counted a sequence it skips at the end of it to avoid counting again
        while temp > 0:
            temp -= 1
            continue

        # if the segment of dna corresponds to the key and there is a repetition of it we start counting
        if dna[i: i + l] == key:
            while dna[i - l: i] == dna[i: i + l]:
                temp += 1
                i += l

            # it compares the value to the previous longest sequence and if it is longer it overrides it
            if temp > tempMax:
                tempMax = temp

    # store the longest sequences in the dictionary using the correspondent key
    sequences[key] += tempMax

# open and iterate trough the database of people treating each one like a dictionary so it can compare to the sequences one
with open(argv[1], newline='') as peopleFile:
    people = DictReader(peopleFile)
    for person in people:
        match = 0
        # compares the sequences and if there is a match print the name
        for dna in sequences:
            if sequences[dna] == int(person[dna]):
                match += 1
        if match == len(sequences):
            print(person['name'])
            exit()

    print("No match")
