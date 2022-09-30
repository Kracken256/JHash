#!/usr/bin/python3

import jhash

text = "12345678"
example_hash = jhash.compute(text.encode())
print("Hash:", example_hash)

print("Attemping to break hash")

