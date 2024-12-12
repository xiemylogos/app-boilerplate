import unittest
from ragger.bip import pack_derivation_path

def add(a, b):
    return a + b

class TestMathFunctions(unittest.TestCase):
    def test_add(self):
        #path = "m/44'/1024'/0'/0/0"
        path = "m/44'/888'/0'/0/0"
        data=pack_derivation_path(path)
        print("data",data)
        print("len:",len(data))
        hex_string = data.hex()
        print(hex_string)

if __name__ == "__main__":
    unittest.main()