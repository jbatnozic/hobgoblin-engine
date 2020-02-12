
#include "Rigel-config.hpp"
#include "Rigel-types.hpp"
#include "Rigel-client-slot.hpp"

#include <memory>
#include <unordered_map>
#include <string>

namespace RIGELNETW_CONFIG_NS_NAME {

    ClientSlot::ClientSlot() {
			
        whitelist = std::make_unique< std::unordered_map<std::string, RegPermission> >();
        socket.setBlocking(false);	

	    reset();

	    }

    ClientSlot::~ClientSlot() {
    
        socket.disconnect();

        }

    void ClientSlot::reset() {
				
	    status = Free;
	    latency = sf::milliseconds(-1);
        waiting_for_ping_back  = false;
        waiting_for_passphrase = true;
        update_ordinal = 0;

        whitelist->clear();
	    ping_clock.restart();
        socket.disconnect();
        packet.clear();
				
	    }

    IpAddress ClientSlot::get_ip() const {
				
	    return (socket.getRemoteAddress());

	    }

    Port ClientSlot::get_port() const {
				
	    return (socket.getRemotePort());
				
	    }

    } // End Rigel namespace