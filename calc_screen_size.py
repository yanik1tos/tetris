#!/usr/bin/env python3

import os


for i in range(100, 1, -1):
    print(i)


input()
os.system("clear")



print("Tens")

for i in range(0, 300, 10):
    print(i, ' ' * (10 - len(str(i))), end="")


print("\nDigits")

for i in range(300):
    print(i % 10, end="")



