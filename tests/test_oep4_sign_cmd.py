import pytest

from application_client.boilerplate_transaction import Transaction
from application_client.boilerplate_command_sender import BoilerplateCommandSender, Errors
from application_client.boilerplate_response_unpacker import unpack_get_public_key_response, unpack_sign_tx_response
from ragger.error import ExceptionRAPDU
from ragger.navigator import NavInsID
from utils import check_signature_validity

# In this tests we check the behavior of the device when asked to sign a oep4 transaction


# In this test we send to the device a oep4 transaction to sign and validate it on screen
# The oep4 transaction is short and will be sent in one chunk
# We will ensure that the displayed information is correct by using screenshots comparison
def test_sign_oep4_tx_short_tx(backend, scenario_navigator):
    # Use the app interface instead of raw interface
    client = BoilerplateCommandSender(backend)
    # The path used for this entire test
    path: str = "m/44'/1024'/0'/0/0"

    # First we need to get the public key of the device in order to build the oep4 transaction
    rapdu = client.get_public_key(path=path)
    _, public_key, _, _ = unpack_get_public_key_response(rapdu.data)

    # Create the oep4 transaction that will be sent to the device for signing
    ope4_transaction = Transaction(
        rawtx = "00d2d64f9355c409000000000000409c0000000000001451108489337c8055a9c1ed9158c947d22070d7561c6c98c223f4285df248d2cf81fb54b7fbc314c841087472616e736665721451108489337c8055a9c1ed9158c947d22070d7ec9441bf328b6cd6607b3847c30b7eaed756dabe40420f000000000000000000000000000000"
    ).serialize()

    # Send the sign device instruction.
    # As it requires on-screen validation, the function is asynchronous.
    # It will yield the result when the navigation is done
    with client.sign_oep4_tx(path=path, oep4_transaction=ope4_transaction):
        # Validate the on-screen request by performing the navigation appropriate for this device
        scenario_navigator.review_approve()

    # The device as yielded the result, parse it and ensure that the signature is correct
    response = client.get_async_response().data
    _, der_sig, _ = unpack_sign_tx_response(response)
    assert check_signature_validity(public_key, der_sig, ope4_transaction)


# Oep4 Transaction signature refused test
# The test will ask for a oep4 transaction signature that will be refused on screen
def test_sign_oep4_tx_refused(backend, scenario_navigator):
    # Use the app interface instead of raw interface
    client = BoilerplateCommandSender(backend)
    path: str = "m/44'/1024'/0'/0/0"

    rapdu = client.get_public_key(path=path)
    _, pub_key, _, _ = unpack_get_public_key_response(rapdu.data)

    oep4_transaction = Transaction(
        rawtx = "00d115ae02abc409000000000000204e00000000000005815d34e0e9ab73a175ec86ffb24aad5bee20f17b00c66b1405815d34e0e9ab73a175ec86ffb24aad5bee20f16a7cc8141451108489337c8055a9c1ed9158c947d22070d76a7cc808000064a7b3b6e00d6a7cc86c51c10a7472616e7366657256321400000000000000000000000000000000000000020068164f6e746f6c6f67792e4e61746976652e496e766f6b6500"
    ).serialize()

    with pytest.raises(ExceptionRAPDU) as e:
        with client.sign_oep4_tx(path=path, oep4_transaction=oep4_transaction):
            scenario_navigator.review_reject()

    # Assert that we have received a refusal
    assert e.value.status == Errors.SW_DENY
    assert len(e.value.data) == 0
