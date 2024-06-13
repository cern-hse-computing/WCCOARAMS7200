/** © Copyright 2022 CERN
 *
 * This software is distributed under the terms of the
 * GNU Lesser General Public Licence version 3 (LGPL Version 3),
 * copied verbatim in the file “LICENSE”
 *
 * In applying this licence, CERN does not waive the privileges
 * and immunities granted to it by virtue of its status as an
 * Intergovernmental Organization or submit itself to any jurisdiction.
 *
 * Author: Adrien Ledeul (HSE)
 *
 **/

#ifndef UTILS_HXX
#define UTILS_HXX


#include <string>
#include <algorithm>
#include <iterator>
#include <vector>
#include <future>
#include <chrono>
#include <iostream>
#include <cstring>

template <class T>
std::ostream& operator << (std::ostream& os, const std::vector<T>& iterable)
{
    for (auto & i : iterable)
        os << i << " ";
    return os;
}


namespace Common {

using std::future;
using std::future_status;
using std::cout;
using std::exception;
using std::endl;

class Utils
{
public:

    Utils() = delete;

    static auto split(const std::string& str, char delimiter = '$') -> std::vector<std::string>
    {
        std::vector<std::string> result;
        std::size_t previous = 0;
        std::size_t current = 0;

        while ((current = str.find(delimiter, previous))!= std::string::npos) {
            result.emplace_back(str.substr(previous, current - previous));
            previous = current + 1;
        }

        result.emplace_back(str.substr(previous));

        return result;
    }

    template <typename T>
    static T CopyNSwapBytes(const T& value)
    {
        T retVal;
        std::memcpy(reinterpret_cast<void*>(&retVal), reinterpret_cast<const void*>(&value), sizeof(T));
        std::reverse(reinterpret_cast<uint8_t*>(&retVal), reinterpret_cast<uint8_t*>(&retVal) + sizeof(T));
        return retVal;
    }

    template <typename T>
    static T CopyNSwapBytes(const void* value)
    {
        return CopyNSwapBytes(*reinterpret_cast<const T*>(value));
    }
    
    template <typename T>
    static T CopyNSwapBytes(const char* value)
    {
        return CopyNSwapBytes(*static_cast<const T*>(static_cast<const void*>(value)));
    }
 

};

}
#endif // UTILS_HXX
