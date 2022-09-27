import jhash
import binascii
import numpy as np


def check():
    compare_to = "$2b$b$X8Jk6FbMLnG8OrMFstPg13FE722IuRLg1yTsTcCm6Ak="
    for i in range(1000):
        data = list(np.random.bytes(512))
        hash = jhash.compute(data)
        if hash == compare_to:
            print(binascii.hexlify(hash), hash)
            return False
        else:
            print("Good", i)
    return True

if __name__ == "__main__":
    if check():
        print("Pass")
    else:
        print("Fail")