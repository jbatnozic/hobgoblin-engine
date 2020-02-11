#pragma once

// RIGEL v1.0

// Main header file for including Rigel Networking
// ***************************************************
//
// Call rn::HandlerMgr::initialize() at the start of main()
//
// Required function signature for handlers:
// void function_name(rn::Node *receiver, rn::NodeType::Enum node_type);
// 
// ***************************************************

#include "Rigel-config.hpp"
#include "Rigel-types.hpp"

#include "Rigel-argfunc.hpp"
#include "Rigel-argrecv.hpp"
#include "Rigel-event.hpp"
#include "Rigel-handlers.hpp"
#include "Rigel-node.hpp"
#include "Rigel-tcp-client.hpp"
#include "Rigel-tcp-server.hpp"
//#include "Rigel_udp_node.h"

