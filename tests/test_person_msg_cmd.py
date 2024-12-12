import pytest

from application_client.boilerplate_person_msg import PersonMsg
from application_client.boilerplate_command_sender import BoilerplateCommandSender, Errors
from application_client.boilerplate_response_unpacker import unpack_get_public_key_response, unpack_sign_person_msg_response
from ragger.error import ExceptionRAPDU
from utils import check_signature_validity

# In this tests we check the behavior of the device when asked to sign a person msg


# In this test we send to the device a person msg to sign and validate it on screen
# The person msg is short and will be sent in one chunk
# We will ensure that the displayed information is correct by using screenshots comparison
def test_sign_person_msg_short_msg(backend, scenario_navigator):
    # Use the app interface instead of raw interface
    client = BoilerplateCommandSender(backend)
    # The path used for this entire test
    path: str = "m/44'/1024'/0'/0/0"

    # First we need to get the public key of the device in order to build the person msg
    rapdu = client.get_public_key(path=path)
    _, public_key, _, _ = unpack_get_public_key_response(rapdu.data)

    # Create the personmsg that will be sent to the device for signing
    personmsg = PersonMsg(
        personmsg="test msg hash"
    ).serialize()

    # Send the sign device instruction.
    # As it requires on-screen validation, the function is asynchronous.
    # It will yield the result when the navigation is done
    with client.sign_person_msg(path=path, personmsg=personmsg):
        # Validate the on-screen request by performing the navigation appropriate for this device
        scenario_navigator.review_approve()

    # The device as yielded the result, parse it and ensure that the signature is correct
    response = client.get_async_response().data
    _, der_sig, _ = unpack_sign_person_msg_response(response)
    assert check_signature_validity(public_key, der_sig, personmsg)

"""
def test_sign_person_msg_long_msg(backend, scenario_navigator):
    # Use the app interface instead of raw interface
    client = BoilerplateCommandSender(backend)
    path: str = "m/44'/1024'/0'/0/0"

    rapdu = client.get_public_key(path=path)
    _, public_key, _, _ = unpack_get_public_key_response(rapdu.data)

    personmsg = PersonMsg(
        personmsg=("This is a very long person msg. "
              "It will force the app client to send the serialized person msg to be sent in chunk. "
              "As the maximum chunk size is 255 bytes we will make this memo greater than 255 characters. "
              "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed non risus. Suspendisse lectus tortor, dignissim sit amet, adipiscing nec, ultricies sed, dolor. Cras elementum ultrices diam.")
    ).serialize()

    with client.sign_person_msg(path=path, personmsg=personmsg):
        scenario_navigator.review_approve()

    response = client.get_async_response().data
    _, der_sig, _ = unpack_sign_person_msg_response(response)
    assert check_signature_validity(public_key, der_sig, personmsg)


# person msg signature refused test
# The test will ask for a person msg signature that will be refused on screen
def test_sign_person_msg_refused(backend, scenario_navigator):
    # Use the app interface instead of raw interface
    client = BoilerplateCommandSender(backend)
    path: str = "m/44'/1024'/0'/0/0"

    rapdu = client.get_public_key(path=path)
    _, pub_key, _, _ = unpack_get_public_key_response(rapdu.data)

    personmsg = PersonMsg(
        personmsg="This person msg will be refused by the user"
    ).serialize()

    with pytest.raises(ExceptionRAPDU) as e:
        with client.sign_person_msg(path=path, personmsg=personmsg):
            scenario_navigator.review_reject()

    # Assert that we have received a refusal
    assert e.value.status == Errors.SW_DENY
    assert len(e.value.data) == 0

"""