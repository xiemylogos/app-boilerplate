# Technical Specification

> **Warning**
This documentation is a template and shall be updated with your own APDUs.

## About

This documentation describes the APDU messages interface to communicate with the Ontology application.

The application covers the following functionalities :

  - Get a public Ont address given a BIP 32 path
  - Sign a basic Ont transaction given a BIP 32 path and raw transaction
  - Retrieve the Ont app version
  - Retrieve the Ont app name

The application interface can be accessed over HID or BLE

## APDUs

### GET ONT PUBLIC ADDRESS

#### Description

This command returns the public key for the given BIP 32 path.

The address can be optionally checked on the device before being returned.

#### Coding

##### `Command`

| CLA | INS   | P1                                                 | P2    | Lc       | Le       |
| --- | ---   | ---                                                | ---   | ---      | ---      |
| 80  |  05   |  00 : return address                               | 00    | variable | variable |
|     |       |  01 : display address and confirm before returning |       |          |          |

##### `Input data`

| Description                                                      | Length |
| ---                                                              | ---    | 
| Number of BIP 32 derivations to perform (max 10)                 | 1      |
| First derivation index (big endian)                              | 4      |
| ...                                                              | 4      |
| Last derivation index (big endian)                               | 4      |
       
##### `Output data`

| Description                                                      | Length |
| ---                                                              | ---    | 
| Public Key length                                                | 1      | 
| Public Key                                                       | var    |
| Chain code length                                                | 1      |
| Chain code                                                       | var    |


### SIGN Ont/Ong TRANSACTION

#### Description

This command signs a Ont transaction after having the user validate the transactions parameters.

The input data is the RLP encoded transaction streamed to the device in 255 bytes maximum data chunks.

#### Coding

##### `Command`

| CLA | INS  | P1                   | P2                               | Lc       | Le       |
| --- | ---  | ---                  | ---                              | ---      | ---      |
| 80  | 02   |  00-FF : chunk index | 00 : last transaction data block | variable | variable |
|     |      |                      | 80 : subsequent transaction data block |    |          |

##### `Input data (first transaction data block)`

| Description                                          | Length   | 
| ---                                                  | ---      | 
| Number of BIP 32 derivations to perform (max 10)     | 1        |
| First derivation index (little endian)               | 4        |
| ...                                                  | 4        |
| Last derivation index (little endian)                | 4        |
  
##### `Input data (other transaction data block)`

| Description                                          | Length   |
| ---                                                  | ---      |
| Transaction chunk                                    | variable |
                              

##### `Output data`

| Description                                          | Length   |
| ---                                                  | ---      | 
| Signature length                                     | 1        |
| Signature                                            | variable |
| v                                                    | 1        |


### SIGN PERSONAL MESSAGE

#### Description

This command signs a personal message after having the user validate the message parameters.

The input data is the RLP encoded transaction streamed to the device in 255 bytes maximum data chunks.

#### Coding

##### `Command`

| CLA | INS  | P1                   | P2                               | Lc       | Le       |
| --- | ---  | ---                  | ---                              | ---      | ---      |
| 80  | 07   |  00-FF : chunk index | 00 : last personal msg data block  | variable | variable |
|     |      |                      | 80 : subsequent personal msg data block |    |          |

##### `Input data (first person message data block)`

| Description                                          | Length   | 
| ---                                                  | ---      | 
| Number of BIP 32 derivations to perform (max 10)     | 1        |
| First derivation index (little endian)               | 4        |
| ...                                                  | 4        |
| Last derivation index (little endian)                | 4        |

```
for example
uint32_t msg_length = 11;
uint8_t msg_content[] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'};
[0x00, 0x00, 0x00, 0x0B] [0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64]
|      msg length       |                      message content                            |
```
##### `Input data (other personal msg data block)`

| Description                                          | Length   |
| ---                                                  | ---      |
| message chunk                                        | variable |


##### `Output data`

| Description                                          | Length   |
| ---                                                  | ---      | 
| Signature length                                     | 1        |
| Signature                                            | variable |
| v                                                    | 1        |


### SIGN OEP4  TRANSACTION

#### Description

This command signs a Oep4 transaction after having the user validate the Oep4 transactions parameters.

The input data is the RLP encoded transaction streamed to the device in 255 bytes maximum data chunks.

#### Coding

##### `Command`

| CLA | INS  | P1                   | P2                                    | Lc       | Le       |
| --- | ---  | ---                  | ---                                   | ---      | ---      |
| 80  | 08   |  00-FF : chunk index | 00 : last oep4 transaction data block | variable | variable |
|     |      |                      | 80 : subsequent oep4 transaction data block |    |          |

##### `Input data (first oep4 transaction data block)`

| Description                                          | Length   | 
| ---                                                  | ---      | 
| Number of BIP 32 derivations to perform (max 10)     | 1        |
| First derivation index (little endian)               | 4        |
| ...                                                  | 4        |
| Last derivation index (little endian)                | 4        |

##### `Input data (other transaction data block)`

| Description                                          | Length   |
| ---                                                  | ---      |
| Transaction chunk                                    | variable |


##### `Output data`

| Description                                          | Length   |
| ---                                                  | ---      | 
| Signature length                                     | 1        |
| Signature                                            | variable |
| v                                                    | 1        |


### GET APP VERSION

#### Description

This command returns boilerplate application version

#### Coding

##### `Command`

| CLA | INS | P1  | P2  | Lc   | Le |
| --- | --- | --- | --- | ---  | ---|
| 80  | 03  | 00  | 00  | 00   | 04 |

##### `Input data`

None

##### `Output data`

| Description                       | Length |
| ---                               | ---    |
| Application major version         | 01 |
| Application minor version         | 01 |
| Application patch version         | 01 |


### GET APP NAME

#### Description

This command returns boilerplate application name

#### Coding

##### `Command`
| CLA | INS | P1  | P2  | Lc   | Le |
| --- | --- | --- | --- | ---  | ---|
| 80  | 04  | 00  | 00  | 00   | 04 |

##### `Input data`

None

##### `Output data`
| Description           | Length   |
| ---                   | ---      |
| Application name      | variable |


## Status Words

The following standard Status Words are returned for all APDUs.

##### `Status Words`


| SW       | SW name                     | Description                                           |
| ---      | ---                         | ---                                                   |
|   6985   | SW_DENY	                 | Rejected by user                                      |
|   6A86   | SW_WRONG_P1P2               | Either P1 or P2 is incorrect                          |
|   6A87   | SW_WRONG_DATA_LENGTH        | Lc or minimum APDU length is incorrect                |
|   6D00   | SW_INS_NOT_SUPPORTED	     | No command exists with INS                            |
|   6E00   | SW_CLA_NOT_SUPPORTED        | Bad CLA used for this application                     |
|   B000   | SW_WRONG_RESPONSE_LENGTH    | Wrong response length (buffer size problem)           |
|   B001   | SW_DISPLAY_BIP32_PATH_FAIL  | BIP32 path conversion to string failed                |
|   B002   | SW_DISPLAY_ADDRESS_FAIL     | Address conversion to string failed                   |
|   B003   | SW_DISPLAY_AMOUNT_FAIL      | Amount conversion to string failed                    |
|   B004   | SW_WRONG_TX_LENGTH	         | Wrong raw transaction length                          |
|   B005   | SW_TX_PARSING_FAIL          | Failed to parse raw transaction                       |
|   B006   | SW_TX_HASH_FAIL	         | Failed to compute hash digest of raw transaction      |
|   B007   | SW_BAD_STATE                | Security issue with bad state                         |
|   B008   | SW_SIGNATURE_FAIL           | Signature of data(tx or personal msg) failed            |
|   B009   | SW_PERSONAL_MSG_PARSING_FAIL  | Failed to parse personal msg                            |
|   B00A   | SW_PERSONAL_MSG_HASH_FAIL     | Failed to compute hash digest of personal msg           |
|   B00B   | SW_WRONG_PERSONAL_MSG_LENGTH  | Wrong personal msg length                               |
|   B00C   | SW_TX_PAYLOAD_PARSING_FAIL  | Failed to parse tx payload                            |
|   B00D   | SW_OEP4_TX_PARSING_FAIL     | Failed to parse oep4 tx                               |
|   B00E   | SW_OEP4_TX_PAYLOAD_PARSING_FAIL | Failed to parse tx payload                        |
|   9000   | OK	                         | Success                                               |