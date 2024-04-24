// Copyright 2024 Jovan Batnozic. Released under MS-PL licence in Serbia.
// See https://github.com/jbatnozic/Hobgoblin?tab=readme-ov-file#licence

#ifndef UHOBGOBLIN_UTIL_FLOW_FIELD_SPOOLER_HPP
#define UHOBGOBLIN_UTIL_FLOW_FIELD_SPOOLER_HPP

#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/HGExcept.hpp>
#include <Hobgoblin/Utility/Flow_field_calculator.hpp>

#include <Hobgoblin/Private/Pmacro_define.hpp>

HOBGOBLIN_NAMESPACE_BEGIN
namespace util {

class FlowFieldSpooler {
public:

private:
};

/*
- Have N "Stations" (N = number of worker threads)
- Each Station has its own FlowFieldCalculator

WORKER THREAD FLOW:
- If idle:
    - If there is a free Station and an outstanding request waiting,
      assign the request to that Station and start working
    - If there are no free Stations, help one of them finish its work
    - If there are free Stations but no outstanding requests, try to help
      an occupied Station finish its work
    - If there are no occupied Stations and no outstanding requests, wait

- If working:

*/

} // namespace util
HOBGOBLIN_NAMESPACE_END

#include <Hobgoblin/Private/Pmacro_define.hpp>

#endif // !UHOBGOBLIN_UTIL_FLOW_FIELD_SPOOLER_HPP
