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

#ifndef RAMS7200LIBFACADE_HXX
#define RAMS7200LIBFACADE_HXX

#define OVERHEAD_READ_MESSAGE 13
#define OVERHEAD_READ_VARIABLE 5
#define OVERHEAD_WRITE_MESSAGE 24
#define OVERHEAD_WRITE_VARIABLE 16
#define PDU_SIZE 240

#include <string>
#include <chrono>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <map>
#include <functional>
#include <memory>
#include <unordered_set>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "RAMS7200MS.hxx"
#include "Common/Logger.hxx"


/**
 * @brief The RAMS7200LibFacade class is a facade and encompasses all the consumer interaction with snap7
 */

class RAMS7200LibFacade
{
public:
    /**
     * @brief RAMS7200LibFacade constructor
     * @param RAMS7200MS & : const reference to the MS object
     * @param queueToDPCallback : a callback that will be called after each poll
     * */
    RAMS7200LibFacade(RAMS7200MS& , queueToDPCallback);
    

    RAMS7200LibFacade(const RAMS7200LibFacade&) = delete;
    RAMS7200LibFacade& operator=(const RAMS7200LibFacade&) = delete;
    RAMS7200LibFacade(RAMS7200LibFacade&&) = delete;
    RAMS7200LibFacade& operator=(RAMS7200LibFacade&&) = delete;
    ~RAMS7200LibFacade() = default;

    void Poll();
    void WriteToPLC();
    void EnsureConnection();

    void Connect();

    template <typename T>
    void sleep_for(T duration)
    {
        std::unique_lock<std::mutex> lk(ms._threadMutex);
        ms._threadCv.wait_for(lk, duration, [&](){
           return !ms._run.load();     
        });
    }

private:
    void Reconnect();
    void Disconnect();
    void RAMS7200MarkDeviceConnectionError(bool);
    void RAMS7200ReadWriteMaxN(std::vector<DPInfo> dpItems, std::vector<TS7DataItem> items, const uint N, const uint PDU_SZ, const uint VAR_OH, const uint MSG_OH, const Common::S7Utils::Operation rorw);
    void doSmoothing(std::vector<DPInfo>&& dpItems, std::vector<TS7DataItem>&& items);
    void queueAll(std::vector<DPInfo>&& dpItems, std::vector<TS7DataItem>&& items);

    uint32_t ioFailures{0};
    RAMS7200MS& ms;

    // S7 related
    queueToDPCallback _queueToDPCB;
    bool _wasConnected{false};
    std::unique_ptr<TS7Client> _client{nullptr};
};

#endif //RAMS7200LIBFACADE_HXX

