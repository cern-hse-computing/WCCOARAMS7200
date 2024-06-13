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


#ifndef RAMS7200HWSERVICE_H_
#define RAMS7200HWSERVICE_H_

#include <HWService.hxx>
#include "RAMS7200MS.hxx"
#include "RAMS7200LibFacade.hxx"
#include "Common/Logger.hxx"

#include <memory>
#include <queue>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <tuple>

class RAMS7200HWService : public HWService
{
  public:
    RAMS7200HWService();
    virtual PVSSboolean initialize(int argc, char *argv[]);
    virtual PVSSboolean start();
    virtual void stop();
    virtual void workProc();
    virtual PVSSboolean writeData(HWObject *objPtr);
    int CheckIP(std::string);

  private:
    void queueToDP(std::vector<toDPTriple>&&);
    void handleNewMS(RAMS7200MS&);

    queueToDPCallback  _queueToDPCB{[this](std::vector<toDPTriple>&& payload){this->queueToDP(std::move(payload));}};
    std::function<void(RAMS7200MS&)> _newMSCB{[this](RAMS7200MS& ms){this->handleNewMS(ms);}};

    //Common
    std::mutex _toDPmutex;
    std::queue<toDPTriple> _toDPqueue;

    enum
    {
       ADDRESS_OPTIONS_IP = 0,
       ADDRESS_OPTIONS_VAR,
       ADDRESS_OPTIONS_POLLTIME,
       ADDRESS_OPTIONS_SIZE
    } ADDRESS_OPTIONS;

    std::vector<std::thread> _plcThreads;
};


void handleSegfault(int signal_code);

#endif
