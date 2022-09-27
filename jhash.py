#!/usr/bin/python3

import numpy as np
import base64
import sys

def compute(raw_data:bytes, length=256,iterations=1,seed=0):
    data = list(raw_data)
    # padding
    for i in data:
        seed = seed ^ i
    if len(data) < length:
        t1 = length - len(data)
        data = data + [0] * t1
    else:
        t1 = len(data) % length
        if t1 != 0:
            data = data + [0] * (length - t1)
    
    # split into chunks
    def split_chunks(l, n):
        for i in range(0, len(l), n):
            yield l[i:i + n]

    #Format hash
    def format(input:bytearray):
        return f"$2b${hex(iterations)[2:]}${base64.b64encode(input).decode()}"

    chunks = list(split_chunks(data,length))

    np.random.seed(seed)
    state = bytearray(np.random.bytes(length // 8))
    for _ in range(iterations):
        for chunk in chunks:
            for index, _ in enumerate(state):
                #tmp = state[index % len(state)]
                #state[index % len(state)] = state[(index + 1) % len(state)]
                #state[(index + 1) % len(state)] = tmp
                state[index] = state[index] ^ state[(index - 1) % len(state)]
                state[index] = state[index] ^ chunk[index]
    return format(state)

def compute_state(seed, length):
    np.random.seed(seed)
    return base64.b64encode(bytearray(np.random.bytes(length // 8)))

if __name__ == "__main__":
    if not sys.stdin.isatty():
        data = sys.stdin.buffer.read()
        print(compute(data))
        exit()
    # print(compute_state(6969,256))
    data = input("Enter password for hash: ").encode()
    print(compute(data))
