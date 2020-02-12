#pragma once

#include "Rigel-config.hpp"

#include "Rigel-SFML-networking.hpp"

#include <type_traits>

namespace RIGELNETW_CONFIG_NS_NAME {

    // Number types:
    typedef sf::Int8   Int8;
    typedef sf::Uint8  Uint8;
    typedef sf::Int16  Int16;
    typedef sf::Uint16 Uint16;
    typedef sf::Int32  Int32;
    typedef sf::Uint32 Uint32;
    typedef sf::Int64  Int64;
    typedef sf::Uint64 Uint64;
    typedef double     Double; 
    typedef float      Float;

    // String type:
    typedef const char * Lpcstr; // (Long Pointer to Const STRing)

    // Type vector:
    #define concat(x, y) x ## y
    #define crtype(x, y) concat(x, y)

    typedef crtype(sf::Uint, RIGELNETW_CONFIG_MAX_ARGS) TypeVec;

    #undef crtype
    #undef concat

    // Other message header type (MsgType, HdlInd, ArgCnt, PacketSize):
    typedef unsigned char MsgType;
    typedef unsigned char ArgCnt;
    typedef RIGELNETW_CONFIG_NS_NAME::Uint16 PktSize;
    typedef RIGELNETW_CONFIG_NS_NAME::Int16 HdlInd;

    // IP/Port:
    typedef sf::IpAddress IpAddress;
    typedef RIGELNETW_CONFIG_NS_NAME::Uint16 Port;

    // Packet:
    typedef sf::Packet Packet;

    // Type enumeration:
	struct Type {

		enum Enum {

            Packet = 16,
				
			String = 32,

            Int8   = 64,
            Uint8  = 65,
			Int16  = 66,
			Uint16 = 67,
			Int32  = 68,
			Uint32 = 69,
            Int64  = 70,
            Uint64 = 71,

			Float  = 72,
			Double = 73,

            OnlyString = Uint8 + String // TEMP
				
			};

        static bool message_has_strings(int message_type) {
                
            return (message_type >= String + Int8);

            }
		
		};

    // Node type enumeration:
    struct NodeType {

        enum Enum {

            Null = 0

            , Server_TCP
            , Client_TCP
            , Server_UDP
            , Client_UDP

            };

        };

    #define RIGELNETW_ENUMERATE_TYPE(type) \
            ( (std::is_same< type, RIGELNETW_CONFIG_NS_NAME::Int8   >::value) * Type::Int8   \
            + (std::is_same< type, RIGELNETW_CONFIG_NS_NAME::Uint8  >::value) * Type::Uint8  \
            + (std::is_same< type, RIGELNETW_CONFIG_NS_NAME::Int16  >::value) * Type::Int16  \
            + (std::is_same< type, RIGELNETW_CONFIG_NS_NAME::Uint16 >::value) * Type::Uint16 \
            + (std::is_same< type, RIGELNETW_CONFIG_NS_NAME::Int32  >::value) * Type::Int32  \
            + (std::is_same< type, RIGELNETW_CONFIG_NS_NAME::Uint32 >::value) * Type::Uint32 \
            + (std::is_same< type, RIGELNETW_CONFIG_NS_NAME::Int64  >::value) * Type::Int64  \
            + (std::is_same< type, RIGELNETW_CONFIG_NS_NAME::Uint64 >::value) * Type::Uint64 \
            + (std::is_same< type, RIGELNETW_CONFIG_NS_NAME::Float  >::value) * Type::Float  \
            + (std::is_same< type, RIGELNETW_CONFIG_NS_NAME::Double >::value) * Type::Double )

	}