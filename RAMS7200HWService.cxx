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
 * Co-author: Richi Dubey (HSE)
 *
 **/

#include <RAMS7200HWService.hxx>
#include "RAMS7200Resources.hxx"

#include <DrvManager.hxx>
#include <PVSSMacros.hxx>     // DEBUG macros
#include "Common/Logger.hxx"
#include "Common/Constants.hxx"
#include "Common/Utils.hxx"

#include "RAMS7200HWMapper.hxx"
#include "RAMS7200LibFacade.hxx"

#include <signal.h>
#include <execinfo.h>
#include <exception>
#include <chrono>
#include <utility>
#include <thread>

static std::atomic<bool> _driverRun{true};

//--------------------------------------------------------------------------------
// called after connect to data

RAMS7200HWService::RAMS7200HWService()
{
  signal(SIGSEGV, handleSegfault);
  signal(SIGABRT, handleSegfault);
}

PVSSboolean RAMS7200HWService::initialize(int argc, char *argv[])
{
  // use this function to initialize internals
  // if you don't need it, you can safely remove the whole method
  Common::Logger::globalInfo(Common::Logger::L1,__PRETTY_FUNCTION__,"RAMS7200 Driver initialization of Internal vars start");

  // add callback for new MS
  static_cast<RAMS7200HWMapper*>(DrvManager::getHWMapperPtr())->setNewMSCallback(_newMSCB);

  Common::Logger::globalInfo(Common::Logger::L1,__PRETTY_FUNCTION__,"RAMS7200 Driver initialization of Internal vars end");
  // To stop driver return PVSS_FALSE
  return PVSS_TRUE;
}


void RAMS7200HWService::queueToDP(std::vector<toDPTriple>&& payload)
{
  std::lock_guard lock{_toDPmutex};
  for(auto& item : payload)
  {
    _toDPqueue.emplace(std::move(item));
  }
}

void RAMS7200HWService::handleNewMS(RAMS7200MS& ms)
{
  std::lock_guard lock{_toDPmutex};
  ms._run = true;

  // PLC thread
  _plcThreads.emplace_back(std::thread([&]() {
    Common::Logger::globalInfo(Common::Logger::L1,__PRETTY_FUNCTION__, "Thread up for PLC IP" + CharString(ms._ip.c_str()));
    
    RAMS7200LibFacade aFacade(ms, this->_queueToDPCB);
    aFacade.Connect();
    bool wasActive = !RAMS7200Resources::getDisableCommands();
    const auto cycleInterval = std::chrono::seconds(Common::Constants::getCycleInterval());
    while(_driverRun && ms._run)
    {
      const bool isNowActive = !RAMS7200Resources::getDisableCommands();
      aFacade.EnsureConnection(wasActive != isNowActive);
      if(isNowActive) {
        // The Server is Active (for redundant systems)
        Common::Logger::globalInfo(Common::Logger::L2,__PRETTY_FUNCTION__, "Polling:", ms._ip.c_str());
        const auto start = std::chrono::steady_clock::now();
        //First do all the writes for this IP, then the reads
        aFacade.WriteToPLC();
        aFacade.Poll();                         
        const auto end = std::chrono::steady_clock::now();
        const auto time_elapsed = end - start;
        
        // If we still have time left, then sleep
        if(time_elapsed < cycleInterval)
          aFacade.sleep_for(cycleInterval- time_elapsed);
      } else {
        // The Server is Passive (for redundant systems)
        aFacade.sleep_for( std::chrono::seconds(1));
      }
      wasActive = isNowActive;
    }
  }));

}

//--------------------------------------------------------------------------------
// called after connect to event

PVSSboolean RAMS7200HWService::start()
{
  // use this function to start your hardware activity.  
  // The ms list is automatically built by exisiting addresses sent at driver startup
  for (auto& msIt : static_cast<RAMS7200HWMapper*>(DrvManager::getHWMapperPtr())->getRAMS7200MSs() )
  {
      this->handleNewMS(msIt.second);
  }

  //Write Driver version
  char* DrvVersion = new char[Common::Constants::getDrvVersion().size()+1];
  std::strcpy(DrvVersion, Common::Constants::getDrvVersion().c_str());
  Common::Logger::globalInfo(Common::Logger::L1, __PRETTY_FUNCTION__, "RAMS7200 Sent Driver version: " + CharString(DrvVersion));
  queueToDP({std::make_tuple("_VERSION", Common::Constants::getDrvVersion().size() + 1, DrvVersion)});

  return PVSS_TRUE;
}

//--------------------------------------------------------------------------------

void RAMS7200HWService::stop()
{
  // use this function to stop your hardware activity.
  Common::Logger::globalInfo(Common::Logger::L1,__PRETTY_FUNCTION__,"RAMS7200 Driver requested to Stop");
  _driverRun.store(false);

  for (auto& msIt : static_cast<RAMS7200HWMapper*>(DrvManager::getHWMapperPtr())->getRAMS7200MSs() )
  {
      msIt.second._run.store(false);
      msIt.second._threadCv.notify_all();
  }

  for(auto& pt : _plcThreads)
  {
    if(pt.joinable())
        pt.join();
  }

}

//--------------------------------------------------------------------------------

void RAMS7200HWService::workProc()
{

  HWObject obj;
  const TimeVar work_time{};

  std::lock_guard lock{_toDPmutex};

  while (!_toDPqueue.empty())
  {
    auto item = std::move(_toDPqueue.front());
    _toDPqueue.pop();

    obj.setAddress(std::get<0>(item));
    
    // find the HWObject via the periphery address in the HWObject list,
    HWObject *addrObj = DrvManager::getHWMapperPtr()->findHWObject(&obj);

    // ok, we found it; now send to the DPEs
    if ( addrObj )
    {
        //addrObj->debugPrint();
        obj.setOrgTime(work_time);  // current time
        obj.setDlen(std::get<1>(item)); //length
        obj.setData((PVSSchar*)(std::move(std::get<2>(item)))); //data
        obj.setObjSrcType(srcPolled);

        if( DrvManager::getSelfPtr()->toDp(&obj, addrObj) != PVSS_TRUE) {
          Common::Logger::globalWarning(__PRETTY_FUNCTION__, "Problem in sending item's value to PVSS for address: " + std::get<0>(item));
        }
    } else {
        Common::Logger::globalWarning(__PRETTY_FUNCTION__, "Problem in getting HWObject for the address: " + std::get<0>(item));   
    }
  }
}


//--------------------------------------------------------------------------------
// we get data from PVSS and shall send it to the periphery

PVSSboolean RAMS7200HWService::writeData(HWObject *objPtr)
{
  Common::Logger::globalInfo(Common::Logger::L2,__PRETTY_FUNCTION__,"Incoming obj address",objPtr->getAddress());

  std::vector<std::string> addressOptions = Common::Utils::split(objPtr->getAddress().c_str());

  // CONFIG DPs have just 1
  if(addressOptions.size() == 1)
  {
      try
      {
        Common::Logger::globalInfo(Common::Logger::L1,__PRETTY_FUNCTION__, "Incoming CONFIG address" + CharString(objPtr->getAddress()) + " : " +CharString(objPtr->getInfo()) );
        Common::Constants::GetParseMap().at(std::string(objPtr->getAddress().c_str()))((const char*)objPtr->getData());
      }
      catch (std::exception& e)
      {
          Common::Logger::globalWarning(__PRETTY_FUNCTION__, "No configuration handling for address:", CharString(objPtr->getAddress().c_str()) + ':' + CharString(e.what()));
      }
  }
  else if (addressOptions.size() == ADDRESS_OPTIONS_SIZE) // Send to PLC
  {

    if(!addressOptions[ADDRESS_OPTIONS_IP].length())
    {
        Common::Logger::globalWarning(__PRETTY_FUNCTION__,"Empty IP");
        return PVSS_FALSE;
    }

    if(!Common::S7Utils::AddressIsValid(addressOptions[ADDRESS_OPTIONS_VAR])){
      Common::Logger::globalWarning("Not a valid Var for address", objPtr->getAddress().c_str());
      return PVSS_FALSE; 
    }

    auto& mss = static_cast<RAMS7200HWMapper*>(DrvManager::getHWMapperPtr())->getRAMS7200MSs();
    auto msIt = mss.find(addressOptions[ADDRESS_OPTIONS_IP]);
    if(msIt == mss.end()){
      Common::Logger::globalWarning(__PRETTY_FUNCTION__,"Connection not found for IP: ", addressOptions[ADDRESS_OPTIONS_IP].c_str());
      return PVSS_FALSE;
    }

    const auto length = static_cast<int>(objPtr->getDlen());
    auto correctval = new char[length];
    std::memcpy(correctval, objPtr->getDataPtr(), length);

    if(length == 2) {
      int16_t inInt16 = Common::Utils::CopyNSwapBytes<int16_t>(correctval);
      Common::Logger::globalInfo(Common::Logger::L2, "Received request to write integer, Correct val is: ", std::to_string(inInt16).c_str());
    } else if(length == 4){
      float inFloat = Common::Utils::CopyNSwapBytes<float>(correctval);
      Common::Logger::globalInfo(Common::Logger::L2, "Received request to write float, Correct val is:  ", std::to_string(inFloat).c_str());
    } else {
      Common::Logger::globalInfo(Common::Logger::L2, "Received request to write non integer/float");
    }

    msIt->second.queuePLCItem(addressOptions[ADDRESS_OPTIONS_VAR], correctval);
    Common::Logger::globalInfo(Common::Logger::L1,__PRETTY_FUNCTION__, "Added write request to queue for Address: " + CharString(objPtr->getAddress()) + " : "+ CharString(objPtr->getInfo()) );
  }
  else
  {
      Common::Logger::globalWarning(__PRETTY_FUNCTION__," Invalid address options size for address: ", objPtr->getAddress().c_str());
  }

  return PVSS_TRUE;
}

//--------------------------------------------------------------------------------
void handleSegfault(int signal_code){
    void *array[50];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 50);

    // print out all the frames to stderr
    fprintf( stderr, "Error: signal %d:\n", signal_code);
    Common::Logger::globalWarning("RAMS7200HWService suffered a segmentation fault, code " + CharString(signal_code));
    backtrace_symbols_fd(array, size, STDERR_FILENO);

    // restore and trigger default handle (to get the core dump)
    signal(signal_code, SIG_DFL);
}

