from io import BytesIO
from typing import Union

from .boilerplate_utils import read, read_uint, read_varint, write_varint, UINT64_MAX


class PersonMsgError(Exception):
    pass


class PersonMsg:
    def __init__(self,
                 personmsg: str) -> None:
        self.personmsg: bytes = personmsg.encode("ascii")


    def serialize(self) -> bytes:
        return b"".join([
            write_varint(len(self.personmsg)),
            self.personmsg
        ])

    @classmethod
    def from_bytes(cls, hexa: Union[bytes, BytesIO]):
        buf: BytesIO = BytesIO(hexa) if isinstance(hexa, bytes) else hexa

        #personmsg_len: int = read_varint(buf)
        #personmsg: str = read(buf, personmsg_len).decode("ascii")

        personmsg: str = read(buf).decode("ascii")
        return cls(personmsg=personmsg)