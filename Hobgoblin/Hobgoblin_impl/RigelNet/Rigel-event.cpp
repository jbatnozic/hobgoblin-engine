
#include "Rigel-config.hpp"
#include "Rigel-types.hpp"
#include "Rigel-event.hpp"

#include <string>
//#include <iostream> // TEMP

namespace RIGELNETW_CONFIG_NS_NAME {

    Event::Event() {
        
        type = Type::Null;

        }

    Event::Event(Type::Enum type_) {
        
        type = type_;

        }

    std::string Event::illegal_reg_req_key() const {
        
        return m_string;

        }

    std::string Event::bad_passphrase_str() const {
        
        return m_string;

        }

    ///////////////////////////////////////////////////////////////////////////

    Event EventFactory::create_connect(size_t client_index) {

        Event rv(Event::Type::Connect);

        rv.connect.client_index = client_index;

        return rv;

        }

    Event EventFactory::create_disconnect(size_t client_index) {

        Event rv(Event::Type::Disconnect);

        rv.disconnect.client_index = client_index;

        return rv;

        }

    Event EventFactory::create_attempt_timeout(unsigned time_ms) {

        Event rv(Event::Type::AttempTimeout);

        rv.attempt_timeout.time_ms = time_ms;

        return rv;

        }

    Event EventFactory::create_conn_timeout(size_t client_index) {

        Event rv(Event::Type::ConnTimeout);

        rv.conn_timeout.client_index = client_index;

        return rv;

        }

    Event EventFactory::create_illegal_reg_req(size_t client_index, std::string & key) {

        Event rv(Event::Type::IllegalRegReq);

        rv.m_string = key;

        rv.illegal_reg_req.client_index = client_index;

        return rv;

        }

    Event EventFactory::create_kicked(void) {

        Event rv(Event::Type::Kicked);

        return rv;

        }

    Event EventFactory::create_bad_passphrase(const std::string & pp, size_t client_index) {
        
        Event rv(Event::Type::BadPassphrase);

        rv.m_string = pp;

        rv.bad_passphrase.client_index = client_index;

        return rv;

        }

    } // End Rigel namespace