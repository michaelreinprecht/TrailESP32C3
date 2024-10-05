// data_structures.h
#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stdint.h>

typedef enum
{
    command = 1,
    location_data = 2,
} struct_types_t;

typedef struct __attribute__((packed)) // ensure 1-byte alignment for struct (same in C#)
{
    int8_t direction; // matches the C# byte -> displays a character
    int32_t speed;    // matches the C# int
    int8_t stop;      // matches the C# byte -> is 0 or 1 (bool)
} command_t;

typedef struct __attribute__((packed))
{
    int32_t x;
    int32_t y;
} location_data_t;

#endif // DATA_STRUCTURES_H
