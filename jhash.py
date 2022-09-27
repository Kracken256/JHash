#!/usr/bin/python3

import numpy as np
import base64
import sys


def compute(raw_data: bytes, length=256, iterations=1, seed=0):
    data = list(raw_data)

    def compute_seed(data, length, seed):
        # Derterminitic seed generation
        tmp = list(data) + [length]
        for i in tmp:
            seed += i
        if seed > (2**32 - 1):
            seed = seed % (2**32 - 1)
        return seed
    seed = compute_seed(data,length,seed)

    # padding
    if len(data) < length:
        t1 = length - len(data)
        data = data + [0] * t1
    else:
        t1 = len(data) % length
        if t1 != 0:
            data = data + [0] * (length - t1)

    # split into chunks of [ length ]
    def split_chunks(l, n):
        for i in range(0, len(l), n):
            yield l[i:i + n]

    # Format hash
    def format(input: bytearray):
        # Encoding format v1
        def encode(data):
            return base64.urlsafe_b64encode(data).decode().replace("=", "")
        return f"$2b${hex(iterations)[2:]}${encode(input)}"

    chunks = list(split_chunks(data, length))

    # Sponge Function absorb, squeeze
    np.random.seed(seed)
    state = bytearray(np.random.bytes(length // 8))
    len_of_state = range(len(state))
    for _ in range(iterations):
        for chunk in chunks:
            for index in len_of_state:
                state[index] = state[index] ^ state[(index - 1) % len(state)]
                state[index] = state[index] ^ chunk[index]
    return format(state)


def compute_state(data, length=256, seed=0):
    # Derterminitic seed generation
    data = list(data)
    tmp = data + [length]
    for i in tmp:
        seed += i
    if seed > (2**32 - 1):
        seed = seed % (2**32 - 1)
    np.random.seed(seed)
    return bytearray(np.random.bytes(length // 8)).hex()

if __name__ == "__main__":
    def print_help():
        print("JHash is a sponge function for data fingerprinting.")
        print("Usage:")
        print("\t--help, -h\tPrint this message")
        print("\t--length\tDigest length (default is 256 bit)")
        print("\t--iter\t\tNumber of iterations (default is 1)")
        print("\t--seed\t\tSeed for initial state. Also based on message. (Default is 0). Can also be a string.")
        print("Note:")
        print("\tIf seed is a string then the chaarcodes are summed and used instead.")
    args = sys.argv
    if "--help" in args or "-h" in args:
        print_help()
        exit()
    if not sys.stdin.isatty():
        data = sys.stdin.buffer.read()
        length = 256
        seed = 0
        iterations = 1
        if "--length" in args:
            length = int(args[args.index("--length")+1])
        if "--seed" in args:
            seed = args[args.index("--seed")+1]
            if not seed.isnumeric():
                tmp = 0
                for i in list(seed):
                    tmp += ord(i)
                seed = tmp
            else:
                seed = int(seed)
        if "--iter" in args:
            iterations = int(args[args.index("--iter")+1])
        
        if "--compute-state" in args:
            print(compute_state(data,length,seed))
        else:
            print(compute(data, length, iterations, seed))
        exit()
    data = input("Enter data to digest: ").encode()
    print(compute(data))
