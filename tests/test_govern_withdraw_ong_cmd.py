import pytest

from application_client.boilerplate_transaction import Transaction
from application_client.boilerplate_command_sender import BoilerplateCommandSender, Errors
from application_client.boilerplate_response_unpacker import unpack_get_public_key_response, unpack_sign_tx_response
from ragger.error import ExceptionRAPDU
from utils import check_signature_validity
import hashlib

# In this tests we check the behavior of the device when asked to sign a transaction
import logging

# Configure logger
logger = logging.getLogger("test_logger")
logger.setLevel(logging.DEBUG)


# In this test we send to the device a transaction to sign and validate it on screen
# The transaction is short and will be sent in one chunk
# We will ensure that the displayed information is correct by using screenshots comparison
def test_sign_withdraw_ong_tx_short_tx(backend, scenario_navigator):
    # Use the app interface instead of raw interface
    client = BoilerplateCommandSender(backend)
    # The path used for this entire test
    path: str = "m/44'/1024'/0'/0/0"

    # First we need to get the public key of the device in order to build the transaction
    rapdu = client.get_public_key(path=path)
    logger.debug("rapu.data.hex:%s", rapdu.data.hex())

    _, public_key, _, _ = unpack_get_public_key_response(rapdu.data)
    logger.debug("pubkey:%s", public_key.hex())

    # Create the transaction that will be sent to the device for signing
    transaction = Transaction(
        rawtx="00d1945146e0c409000000000000204e00000000000003aa2d413291c856afadd4adaf5a0778ddefd2355600c66b1403aa2d413291c856afadd4adaf5a0778ddefd2356a7cc86c0b77697468647261774f6e671400000000000000000000000000000000000000070068164f6e746f6c6f67792e4e61746976652e496e766f6b65"
    ).serialize()

    # Send the sign device instruction.
    # As it requires on-screen validation, the function is asynchronous.
    # It will yield the result when the navigation is done
    logger.debug("tx:%s", transaction.hex())
    with client.sign_withdraw_ong_tx(path=path, transaction=transaction):
        # Validate the on-screen request by performing the navigation appropriate for this device
        scenario_navigator.review_approve()

    # The device as yielded the result, parse it and ensure that the signature is correct
    response = client.get_async_response().data
    _, der_sig, _ = unpack_sign_tx_response(response)
    first_hash = hashlib.sha256(transaction).digest()
    second_hash = hashlib.sha256(first_hash).digest()
    logger.debug("pubkey:%s,der_sig:%s,transaction:%s,first_hash:%s", public_key.hex(), der_sig.hex(),
                 transaction.hex(), first_hash.hex())
    assert check_signature_validity(public_key, der_sig, second_hash)


# Transaction signature refused test
# The test will ask for a transaction signature that will be refused on screen
def test_sign_withdraw_ong_tx_refused(backend, scenario_navigator):
    # Use the app interface instead of raw interface
    client = BoilerplateCommandSender(backend)
    path: str = "m/44'/1024'/0'/0/0"

    rapdu = client.get_public_key(path=path)
    _, pub_key, _, _ = unpack_get_public_key_response(rapdu.data)

    transaction = Transaction(
        rawtx="00d1945146e0c409000000000000204e00000000000003aa2d413291c856afadd4adaf5a0778ddefd2355600c66b1403aa2d413291c856afadd4adaf5a0778ddefd2356a7cc86c0b77697468647261774f6e671400000000000000000000000000000000000000070068164f6e746f6c6f67792e4e61746976652e496e766f6b65"
    ).serialize()

    with pytest.raises(ExceptionRAPDU) as e:
        with client.sign_withdraw_ong_tx(path=path, transaction=transaction):
            scenario_navigator.review_reject()

    # Assert that we have received a refusal
    assert e.value.status == Errors.SW_DENY
    assert len(e.value.data) == 0
