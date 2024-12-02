from hashlib import sha256
from sha3 import keccak_256

from ecdsa.curves import SECP256k1
from ecdsa.keys import VerifyingKey
from ecdsa.util import sigdecode_der


# Check if a signature of a given message is valid
def check_signature_validity(public_key: bytes, signature: bytes, message: bytes) -> bool:
    pk: VerifyingKey = VerifyingKey.from_string(
        public_key,
        curve=SECP256k1,
        hashfunc=sha256
    )
    return pk.verify(signature=signature,
                     data=message,
                     hashfunc=keccak_256,
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