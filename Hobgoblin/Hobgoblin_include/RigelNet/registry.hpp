#pragma once

#include "Rigel-config.hpp"
#include "Rigel-types.hpp"

#include <string>
#include <unordered_map>

namespace RIGELNETW_CONFIG_NS_NAME {

    class HandlerMgr;
    class TCPServer;
    class TCPClient;   

    #define String std::string

    //////////////////////////////////////////////////
    typedef unsigned short RegPermission;

    const RegPermission REG_RDONLY = (0);

    const RegPermission REG_WR_INT = (1 << 0);
    const RegPermission REG_WR_DBL = (1 << 2);
    const RegPermission REG_WR_STR = (1 << 4);

    const RegPermission REG_WR_ANY = REG_WR_INT +
                                     REG_WR_DBL +
                                     REG_WR_STR ;

    const RegPermission REG_RM_INT = (1 << 1);
    const RegPermission REG_RM_DBL = (1 << 3);
    const RegPermission REG_RM_STR = (1 << 5);

    const RegPermission REG_RM_ANY = REG_RM_INT +
                                     REG_RM_DBL +
                                     REG_RM_STR ;

    const RegPermission REG_ERROR  = (1 << 7);
    //////////////////////////////////////////////////

    class Registry {
        
        friend class HandlerMgr;
        friend class TCPServer;
        friend class TCPClient;

        private:

            std::unordered_map<String, Int32>  map_int;
            std::unordered_map<String, Double> map_dbl;
            std::unordered_map<String, String> map_str;

        public:

            void set_int(const String& key, const Int32   val);
            void set_dbl(const String& key, const Double  val);
            void set_str(const String& key, const String &val);

            void del_int(const String& key);
            void del_dbl(const String& key);
            void del_str(const String& key);

            void clear_int();
            void clear_dbl();
            void clear_str();
            void clear_all();

            bool int_exists(const String& key) const;
            bool dbl_exists(const String& key) const;
            bool str_exists(const String& key) const;

            Int32  get_int(const String& key) const;
            Double get_dbl(const String& key) const;
            String get_str(const String& key) const;

            bool sget_int(const String& key, Int32  &val) const;
            bool sget_dbl(const String& key, Double &val) const;
            bool sget_str(const String& key, String &val) const;

        };
    
    #undef String

    } //End Rigel namespace
