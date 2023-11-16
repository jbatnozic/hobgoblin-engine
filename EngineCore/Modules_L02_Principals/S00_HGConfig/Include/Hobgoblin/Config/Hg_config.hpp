#ifndef UHOBGOBLIN_CONFIG_HG_CONFIG_HPP
#define UHOBGOBLIN_CONFIG_HG_CONFIG_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>

#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>
HOBGOBLIN_NAMESPACE_BEGIN

//! Thrown when parsing of a HGConfig stream (string or file) fails due to a syntax error.
class HGConfigParseError : public TracedRuntimeError {
public:
    using TracedRuntimeError::TracedRuntimeError;
};

//! Thrown when interpreting of a HGConfig stream (string or file) fails due to a semantic error, such
//! as an entry defined without a section, without a descriptor, with an invalid value and similar.
class HGConfigSemanticError : public TracedRuntimeError {
public:
    using TracedRuntimeError::TracedRuntimeError;
};

class HGConfig {
public:
    //! Describes what kind of data is expected as the value of the entry.
    enum class EntryKind {
        Unchecked, //!< Value can be any string. Supports 'min-length' and 'max-length' extra descriptors.
        OneOf,     //!< Value can be only 1 of the options given as main descriptor arguments.
        Regex,     //!< Value must fit the regex given as the argument 0 of the main descriptor. Supports 'min-length' and 'max-length' extra descriptors.
        Integer,   //!< Value must be an integral value (64 bit, signed). Supports 'min-value' and 'max-value' extra descriptors.
        Real       //!< Value must be a floating-point value (64 bit). Supports 'min-value' and 'max-value' extra descriptors.
    };

    //! Determines the kind of data (if first/main descriptor) or additional constraints
    //! for the data (descriptors other than the main one) that is the value of the entry.
    struct EntryDescriptor {
        std::string label;
        std::vector<std::string> args;
    };

    //! A single entry in the configuration with a variable number of descriptors.
    struct Entry {
        //! The kind of the entry as determined by the label of the main descriptor.
        EntryKind kind{};
        EntryDescriptor mainDescriptor;
        std::vector<EntryDescriptor> extraDescriptors;
        std::string value;
    };

    ///////////////////////////////////////////////////////////////////////////
    // CONSTRUCTION                                                          //
    ///////////////////////////////////////////////////////////////////////////

    HGConfig();

    explicit HGConfig(std::istream& aInputStream);

    explicit HGConfig(const std::string& aInputString);

    HGConfig(const HGConfig& aOther) = delete;

    HGConfig(HGConfig&& aOther) noexcept;

    ~HGConfig();

    HGConfig& operator=(const HGConfig& aOther) = delete;

    HGConfig& operator=(HGConfig&& aOther) noexcept;

    ///////////////////////////////////////////////////////////////////////////
    // READING ENTRIES                                                       //
    ///////////////////////////////////////////////////////////////////////////

    bool sectionExists(const std::string& aSectionName) const;

    bool entryExists(const std::string& aSectionName, const std::string& aEntryName) const;

    std::string getRawValue(const std::string& aSectionName, const std::string& aEntryName) const;

    std::string getStringValue(const std::string& aSectionName, const std::string& aEntryName) const;

    template <class taType = std::int64_t>
    taType getIntegerValue(const std::string& aSectionName,
                           const std::string& aEntryName,
                           PZInteger aRadix = 10) const;

    template <class taType = double>
    taType getRealValue(const std::string& aSectionName, const std::string& aEntryName) const;

    const Entry& getEntry(const std::string& aSectionName, const std::string& aEntryName) const;

    // TODO - iterate over all sections/entries

    ///////////////////////////////////////////////////////////////////////////
    // MODIFYING THE CONFIG                                                  //
    ///////////////////////////////////////////////////////////////////////////

    // TODO - change values, change whole entries, insert/remove entries and sections

    ///////////////////////////////////////////////////////////////////////////
    // STATE DUMPING                                                         //
    ///////////////////////////////////////////////////////////////////////////

    //! Returns a string representing the whole state of the config object. The string will be in
    //! the proper HGConfig format so another HGConfig object can be constructed from it.
    std::string dumpState() const;

private:
    class Impl;
    std::unique_ptr<Impl> _impl;

    std::int64_t _getSignedIntegerValueImpl(const std::string& aSectionName,
                                            const std::string& aEntryName,
                                            PZInteger aRadix) const;

    std::uint64_t _getUnsignedIntegerValueImpl(const std::string& aSectionName,
                                               const std::string& aEntryName,
                                               PZInteger aRadix) const;

    double _getRealValueImpl(const std::string& aSectionName, const std::string& aEntryName) const;
};

template <class taType>
taType HGConfig::getIntegerValue(const std::string& aSectionName,
                                 const std::string& aEntryName,
                                 PZInteger aRadix) const {
    if constexpr (std::is_signed<taType>::value) {
        return static_cast<taType>(_getSignedIntegerValueImpl(aSectionName, aEntryName, aRadix));
    } else {
        return static_cast<taType>(_getUnsignedIntegerValueImpl(aSectionName, aEntryName, aRadix));
    }
}

template <class taType>
taType HGConfig::getRealValue(const std::string& aSectionName, const std::string& aEntryName) const {
    return static_cast<taType>(_getRealValueImpl(aSectionName, aEntryName));
}

HOBGOBLIN_NAMESPACE_END
#include <Hobgoblin/Private/Pmacro_undef.hpp>
#include <Hobgoblin/Private/Short_namespace.hpp>

#endif // !UHOBGOBLIN_CONFIG_HG_CONFIG_HPP
