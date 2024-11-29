from io import BytesIO
from typing import Union


class PersonMsgError(Exception):
    pass


class PersonMsg:
    def __init__(self,
                 personmsg: str) -> None:
        self.personmsg: bytes = personmsg.encode("ascii")

        if len(self.personmsg) > 1024:
            raise PersonMsgError(f"Bad person msg: '{self.personmsg}'!")

    def serialize(self) -> bytes:
        return b"".join([
            self.personmsg
        ])

    @classmethod
    def from_bytes(cls, hexa: Union[bytes, BytesIO]):
        buf: BytesIO = BytesIO(hexa) if isinstance(hexa, bytes) else hexa

        personmsg: str = buf.read().decode("ascii")

        return cls(personmsg=personmsg)
