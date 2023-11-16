
#include <Hobgoblin/Config/Hg_config.hpp>

#include <Hobgoblin/Format.hpp>

#include <algorithm>
#include <cassert>
#include <cctype>
#include <list>
#include <regex>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <Hobgoblin/Private/Pmacro_define.hpp>
HOBGOBLIN_NAMESPACE_BEGIN

#define FAIL_PARSE(_msg_) \
    HG_THROW_TRACED(HGConfigParseError, 0, _msg_)

#define FAIL_PARSE_FMT(...) \
    HG_THROW_TRACED(HGConfigParseError, 0, __VA_ARGS__)

#define FAIL_SEMANTIC(...) \
    HG_THROW_TRACED(HGConfigSemanticError, 0, __VA_ARGS__)

namespace {
using EntryKind = HGConfig::EntryKind;

enum class LineKind {
    EmptyOrComment,
    SectionHeader,
    Entry
};
using LineList = std::list<std::pair<std::string, LineKind>>;

using EntryDescriptor = HGConfig::EntryDescriptor;

struct EntryControlBlock : HGConfig::Entry {
    LineList* lineList = nullptr;
    LineList::iterator firstLine;
    LineList::iterator lastLine;

    EntryControlBlock() = default;

    EntryControlBlock(const EntryControlBlock&) = delete;
    EntryControlBlock& operator=(const EntryControlBlock&) = delete;

    EntryControlBlock(EntryControlBlock&& aOther) {
        SELF = std::move(aOther);
    }

    EntryControlBlock& operator=(EntryControlBlock&& aOther) {
        if (&aOther != this) {
            kind = aOther.kind;
            mainDescriptor = std::move(aOther.mainDescriptor);
            extraDescriptors = std::move(aOther.extraDescriptors);
            value = std::move(aOther.value);
            lineList = aOther.lineList;
            firstLine = aOther.firstLine;
            lastLine = aOther.lastLine;

            aOther.lineList = nullptr;
        }
        return SELF;
    }

    // This D-tor is not virtual but it's not a problem because
    // we don't hold a reference to the base class anywhere.
    ~EntryControlBlock() {
        if (lineList) {
            lineList->erase(firstLine, lastLine);
            lineList = nullptr;
        }
    }
};

struct SectionControlBlock {
    LineList::iterator line;
    std::unordered_map<std::string, EntryControlBlock> entries;
};

EntryKind StringToEntryKind(const std::string& aString) {
    if (aString == "unchecked")  return EntryKind::Unchecked;
    if (aString == "one-of")     return EntryKind::OneOf;
    if (aString == "regex")      return EntryKind::Regex;
    if (aString == "integer")    return EntryKind::Integer;
    if (aString == "real")       return EntryKind::Real;

    FAIL_SEMANTIC("Unknown entry type <<{}>>", aString);
}

const std::vector<std::string>& ALLOWED_DESCRIPTORS_FOR_UNCHECKED = {"min-length", "max-length"};
const std::vector<std::string>& ALLOWED_DESCRIPTORS_FOR_REGEX     = {"min-length", "max-length"};
const std::vector<std::string>& ALLOWED_DESCRIPTORS_FOR_INTEGER   = {"min-value", "max-value"};
const std::vector<std::string>& ALLOWED_DESCRIPTORS_FOR_REAL      = {"min-value", "max-value"};

void CheckExtraDescriptors(const EntryControlBlock& aEntry,
                           const std::vector<std::string>& aAllowedDescriptors) {
    const auto isAllowed = [&](const std::string& aLabel) {
        return std::any_of(aAllowedDescriptors.begin(), 
                           aAllowedDescriptors.end(),
                           [&](const std::string& aL) {
                               return aL == aLabel;
                           });
    };

    for (const auto& desc : aEntry.extraDescriptors) {
        if (!isAllowed(desc.label)) {
            FAIL_SEMANTIC("entry of kind <<{}>> doesn't support extra descriptor <<{}>>",
                              aEntry.mainDescriptor.label,
                              desc.label);
        }

        // Currently all extra descriptors support exactly 1 parameter
        if (desc.args.size() != 1) {
            FAIL_SEMANTIC("extra descriptor <<{}>> must have exactly 1 parameter", desc.label);
        }

        if (desc.label == "min-length") {
            if (static_cast<long long>(aEntry.value.size()) < std::stoll(desc.args[0])) {
                FAIL_SEMANTIC("value <<{}>> is shorter than the minimum allowed value ({}).",
                                  aEntry.value,
                                  desc.args[0]);
            }
        }
        else if (desc.label == "max-length") {
            if (static_cast<long long>(aEntry.value.size()) > std::stoll(desc.args[0])) {
                FAIL_SEMANTIC("value <<{}>> is longer than the maximum allowed value ({}).",
                                  aEntry.value,
                                  desc.args[0]);
            }
        }
        else if (desc.label == "min-value") {
            if (aEntry.mainDescriptor.label == "integer") {
                if (std::stoll(aEntry.value) < std::stoll(desc.args[0])) {
                    FAIL_SEMANTIC("value <<{}>> is lower than the minimum allowed value ({}).",
                                      aEntry.value,
                                      desc.args[0]);
                }
            }
            else { // otherwise it must be real
                if (std::stod(aEntry.value) < std::stod(desc.args[0])) {
                    FAIL_SEMANTIC("value <<{}>> is lower than the minimum allowed value ({}).",
                                      aEntry.value,
                                      desc.args[0]);
                }
            }
        }
        else if (desc.label == "max-value") {
            if (aEntry.mainDescriptor.label == "integer") {
                if (std::stoll(aEntry.value) > std::stoll(desc.args[0])) {
                    FAIL_SEMANTIC("value <<{}>> is over the maximum allowed value ({}).",
                                      aEntry.value,
                                      desc.args[0]);
                }
            }
            else { // otherwise it must be real
                if (std::stod(aEntry.value) > std::stod(desc.args[0])) {
                    FAIL_SEMANTIC("value <<{}>> is over the maximum allowed value ({}).",
                                      aEntry.value,
                                      desc.args[0]);
                }
            }
        }
    }
}

void ValidateEntry(const EntryControlBlock& aEntry) {
    switch (aEntry.kind) {
        case EntryKind::Unchecked:
            CheckExtraDescriptors(aEntry, ALLOWED_DESCRIPTORS_FOR_UNCHECKED);
            break;

        case EntryKind::OneOf:
            if (!aEntry.extraDescriptors.empty()) {
                FAIL_SEMANTIC("descriptor one-of doesn't support any extra descriptors",
                                  aEntry.value);
            }
            for (const auto& arg : aEntry.mainDescriptor.args) {
                if (aEntry.value == arg) {
                    return;
                }
            }
            FAIL_SEMANTIC("value <<{}>> does not match any of the one-of options",
                              aEntry.value);

        case EntryKind::Regex:
            {
                try {
                    if (aEntry.mainDescriptor.args.size() != 1) {
                        FAIL_SEMANTIC("regex descriptor must have exactly 1 parameter - the regex pattern");
                    }
                    const std::regex regex{aEntry.mainDescriptor.args[0]};
                    if (std::regex_match(aEntry.value, regex)) {
                        CheckExtraDescriptors(aEntry, ALLOWED_DESCRIPTORS_FOR_REGEX);
                        return;
                    }
                    FAIL_SEMANTIC("value <<{}>> does not match regex pattern <<{}>>",
                                      aEntry.value,
                                      aEntry.mainDescriptor.args[0]);
                } catch (const std::exception& ex) {
                    FAIL_SEMANTIC("failed to apply regex <<{}>> with error <<{}>> (check pattern syntax)",
                                      aEntry.mainDescriptor.args[0],
                                      ex.what());
                }
            }


        case EntryKind::Integer:
            {
                std::string value = aEntry.value;
                value.erase(std::remove_if(value.begin(), value.end(),
                                           [](char aChar) {
                                               return std::isspace(aChar);
                                           }), value.end());
                if (value.empty()) {
                    FAIL_PARSE("value for integer can't be empty");
                }
                try {
                    std::size_t idx;
                    if (value[0] == '-') {
                        static_cast<void>(std::stoll(value, &idx, 0));
                    } else {
                        static_cast<void>(std::stoull(value, &idx, 0));
                    }
                    if (idx != value.size()) {
                        FAIL_PARSE("unexpected characters found while parsing");
                    }
                } catch (const std::exception& ex) {
                    FAIL_PARSE_FMT("could not parse integer from string <<{}>> reason: <<{}>>",
                                   aEntry.value,
                                   ex.what());
                }
                CheckExtraDescriptors(aEntry, ALLOWED_DESCRIPTORS_FOR_INTEGER);
            }
            break;

        case EntryKind::Real:
            {
                std::string value = aEntry.value;
                value.erase(std::remove_if(value.begin(), value.end(),
                                           [](char aChar) {
                                                return std::isspace(aChar);
                                           }), value.end());
                if (value.empty()) {
                    FAIL_PARSE("value for real can't be empty");
                }
                try {
                    std::size_t idx;
                    static_cast<void>(std::stod(value, &idx));
                    if (idx != value.size()) {
                        FAIL_PARSE("unexpected characters found while parsing");
                    }
                } catch (const std::exception& ex) {
                    FAIL_PARSE_FMT("could not parse real from string <<{}>> reason: <<{}>>",
                                   aEntry.value,
                                   ex.what());
                }
                CheckExtraDescriptors(aEntry, ALLOWED_DESCRIPTORS_FOR_REAL);
            }
            break;

        default:
            assert(false && "Unreachable");
    }
}

} // namespace

///////////////////////////////////////////////////////////////////////////
// IMPL                                                                  //
///////////////////////////////////////////////////////////////////////////

class HGConfig::Impl {
public:
    explicit Impl() = default;

    explicit Impl(std::istream& aInputStream) {
        _loadFromStream(aInputStream);
    }

    explicit Impl(const std::string& aInputString) {
        std::stringstream ss{aInputString};
        _loadFromStream(ss);
    }

    bool sectionExists(const std::string& aSectionName) const {
        return (_sections.find(aSectionName) != _sections.end());
    }

    bool entryExists(const std::string& aSectionName, const std::string& aEntryName) const {
        return (_sections.at(aSectionName).entries.find(aEntryName) !=
                _sections.at(aSectionName).entries.end());
    }

    std::string getRawValue(const std::string& aSectionName, const std::string& aEntryName) const {
        return getEntry(aSectionName, aEntryName).value;
    }

    const Entry& getEntry(const std::string& aSectionName, const std::string& aEntryName) const {
        return _sections.at(aSectionName).entries.at(aEntryName);
    }

    std::string getStringValue(const std::string& aSectionName, const std::string& aEntryName) const {
        return getRawValue(aSectionName, aEntryName);
    }

    std::int64_t getSignedIntegerValue(const std::string& aSectionName,
                                       const std::string& aEntryName,
                                       PZInteger aRadix) const {
        return std::stoll(getRawValue(aSectionName, aEntryName), nullptr, aRadix);
    }

    std::uint64_t getUnsignedIntegerValue(const std::string& aSectionName,
                                          const std::string& aEntryName,
                                          PZInteger aRadix) const {
        return std::stoull(getRawValue(aSectionName, aEntryName), nullptr, aRadix);
    }

    double getRealValue(const std::string& aSectionName, const std::string& aEntryName) const {
        return std::stod(getRawValue(aSectionName, aEntryName));
    }

    // TODO - change values, change whole entries, insert/remove entries and sections

    std::string dumpState() const {
        std::stringstream ss;

        for (auto iter = _lines.begin(); iter != _lines.end(); ++iter) {
            ss << iter->first;
            if (iter != std::prev(_lines.end())) {
                ss << '\n';
            }
        }

        return ss.str();
    }

private:
    LineList _lines;
    std::unordered_map<std::string, SectionControlBlock> _sections;

    const std::regex _emptyLineOrCommentRegex{R"_(^\s*(//.*)?$)_"};
    const std::regex _sectionHeaderRegex{R"_(^\s*\[\[\s*([-_a-zA-Z0-9]+)\s*\]\]\s*$)_"};

    void _loadFromStream(std::istream& aInputStream) {
        struct {
            LineList::iterator beginning;
            LineList::iterator ending;
            std::string text;
        } semanticLine;

        bool inMultilineConstruct = false;

        std::string currentSection;
        std::string currentLine;

        PZInteger currentLineNum = 0;

        try {
            for (; std::getline(aInputStream, currentLine); currentLineNum += 1) {
                const auto lineKind = _identifyLine(currentLine, currentSection);
                _lines.push_back({currentLine, lineKind});
                if (!inMultilineConstruct) {
                    semanticLine.beginning = std::prev(_lines.end());
                }

                semanticLine.text += currentLine;
                if (!semanticLine.text.empty() && semanticLine.text.back() == '\\') {
                    semanticLine.text.pop_back();
                    inMultilineConstruct = true;
                    continue;
                }

                semanticLine.ending = std::prev(_lines.end());
                inMultilineConstruct = false;

                // First ID was for the current line, now we identify the whole assembled (semantic) line
                // TODO don't reevaluate if not in multiline thing
                switch (_identifyLine(semanticLine.text, currentSection)) {
                case LineKind::EmptyOrComment:
                    if (semanticLine.beginning != semanticLine.ending) {
                        FAIL_PARSE_FMT("\\ found in line {} <<{}>> - "
                                       "using a backslash to break a line is not allowed for comments.",
                                       currentLineNum,
                                       currentLine);
                    }
                    semanticLine.text.clear();
                    break;

                case LineKind::SectionHeader:
                    if (semanticLine.beginning != semanticLine.ending) {
                        FAIL_PARSE_FMT("\\ found in line {} <<{}>> - "
                                       "using a backslash to break a line is not allowed for section headers.",
                                       currentLineNum,
                                       currentLine);
                    }
                    _processSectionHeader(currentSection, semanticLine.beginning);
                    semanticLine.text.clear();
                    break;

                case LineKind::Entry:
                    _processEntry(currentSection, semanticLine.text, semanticLine.beginning, semanticLine.ending);
                    semanticLine.text.clear();
                    break;

                default:
                    assert(false && "Unreachable!");
                }
            }
        } catch (const HGConfigSemanticError& ex) {
            FAIL_SEMANTIC("Semantic error: in line ({}) with message: {}", currentLineNum, ex.what());
        } catch (const HGConfigParseError& ex) {
            FAIL_PARSE_FMT("Parse error: in line ({}) with message: {}", currentLineNum, ex.what());
        } catch (const std::exception& ex) {
            // We can treat generic C++ errors as parse errors because they are most likely
            // caused by errors in the format.
            FAIL_SEMANTIC("Parse error: in line ({}) with message: {}", currentLineNum, ex.what());
        }
    }

    //! If the returned value is LineKind::SectionHeader, aOutSectionName will also be changed.
    LineKind _identifyLine(const std::string& aLine, std::string& aOutSectionName) const {
        if (std::regex_match(aLine, _emptyLineOrCommentRegex)) {
            return LineKind::EmptyOrComment;
        }
        std::smatch smatch;
        if (std::regex_match(aLine, smatch, _sectionHeaderRegex)) {
            aOutSectionName = smatch[1];
            return LineKind::SectionHeader;
        }
        return LineKind::Entry;
    }

    void _processSectionHeader(const std::string& aSectionName, LineList::iterator aLineIter) {
        if (_sections.find(aSectionName) != _sections.end()) {
            FAIL_SEMANTIC("multiple definitions found for section <<{}>>.", aSectionName);
        }
        
        SectionControlBlock& sectionControlBlock = _sections[aSectionName];
        sectionControlBlock.line = aLineIter;
    }

    void _processEntry(
        const std::string& aCurrentSection,
        const std::string& aLine,
        LineList::iterator aFirstLineIter,
        LineList::iterator aLastLineIter
    ) {
        // PART 1: Find section

        if (aCurrentSection.empty()) {
            FAIL_SEMANTIC("entry defined in line <<{}>> but no section was declared prior.", aLine);
        }
        const auto sectionIter = _sections.find(aCurrentSection);
        assert(sectionIter != _sections.end());

        SectionControlBlock& sectionControlBlock = sectionIter->second;

        // PART 2: Parse entry

        struct {
            std::string name;
            EntryDescriptor mainDescriptor;
            std::vector<EntryDescriptor> extraDescriptors;
            std::string value;
        } entryInfo;

        enum class State {
            SearchingForName,
            ReadingName,
            SearchingForMainDescriptor,
            SearchingForExtraDescriptorOrColon,
            SearchingForEqualsSign,
            SearchingForValue,
            ReadingValue,
            SearchingForEOL
        } state = State::SearchingForName;

        std::string stack;

        for (std::size_t i = 0; i < aLine.size(); i += 1) {
            const char c = aLine[i];
            switch (state) {
                case State::SearchingForName:
                    if (std::isspace(c)) {
                        continue;
                    }
                    if (std::isalpha(c) || c == '_') {
                        stack.push_back(c);
                        state = State::ReadingName;
                        continue;
                    }
                    FAIL_PARSE_FMT("Unexpected character <<{}>> found in line <<{}>> (entry name expected).",
                                   c,
                                   aLine);
                    
                case State::ReadingName:
                    if (std::isalnum(c) || c == '-' || c == '_') {
                        stack.push_back(c);
                        continue;
                    }
                    if (std::isspace(c)) {
                        entryInfo.name = std::move(stack);
                        stack.clear();
                        state = State::SearchingForMainDescriptor;
                        continue;
                    }
                    if (c == '[') {
                        entryInfo.name = std::move(stack);
                        stack.clear();
                        i -= 1;
                        state = State::SearchingForMainDescriptor;
                        continue;
                    }
                    FAIL_PARSE_FMT("Unexpected character <<{}>> found in line <<{}>> (entry name expected).",
                                   c,
                                   aLine);

                case State::SearchingForMainDescriptor:
                    if (std::isspace(c)) {
                        continue;
                    }
                    if (c == '[') {
                        entryInfo.mainDescriptor = _parseDescriptor(aLine, i);
                        state = State::SearchingForExtraDescriptorOrColon;
                        continue;
                    }
                    FAIL_PARSE_FMT("Unexpected character <<{}>> found in line <<{}>> ([ expected).",
                                   c,
                                   aLine);

                case State::SearchingForExtraDescriptorOrColon:
                    if (std::isspace(c)) {
                        continue;
                    }
                    if (c == ':') {
                        state = State::SearchingForEqualsSign;
                        continue;
                    }
                    if (c == '[') {
                        entryInfo.extraDescriptors.emplace_back(_parseDescriptor(aLine, i));
                        state = State::SearchingForExtraDescriptorOrColon;
                        continue;
                    }
                    FAIL_PARSE_FMT("Unexpected character <<{}>> found in line <<{}>> ([ or := expected).",
                                   c,
                                   aLine);

                case State::SearchingForEqualsSign:
                    if (c == '=') {
                        state = State::SearchingForValue;
                        continue;
                    }
                    FAIL_PARSE_FMT("Unexpected character <<{}>> found in line <<{}>> (:= expected).",
                                   c,
                                   aLine);

                case State::SearchingForValue:
                    if (std::isspace(c)) {
                        continue;
                    }
                    if (c == '"') {
                        state = State::ReadingValue;
                        continue;
                    }
                    FAIL_PARSE_FMT("Unexpected character <<{}>> found in line <<{}>> (value expected).",
                                   c,
                                   aLine);

                case State::ReadingValue:
                    if (c == '"') {
                        if (!stack.empty() && stack.back() == '\\') {
                            stack.pop_back();
                            stack.push_back('"');
                        } else {
                            entryInfo.value = std::move(stack);
                            stack.clear();

                            if (i == aLine.size() - 1) {
                                goto HAPPY_END;
                            } else {
                                state = State::SearchingForEOL;
                                continue;
                            }
                        }
                    } else {
                        stack.push_back(c);
                    }
                    break;

                case State::SearchingForEOL:
                    if (i == aLine.size() - 1) {
                        goto HAPPY_END;
                    }
                    if (std::isspace(c)) {
                        continue;
                    }
                    FAIL_PARSE_FMT("Unexpected character <<{}>> found in line <<{}>> (EOL expected).",
                                   c,
                                   aLine);

                default:
                    assert(false && "Unreachable!");
            }
        }

        FAIL_PARSE_FMT("Unexpected EOL found in line <<{}>> (full entry expected).", aLine);

        // PART 3: Validate & insert entry

        HAPPY_END: {
            EntryControlBlock entryControlBlock;
            entryControlBlock.lineList = &_lines;
            entryControlBlock.firstLine = aFirstLineIter;
            entryControlBlock.lastLine = aLastLineIter;
            entryControlBlock.kind = StringToEntryKind(entryInfo.mainDescriptor.label);
            entryControlBlock.mainDescriptor = std::move(entryInfo.mainDescriptor);
            entryControlBlock.extraDescriptors = std::move(entryInfo.extraDescriptors);
            entryControlBlock.value = std::move(entryInfo.value);

            ValidateEntry(entryControlBlock);

            if (sectionControlBlock.entries.find(entryInfo.name) != sectionControlBlock.entries.end()) {
                FAIL_SEMANTIC("multiple definitions found for entry <<{}>> in section <<{}>>",
                                  entryInfo.name,
                                  aCurrentSection);
            }

            sectionControlBlock.entries[entryInfo.name] = std::move(entryControlBlock);
        }
    }

    static EntryDescriptor _parseDescriptor(const std::string& aLine, std::size_t& aIndex) {
        auto& i = aIndex;
        assert(aLine[i] == '[');

        EntryDescriptor result;

        enum class State {
            SearchingForLabel,
            ReadingLabel,
            SearchingForColon,
            SearchingForParameterOrBracket,
            ReadingParameter,
            SearchingForCommaOrBracket
        } state = State::SearchingForLabel;

        std::string stack;

        for (i = i + 1; i < aLine.size(); i += 1) {
            const char c = aLine[i];
            switch (state) {
                case State::SearchingForLabel:
                    if (std::isspace(c)) {
                        continue;
                    }
                    if (std::isalpha(c) || c == '_') {
                        stack.push_back(c);
                        state = State::ReadingLabel;
                        continue;
                    }
                    FAIL_PARSE_FMT("Unexpected character <<{}>> found in line <<{}>> (descriptor label expected).",
                                   c,
                                   aLine);

                case State::ReadingLabel:
                    if (std::isalnum(c) || c == '-' || c == '_') {
                        stack.push_back(c);
                        continue;
                    }
                    if (std::isspace(c)) {
                        result.label = std::move(stack);
                        stack.clear();
                        state = State::SearchingForColon;
                        continue;
                    }
                    if (c == ':') {
                        result.label = std::move(stack);
                        stack.clear();
                        i -= 1;
                        state = State::SearchingForColon;
                        continue;
                    }
                    FAIL_PARSE_FMT("Unexpected character <<{}>> found in line <<{}>> (descriptor label expected).",
                                   c,
                                   aLine);

                case State::SearchingForColon:
                    if (std::isspace(c)) {
                        continue;
                    }
                    if (c == ':') {
                        state = State::SearchingForParameterOrBracket;
                        continue;
                    }
                    FAIL_PARSE_FMT("Unexpected character <<{}>> found in line <<{}>> (colon expected after label).",
                                   c,
                                   aLine);

                case State::SearchingForParameterOrBracket:
                    if (std::isspace(c)) {
                        continue;
                    }
                    if (c == ']') {
                        goto HAPPY_END;
                    }
                    if (c == '"') {
                        state = State::ReadingParameter;
                        continue;
                    }
                    FAIL_PARSE_FMT("Unexpected character <<{}>> found in line <<{}>> (\" or ] expected).",
                                   c,
                                   aLine);

                case State::ReadingParameter:
                    if (c == '"') {
                        if (!stack.empty() && stack.back() == '\\') {
                            stack.pop_back();
                            stack.push_back('"');
                        } else {
                            result.args.emplace_back(std::move(stack));
                            stack.clear();
                            state = State::SearchingForCommaOrBracket;
                            continue;
                        }
                    } else {
                        stack.push_back(c);
                    }
                    break;

                case State::SearchingForCommaOrBracket:
                    if (std::isspace(c)) {
                        continue;
                    }
                    if (c == ',') {
                        state = State::SearchingForParameterOrBracket;
                        continue;
                    }
                    if (c == ']') {
                        goto HAPPY_END;
                    }
                    FAIL_PARSE_FMT("Unexpected character <<{}>> found in line <<{}>> (, or ] expected).",
                                   c,
                                   aLine);

                default:
                    assert(false && "Unreachable!");
            }
        }
        
        FAIL_PARSE_FMT("Unexpected EOL found in line <<{}>> (full entry descriptor expected).", aLine);

        HAPPY_END: return result;
    }
};

///////////////////////////////////////////////////////////////////////////
// CONSTRUCTION                                                          //
///////////////////////////////////////////////////////////////////////////

HGConfig::HGConfig()
    : _impl{std::make_unique<Impl>()} {
}

HGConfig::HGConfig(std::istream& aInputStream)
    : _impl{std::make_unique<Impl>(aInputStream)} {
}

HGConfig::HGConfig(const std::string& aInputString)
    : _impl{std::make_unique<Impl>(aInputString)} {
}

HGConfig::HGConfig(HGConfig&& aOther) noexcept
    : _impl{std::move(aOther._impl)} {
}

HGConfig::~HGConfig() = default;

HGConfig& HGConfig::operator=(HGConfig&& aOther) noexcept {
    if (&aOther != this) {
        _impl = std::move(aOther._impl);
    }
    return SELF;
}

///////////////////////////////////////////////////////////////////////////
// READING ENTRIES                                                       //
///////////////////////////////////////////////////////////////////////////

bool HGConfig::sectionExists(const std::string& aSectionName) const {
    return _impl->sectionExists(aSectionName);
}

bool HGConfig::entryExists(const std::string& aSectionName, const std::string& aEntryName) const {
    return _impl->entryExists(aSectionName, aEntryName);
}

std::string HGConfig::getRawValue(const std::string& aSectionName, const std::string& aEntryName) const {
    return _impl->getRawValue(aSectionName, aEntryName);
}

const HGConfig::Entry& HGConfig::getEntry(const std::string& aSectionName, const std::string& aEntryName) const {
    return _impl->getEntry(aSectionName, aEntryName);
}

std::string HGConfig::getStringValue(const std::string& aSectionName, const std::string& aEntryName) const {
    return _impl->getStringValue(aSectionName, aEntryName);
}

///////////////////////////////////////////////////////////////////////////
// MODIFYING THE CONFIG                                                  //
///////////////////////////////////////////////////////////////////////////

// TODO

///////////////////////////////////////////////////////////////////////////
// STATE DUMPING                                                         //
///////////////////////////////////////////////////////////////////////////

std::string HGConfig::dumpState() const {
    return _impl->dumpState();
}

///////////////////////////////////////////////////////////////////////////
// PRIVATE                                                               //
///////////////////////////////////////////////////////////////////////////

std::int64_t HGConfig::_getSignedIntegerValueImpl(const std::string& aSectionName,
                                                  const std::string& aEntryName,
                                                  PZInteger aRadix) const {
    return _impl->getSignedIntegerValue(aSectionName, aEntryName, aRadix);
}

std::uint64_t HGConfig::_getUnsignedIntegerValueImpl(const std::string& aSectionName,
                                                     const std::string& aEntryName,
                                                     PZInteger aRadix) const {
    return _impl->getUnsignedIntegerValue(aSectionName, aEntryName, aRadix);
}

double HGConfig::_getRealValueImpl(const std::string& aSectionName, const std::string& aEntryName) const {
    return _impl->getRealValue(aSectionName, aEntryName);
}

HOBGOBLIN_NAMESPACE_END
#include <Hobgoblin/Private/Pmacro_undef.hpp>
