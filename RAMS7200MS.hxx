/** © Copyright 2023 CERN
 *
 * This software is distributed under the terms of the
 * GNU Lesser General Public Licence version 3 (LGPL Version 3),
 * copied verbatim in the file “LICENSE”
 *
 * In applying this licence, CERN does not waive the privileges
 * and immunities granted to it by virtue of its status as an
 * Intergovernmental Organization or submit itself to any jurisdiction.
 *
 * Author: Alexandru Savulescu (HSE)
 *
 **/

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "Common/S7Utils.hxx"
#include "Common/Constants.hxx"
#include <tuple>
#include "CharString.hxx"

using toDPTriple = std::tuple<CharString, uint16_t, char*>;
using queueToDPCallback = std::function<void(std::vector<toDPTriple>&&)>;

struct RAMS7200MSVar
{
    RAMS7200MSVar(std::string varName, int pollTime, TS7DataItem type);

    const std::string varName;
    const uint32_t pollTime;
    std::chrono::steady_clock::time_point lastPollTime{std::chrono::steady_clock::now()};
    TS7DataItem _toPlc;
    TS7DataItem _toDP;
    bool _isString{false};
   
};

struct DPInfo
{
    // DP info
    const std::string dpAddress;
    const std::string plcAddress;
    const int dpSize;
};

class RAMS7200MS
{
    public:
        RAMS7200MS() = delete;
        RAMS7200MS(std::string ip): _ip(ip) {};
        RAMS7200MS(const RAMS7200MS&) = delete;
        RAMS7200MS& operator=(const RAMS7200MS&) = delete;
        RAMS7200MS(RAMS7200MS&& other) noexcept : _ip(other._ip) {
            if(this == &other) return;
            vars = std::move(other.vars);
            _run = other._run.load();
        }
        RAMS7200MS& operator=(RAMS7200MS&& other) = delete;
        ~RAMS7200MS() = default;
    protected:    
        void addVar(std::string varName, int pollTime);
        void removeVar(std::string varName);
        const std::string _ip; 
        
        void queuePLCItem(const std::string& varName, void* item);
        inline bool isEmpty() const {return vars.empty();}
    private: 
        std::unordered_map<std::string, RAMS7200MSVar> vars;
        std::atomic<bool> _run{false};
        std::mutex _rwmutex;
        bool previouslyConnected{false};
        std::mutex _threadMutex;
        std::condition_variable _threadCv;

    friend class RAMS7200LibFacade;
    friend class RAMS7200HWService;
    friend class RAMS7200HWMapper;
};
