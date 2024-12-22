from hashlib import sha256
from _sha256 import sha256
from ecdsa.curves import NIST256p

from ecdsa.curves import SECP256k1
from ecdsa.keys import VerifyingKey
from ecdsa.util import sigdecode_der


# Check if a signature of a given message is valid
def check_signature_validity(public_key: bytes, signature: bytes, message: bytes) -> bool:
    pk: VerifyingKey = VerifyingKey.from_string(
        public_key,
        curve=NIST256p,
        hashfunc=sha256
    )
    return pk.verify(signature=signature,
                     data=message,
                     hashfunc=sha256,
                     sigdecode=sigdecode_der)


def checkperson_signature_validity(public_key: bytes, signature: bytes, message: bytes) -> bool:
    pk: VerifyingKey = VerifyingKey.from_string(
        public_key,
        curve=NIST256p,
        hashfunc=sha256
    )
    return pk.verify(signature=signature,
                     data=message,
                     hashfunc=sha256,
                     sigdecode=sigdecode_der)


def hex_to_bytes(hex_str: str) -> bytes:
    """
    Converts a hexadecimal string to a bytes object.

    Args:
        hex_str (str): The hexadecimal string to convert.

    Returns:
        bytes: The corresponding bytes object.
    """
    # Remove any whitespace and ensure the string length is even
    hex_str = hex_str.replace(" ", "").strip()
    if len(hex_str) % 2 != 0:
        raise ValueError("Hex string must have an even number of characters")
    return bytes.fromhex(hex_str)

def utf8_strlen(s: str) -> int:
        byte_data = s.encode('utf-8')
        length = 0
        i = 0
        while i < len(byte_data):
            byte = byte_data[i]
            if (byte & 0x80) == 0:
                i += 1
            elif (byte & 0xE0) == 0xC0:
                i += 2
            elif (byte & 0xF0) == 0xE0:
                i += 3
            elif (byte & 0xF8) == 0xF0:
                i += 4
            else:
                i += 1
            length += 1
        return length

def int_byte(value: int) ->bytes:
    byte_length = (value.bit_length() + 7) // 8
    return value.to_bytes(byte_length, byteorder='little')