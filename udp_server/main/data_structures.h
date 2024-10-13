// data_structures.h
#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stdint.h>

typedef enum
{
    ERR = 0,
    ACK = 1,
    CONTROL_COMMAND = 2,
    MOVE_TO_COMMAND = 3,
} message_type_t;

typedef enum
{
    ACK_FLAG = 0,
    NACK_FLAG = 1,
} message_flags_t;

typedef struct __attribute__((packed))
{
    int8_t version_number; // 1 byte defining version number of the protocol
    int8_t message_type;   // 1 byte defining message type (e.g. ERR=0, ACK=1, Control_Command=2, MoveTo_Command=3, ...)
    int8_t flags;          // 1 byte for different flags (e.g. ACK=0, NACK=1, ...)
    int8_t length;         // 1 byte specifying the length of the payload in bytes
} header_t;

typedef struct __attribute__((packed))
{
    int8_t direction;
    int32_t speed;
    int8_t stop;
} control_command_t;

typedef struct __attribute__((packed))
{
    int32_t x;
    int32_t y;
} move_to_command_t;

#endif // DATA_STRUCTURES_H
