import pytest

from ragger.error import ExceptionRAPDU
from application_client.boilerplate_command_sender import CLA, InsType, P1, P2, Errors
from utils import hex_to_bytes


# Ensure the app returns an error when a bad CLA is used
def test_bad_cla(backend):
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(cla=CLA + 1, ins=InsType.GET_VERSION)
    assert e.value.status == Errors.SW_CLA_NOT_SUPPORTED


# Ensure the app returns an error when a bad INS is used
def test_bad_ins(backend):
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(cla=CLA, ins=0xff)
    assert e.value.status == Errors.SW_INS_NOT_SUPPORTED


# Ensure the app returns an error when a bad P1 or P2 is used
def test_wrong_p1p2(backend):
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(cla=CLA, ins=InsType.GET_VERSION, p1=P1.P1_START + 1, p2=P2.P2_LAST)
    assert e.value.status == Errors.SW_WRONG_P1P2
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(cla=CLA, ins=InsType.GET_VERSION, p1=P1.P1_START, p2=P2.P2_MORE)
    assert e.value.status == Errors.SW_WRONG_P1P2
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(cla=CLA, ins=InsType.GET_APP_NAME, p1=P1.P1_START + 1, p2=P2.P2_LAST)
    assert e.value.status == Errors.SW_WRONG_P1P2
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(cla=CLA, ins=InsType.GET_APP_NAME, p1=P1.P1_START, p2=P2.P2_MORE)
    assert e.value.status == Errors.SW_WRONG_P1P2


# Ensure the app returns an error when a bad data length is used
def test_wrong_data_length(backend):
    # APDUs must be at least 4 bytes: CLA, INS, P1, P2.
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange_raw(bytes.fromhex("E00300"))
    assert e.value.status == Errors.SW_WRONG_DATA_LENGTH
    # APDUs advertises a too long length
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange_raw(bytes.fromhex("E003000005"))
    assert e.value.status == Errors.SW_WRONG_DATA_LENGTH

# Ensure there is no state confusion when trying wrong APDU sequences
def test_invalid_state(backend):
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(cla=CLA,
                         ins=InsType.SIGN_TX,
                         p1=P1.P1_START + 1,  # Try to continue a flow instead of start a new one
                         p2=P2.P2_MORE,
                         data=hex_to_bytes("00d115ae02abc409000000000000204e00000000000005815d34e0e9ab73a175ec86ffb24aad5bee20f17b00c66b1405815d34e0e9ab73a175ec86ffb24aad5bee20f16a7cc8141451108489337c8055a9c1ed9158c947d22070d76a7cc808000064a7b3b6e00d6a7cc86c51c10a7472616e7366657256321400000000000000000000000000000000000000020068164f6e746f6c6f67792e4e61746976652e496e766f6b6500")) # data is not parsed in this case
    assert e.value.status == Errors.SW_BAD_STATE
