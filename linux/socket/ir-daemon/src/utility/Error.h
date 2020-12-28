/*
 * Error.h
 *
 *  Created on: Apr 4, 2014
 *      Author: Lysle
 */

#ifndef ERROR_H_
#define ERROR_H_

typedef enum {
    ERROR_NONE=0,
    ERROR_FAIL=1,
    ERROR_ALREADY_INITIALIZED=2,
    ERROR_COULD_NOT_CREATE=3,
    ERROR_COULD_NOT_REUSE=4,
    ERROR_BAD_ADDRESS=5,
    ERROR_COULD_NOT_BIND=6,
    ERROR_COULD_NOT_LISTEN=7,
    ERROR_TIMEOUT=8,
    ERROR_SYSTEM=9,             /* see errno */
    ERROR_COULD_NOT_ACCEPT=10,
    ERROR_CLOSED=11,
    ERROR_COULD_NOT_FIND=12,
    ERROR_COULD_NOT_START=13,
    ERROR_NO_COMMAND=14,
    ERROR_INVALID_COMMAND=15,
    ERROR_INCORRECT_NUM_ARGS=16,
    ERROR_INVALID_PARAMETER=17,
    ERROR_COULD_NOT_OPEN=18,
    ERROR_COULD_NOT_WRITE=19,
    ERROR_COULD_NOT_READ=20,
} T_error;



#endif /* ERROR_H_ */