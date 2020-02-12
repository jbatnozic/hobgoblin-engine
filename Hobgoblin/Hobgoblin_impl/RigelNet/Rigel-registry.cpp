
#include "Rigel-config.hpp"
#include "Rigel-types.hpp"
#include "Rigel-registry.hpp"

#include <string>

namespace RIGELNETW_CONFIG_NS_NAME {

    #define String std::string

    void Registry::set_int(const String& key, const Int32  val) {

        map_int[key] = val;

        }

    void Registry::set_dbl(const String& key, const Double val) {

        map_dbl[key] = val;

        }

    void Registry::set_str(const String& key, const String& val) {

        map_str[key] = val;

        }

    void Registry::del_int(const String& key) {
        
        map_int.erase(key);

        }

    void Registry::del_dbl(const String& key) {
        
        map_dbl.erase(key);

        }

    void Registry::del_str(const String& key) {
        
        map_str.erase(key);

        }

    void Registry::clear_int() {
        
        map_int.clear();

        }

    void Registry::clear_dbl() {
        
        map_dbl.clear();

        }

    void Registry::clear_str() {
        
        map_str.clear();

        }

    void Registry::clear_all() {
        
        map_int.clear();
        map_dbl.clear();
        map_str.clear();

        }

    bool Registry::int_exists(const String& key) const {
    
        return (map_int.count(key) > 0);
    
        }

    bool Registry::dbl_exists(const String& key) const {

        return (map_dbl.count(key) > 0);

        }

    bool Registry::str_exists(const String& key) const {

        return (map_str.count(key) > 0);

        }

    Int32  Registry::get_int(const String& key) const {
    
        if (!int_exists(key)) return 0;

        return map_int.at(key);

        }

    Double Registry::get_dbl(const String& key) const {

        if (!dbl_exists(key)) return 0.0;

        return map_dbl.at(key);

        }

    String Registry::get_str(const String& key) const {

        if (!str_exists(key)) return "";

        return map_str.at(key);

        }

    bool Registry::sget_int(const String& key, Int32  &val) const {
        
        auto iter = map_int.find(key);

        if (iter == map_int.end()) return false;

        val = (*iter).second;

        return true;

        }

    bool Registry::sget_dbl(const String& key, Double &val) const {
        
        auto iter = map_dbl.find(key);

        if (iter == map_dbl.end()) return false;

        val = (*iter).second;

        return true;

        }

    bool Registry::sget_str(const String& key, String &val) const {
        
        auto iter = map_str.find(key);

        if (iter == map_str.end()) return false;

        val = (*iter).second;

        return true;

        }

    #undef String

    } // End Rigel namespace