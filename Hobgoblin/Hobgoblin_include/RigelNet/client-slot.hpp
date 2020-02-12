#pragma once

#include "Rigel-SFML-networking.hpp"
#include "Rigel-config.hpp"
#include "Rigel-types.hpp"

#include "Rigel-node.hpp"
#include "Rigel-registry.hpp"

#include <string>
#include <memory>
#include <unordered_map>

namespace RIGELNETW_CONFIG_NS_NAME {

	class ClientSlot : public sf::NonCopyable {
		
		friend class TCPServer;

		private:

			enum SlotStatus {
				
				Free    = 0, 
				Working = 1
				
				};

			// Attributes	
			sf::TcpSocket socket;
			Packet    packet;
		
			sf::Clock ping_clock;
            sf::Time  latency;

            sf::Clock prod_clock;

			int  status;
			bool waiting_for_ping_back;
            bool waiting_for_passphrase;

            unsigned update_ordinal;

            // Registry write permissions
            std::unique_ptr< std::unordered_map<std::string, RegPermission> > whitelist;
			
            // Methods
			IpAddress get_ip() const;

			Port get_port() const;

			void reset();

        public:

            ClientSlot();

            ~ClientSlot();

		};

    } // End Rigel namespace