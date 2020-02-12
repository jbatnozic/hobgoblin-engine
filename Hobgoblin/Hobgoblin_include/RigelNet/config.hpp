#pragma once

// Name of Rigel's namespace; Default is rn
#define RIGELNETW_CONFIG_NS_NAME rn

// Maximum number of arguments you can pass with a single send() method call
// Can be 8, 16, 32 or 64; Default is 16
#define RIGELNETW_CONFIG_MAX_ARGS 16

namespace RIGELNETW_CONFIG_NS_NAME {
    
    const size_t CONFIG_HDLVEC_INIT_N = 64;

    }

// Define to compile without passphrase functionality
// #define RIGELNETW_CONFIG_NO_PASSPHRASE

// Define to compile with cout-ing send statistics
// #define RIGELNETW_CONFIG_SEND_STATS