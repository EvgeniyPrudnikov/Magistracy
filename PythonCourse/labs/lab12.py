import argparse
from sys import exit

"""
Unix cut analog
"""


def fieldsNumbers(fieldsList):
    """
    parse fields string
    :param fieldsString: list of str
    :return: list
    """
    return map(int, fieldsList.split(','))


def readFile(filename):
    """
    read input file
    :param filename: str
    :return: list
    """
    lines = [line.rstrip('\n') for line in open(filename)]
    # return [i.split(sep) for i in lines if len(i.split(sep)) > 1]
    return [i.split(sep) for i in lines]


def take(sourceTable, listOfFields):
    """
    takes needed columns from source table
    :param sourceTable: list
    :param listOfFields: list
    :return: list
    """
    # print table
    # if not sourceTable:
    #    print 'error: wrong separator'
    #    exit()
    try:
        result = []
        [result.append([i[j] for i in sourceTable]) for j in listOfFields]
    except:
        print 'error: wrong column number'
        exit()
    return result


def cut(takenColumns, sep):
    """
    pretty print of taken columns
    :param takenColumns: list
    :return: none
    """
    tempList = map(list, zip(*takenColumns))
    for i in tempList:
        print sep.join(i)


def parseInputArgs():
    """
    parse input args by argparse lib
    :return: object
    """
    parser = argparse.ArgumentParser(description="Unix cut analog", usage='%(prog)s [arguments]')

    # pos arg
    parser.add_argument('filename', type=str, help='input file name')

    # req arg
    requiredNamed = parser.add_argument_group('required arguments')
    requiredNamed.add_argument('-f', '--fields', type=str, help='list of fields, separated by comma', required=True)
    # optional args
    parser.add_argument('-s', '--separator', type=str, default='\t', help='column separator, default tab')

    args = parser.parse_args()
    return args


# parse input args
inputArgs = parseInputArgs()

# --- set variables from args ---------------------------
filename = inputArgs.filename
sep = inputArgs.separator
# --- ----------------------------------------------------

# --- source table  --------------------------------------
table = readFile(filename)
# --- ----------------------------------------------------

# --- needed fields --------------------------------------
fields = fieldsNumbers(inputArgs.fields)
# --- ----------------------------------------------------

if __name__ == '__main__':
    # --- print result table ---------------------------------
    cut(take(table, fields), sep)
    # --- ----------------------------------------------------
