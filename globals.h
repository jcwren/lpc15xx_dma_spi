#pragma once

//
//  I/O pin definitions
//
#define GPIO_LCD_RA8876_CS_PORT      0  // PIO0.14  -- D13/P5 on J1
#define GPIO_LCD_RA8876_CS_PIN      14  // PIO0.14  -- RA8876 chip select (active low)

#define GPIO_SPI0_MISO_PORT          0  // PIO0.12  -- D12/P6 on J1
#define GPIO_SPI0_MISO_PIN          12  // PIO0.12  -- SPI0 MISO

#define GPIO_SPI0_MOSI_PORT          0  // PIO0.28  -- D11/P7 on J1
#define GPIO_SPI0_MOSI_PIN          28  // PIO0.28  -- SPI0 MOSI

#define GPIO_SPI0_SCK_PORT           0  // PIO0.27  -- D10/P8 on J1
#define GPIO_SPI0_SCK_PIN           27  // PIO0.27  -- SPI0 SCK

//
//
//  Utility macros
//
#define mlo(x) ((x) & 0xff)
#define mhi(x) ((x) >> 8)

//
//  For instantiated arrays:
//    isizeof    - return the sizeof a type as an integer
//
//    arrsizeof  - return the number of elements in an array (size of array / size of a single element) as a sizeof_t
//    iarrsizeof - return the number of elements in an array (size of array / size of a single element) as an int (to prevent signed/unsigned comparison warnings)
//    elsizeof   - return the size of a single element in an array
//
//  For members of structures (not necessarily instantiated)
//    earrsizeof - return the number of elements in an array defined in a structure
//    esizeof    - return size of a member in a structure
//    esizeofx   - return size of a element in an array that is a member of a structure
//    esizeofn   - return size of a element in an array that is a member of a structure, multipled by 'count'
//    offset     - return the offset at which a member of a structure starts at
//    offsetn    - return the offset at which an array element in a member of a structure starts at
//    stypeof    - return the typeof() of a member of a structure
//
#define isizeof(x) ((int) sizeof (x))

#define arrsizeof(x) (sizeof (x) / sizeof (x [0]))
#define iarrsizeof(x) (int) (sizeof (x) / sizeof (x [0]))
#define elsizeof(x) (sizeof (x [0]))

#define earrsizeof(type,mem) (size_t) (sizeof (((type *)0)->mem) / sizeof(((type *)0)->mem [0]))
#define esizeof(type,mem) (size_t) sizeof (((type *)0)->mem)
#define esizeofx(type,mem) (size_t) sizeof (((type *)0)->mem [0])
#define esizeofn(type,mem,count) (size_t) (sizeof (((type *)0)->mem [0]) * (count))
#define offset(type,mem) (size_t) (&(((type *)0)->mem))
#define offsetn(type,mem,index) (size_t) ((size_t) (&(((type *)0)->mem [0])) + (size_t) ((size_t) sizeof (((type *)0)->mem [0]) * (size_t) (index)))
#define stypeof(type,mem) typeof (((type *)0)->mem)

#define STRINGIFY(a) #a
#define STRINGIFY_CONSTANT(a) STRINGIFY(a)

//
//  Create macros for attributes
//
#ifndef __DEPRECATED
  #define __DEPRECATED     __attribute__ ((deprecated))
#endif

#ifndef __NAKED_NORETURN
  #define __NAKED_NORETURN __attribute__ ((naked, noreturn))
#endif

#ifndef __NOOPTIMIZE
  #define __NOOPTIMIZE     __attribute__ ((optimize("O0")))
#endif

#ifndef __NORETURN
  #define __NORETURN       __attribute__ ((noreturn))
#endif

#ifndef __PACKED
  #define __PACKED         __attribute__ ((packed))
#endif

#ifndef __UNUSED
  #define __UNUSED         __attribute__ ((unused))
#endif

#ifndef __USED
  #define __USED           __attribute__ ((used))
#endif

#ifndef __FALLTHROUGH
  #define __FALLTHROUGH    __attribute__ ((fallthrough))
#endif

#ifndef __ALIGNED
  #define __ALIGNED(x)     __attribute__ ((aligned (x)))
#endif

//
//  FreeRTOS time macros
//
#define pdMS_TO_TICKS(xTimeInMs) ((TickType_t) (xTimeInMs))

#define PREFIX_DECI        ((TickType_t) 10)
#define PREFIX_CENTI       ((TickType_t) 100)
#define PREFIX_MILLI       ((TickType_t) 1000)
#define pdDAY_TO_TICKS(x)  (pdMS_TO_TICKS (x * PREFIX_MILLI * 60 * 60 * 24))
#define pdHOUR_TO_TICKS(x) (pdMS_TO_TICKS (x * PREFIX_MILLI * 60 * 60))
#define pdMIN_TO_TICKS(x)  (pdMS_TO_TICKS (x * PREFIX_MILLI * 60))
#define pdSEC_TO_TICKS(x)  (pdMS_TO_TICKS (x * PREFIX_MILLI))
#define pdDS_TO_TICKS(x)   (pdMS_TO_TICKS (x * (PREFIX_MILLI / PREFIX_DECI)))
#define pdCS_TO_TICKS(x)   (pdMS_TO_TICKS (x * (PREFIX_MILLI / PREFIX_CENTI)))
#define pdSEC_TO_MS(x)     (x * PREFIX_MILLI)
#define pdDS_TO_MS(x)      (x * (PREFIX_MILLI / PREFIX_DECI))
#define pdCS_TO_MS(x)      (x * (PREFIX_MILLI / PREFIX_CENTI))
#define pdMS_TO_MS(x)      (x)
