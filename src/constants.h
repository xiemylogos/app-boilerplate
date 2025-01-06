#pragma once

/**
 * Instruction class of the Boilerplate application.
 */
#define CLA 0x80

/**
 * Length of APPNAME variable in the Makefile.
 */
#define APPNAME_LEN (sizeof(APPNAME) - 1)

/**
 * Maximum length of MAJOR_VERSION || MINOR_VERSION || PATCH_VERSION.
 */
#define APPVERSION_LEN 3

/**
 * Maximum length of application name.
 */
#define MAX_APPNAME_LEN 64

/**
 * Maximum transaction length (bytes).
 */
#define MAX_TRANSACTION_LEN 1024

/**
 * Maximum person msg length (bytes).
 */
#define MAX_PERSON_MSG_LEN  1024

/**
 * Maximum signature length (bytes).
 */
#define MAX_DER_SIG_LEN 72
