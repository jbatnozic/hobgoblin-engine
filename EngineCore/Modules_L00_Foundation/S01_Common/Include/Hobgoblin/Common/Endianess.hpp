// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_COMMON_ENDIANESS_HPP
#define UHOBGOBLIN_COMMON_ENDIANESS_HPP

#ifdef __ORDER_LITTLE_ENDIAN__
#define HG_LITTLE_ENDIAN __ORDER_LITTLE_ENDIAN__
#else
#define HG_LITTLE_ENDIAN 1234
#endif

#ifdef __ORDER_BIG_ENDIAN__
#define HG_BIG_ENDIAN __ORDER_BIG_ENDIAN__
#else
#define HG_BIG_ENDIAN 4321
#endif

#ifdef __BYTE_ORDER__
#define HG_ENDIANESS __BYTE_ORDER__
#else
#define HG_ENDIANESS HG_LITTLE_ENDIAN
#endif

#endif // !UHOBGOBLIN_COMMON_ENDIANESS_HPP
