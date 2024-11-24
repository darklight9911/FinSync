import os
import sys

projectfiles = [
    "finsync.c",
    "fsGlobal.h",
    "structures.h",
    "constants.h",
    "views.h"
]

availableProjectFiles = os.listdir()

def checkFiles() -> int:
    totalLinesCount = 0
    for i in projectfiles:
        if i not in availableProjectFiles:
            print(f"File {i} not available in project directory")
            return False
        else:
            fileLines = open(i, "r").read().splitlines()
            totalLinesCount =  totalLinesCount + len(fileLines)
    print("All project files are in projectdirectory")
    return totalLinesCount


if sys.argv[1] == "info":
    totalLinesCount = checkFiles()
    print(f"Total Lines of Project: {totalLinesCount}")
    print(f"Additional Library: {open("linkers", "r").read().splitlines()}")