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

#include "RAMS7200HWMapper.hxx"
#include "Transformations/RAMS7200StringTrans.hxx"
#include "Transformations/RAMS7200UInt16Trans.hxx"
#include "Transformations/RAMS7200UInt32Trans.hxx"
#include "Transformations/RAMS7200FloatTrans.hxx"
#include "Transformations/RAMS7200BoolTrans.hxx"
#include "Transformations/RAMS7200Uint8Trans.hxx"
#include "RAMS7200HWService.hxx"

#include <algorithm>
#include <utility>
#include "Common/Logger.hxx"
#include "Common/Constants.hxx"
#include "Common/Utils.hxx"
#include "Common/S7Utils.hxx"
#include <PVSSMacros.hxx>     // DEBUG macros


//--------------------------------------------------------------------------------
// We get new configs here. Create a new HW-Object on arrival and insert it.

PVSSboolean RAMS7200HWMapper::addDpPa(DpIdentifier &dpId, PeriphAddr *confPtr)
{
  // We don't use Subindices here, so its simple.
  // Otherwise we had to look if we already have a HWObject and adapt its length.

  Common::Logger::globalInfo(Common::Logger::L1,__PRETTY_FUNCTION__, "With Direction: " + CharString(confPtr->getDirection()) + ", addDpPa called for " + confPtr->getName().c_str());

  // tell the config how we will transform data to/from the device
  // by installing a Transformation object into the PeriphAddr
  // In this template, the Transformation type was set via the
  // configuration panel (it is already set in the PeriphAddr)

  switch (confPtr->getTransformationType()) {
    case TransUndefinedType:
      Common::Logger::globalInfo(Common::Logger::L1, __PRETTY_FUNCTION__, "Undefined transformation" + CharString(confPtr->getTransformationType()) +", For address: "+ confPtr->getName());
      return HWMapper::addDpPa(dpId, confPtr);
    case RAMS7200DrvBoolTransType:
      Common::Logger::globalInfo(Common::Logger::L3,"Bool transformation");
      confPtr->setTransform(new Transformations::RAMS7200BoolTrans);
      break;
    case RAMS7200DrvUint8TransType:
      Common::Logger::globalInfo(Common::Logger::L3,"Uint8 transformation");
      confPtr->setTransform(new Transformations::RAMS7200Uint8Trans);
      break;
    case RAMS7200DrvUInt32TransType:
      Common::Logger::globalInfo(Common::Logger::L3,"UInt32 transformation");
      confPtr->setTransform(new Transformations::RAMS7200UInt32Trans);
      break;
    case RAMS7200DrvUInt16TransType:
      Common::Logger::globalInfo(Common::Logger::L3,"UInt16 transformation");
      confPtr->setTransform(new Transformations::RAMS7200UInt16Trans);
      break;
    case RAMS7200DrvFloatTransType:
      Common::Logger::globalInfo(Common::Logger::L3,"Float transformation");
      confPtr->setTransform(new Transformations::RAMS7200FloatTrans);
      break;
    case RAMS7200DrvStringTransType:
      Common::Logger::globalInfo(Common::Logger::L3,"String transformation");
      confPtr->setTransform(new Transformations::RAMS7200StringTrans);
      break;
    default:
      Common::Logger::globalError("RAMS7200HWMapper::addDpPa", CharString("Illegal transformation type ") + CharString((int) confPtr->getTransformationType()));
      return HWMapper::addDpPa(dpId, confPtr);
  }


  // First add the config, then the HW-Object
  if ( !HWMapper::addDpPa(dpId, confPtr) )  // FAILED !! 
  {
    Common::Logger::globalInfo(Common::Logger::L1, __PRETTY_FUNCTION__, "Failed in adding DP Para to HW Mapper object for address : " + CharString(confPtr->getName()));
    return PVSS_FALSE;
  }

  std::vector<std::string> addressOptions = Common::Utils::split(confPtr->getName().c_str());

  HWObject *hwObj = new HWObject;
  // Set Address and Subindex
  Common::Logger::globalInfo(Common::Logger::L3, "New Object", "name:" + confPtr->getName());
  hwObj->setConnectionId(confPtr->getConnectionId());
  hwObj->setAddress(confPtr->getName());       // Resolve the HW-Address, too

  // Set the data type.
  hwObj->setType(confPtr->getTransform()->isA());

  // Set the len needed for data from _all_ subindices of this PVSS-Address.
  // Because we will deal with subix 0 only this is the Transformation::itemSize
  hwObj->setDlen(confPtr->getTransform()->itemSize());
  // Add it to the list
  addHWObject(hwObj);

  if( (confPtr->getDirection() == DIRECTION_IN || confPtr->getDirection() == DIRECTION_INOUT) && (addressOptions.size() == 3) ) {
    if(!Common::S7Utils::AddressIsValid(addressOptions[1])){
      Common::Logger::globalError(__PRETTY_FUNCTION__, "Address is not valid!", CharString(confPtr->getName()));
      return PVSS_FALSE;
    }
    // TODO: add warning if requested transformation is not the same as the s7 type
    addAddress(addressOptions[0], addressOptions[1], addressOptions[2]);
  }

  return PVSS_TRUE;
}

//--------------------------------------------------------------------------------

PVSSboolean RAMS7200HWMapper::clrDpPa(DpIdentifier &dpId, PeriphAddr *confPtr)
{
  Common::Logger::globalInfo(Common::Logger::L3, "clrDpPa called for" + confPtr->getName());

  std::vector<std::string> addressOptions = Common::Utils::split(confPtr->getName().c_str());

  // Find our HWObject via a template`  
  HWObject adrObj;
  adrObj.setAddress(confPtr->getName());

  // Lookup HW-Object via the Name, not via the HW-Address
  // The class type isn't important here
  HWObject *hwObj = findHWAddr(&adrObj);

  if(hwObj == NULL) {
    Common::Logger::globalWarning(__PRETTY_FUNCTION__, "Error in getting HW Address");
    return PVSS_FALSE;
  }

  if(confPtr->getDirection() == DIRECTION_IN || confPtr->getDirection() == DIRECTION_INOUT)
  {
      if (addressOptions.size() == 3) // IP + VAR + POLLTIME
      {
        removeAddress(addressOptions[0], addressOptions[1], addressOptions[2]);
      }
  }

  if ( hwObj ) {
    // Object exists, remove it from the list and delete it.
    clrHWObject(hwObj);
    delete hwObj;
  }

  // Call function of base class to remove config
  return HWMapper::clrDpPa(dpId, confPtr);
}

void RAMS7200HWMapper::addAddress(const std::string &ip, const std::string &var, const std::string &pollTime)
{
  auto msIt = RAMS7200MSs.find(ip);
  if(msIt == RAMS7200MSs.end())
  {
    msIt = RAMS7200MSs.emplace(std::piecewise_construct,
          std::forward_as_tuple(ip),
          std::forward_as_tuple(RAMS7200MS{ip})).first;
    Common::Logger::globalInfo(Common::Logger::L1,__PRETTY_FUNCTION__, "New RAMS7200MS device incoming, IP : " + CharString(msIt->second._ip.c_str()));
    if(_newMSCB){
      _newMSCB(msIt->second);
    }
  }
  if(!msIt->second._run.load() && _newMSCB){
      _newMSCB(msIt->second);
  }
  if(Common::S7Utils::AddressIsValid(var))
    msIt->second.addVar(var, std::stoi(pollTime));   
}


void RAMS7200HWMapper::removeAddress(const std::string &ip, const std::string &var, const std::string &pollTime)
{
  auto msIt = RAMS7200MSs.find(ip);
  if(msIt != RAMS7200MSs.end()) {
    {
        std::lock_guard lk(msIt->second._threadMutex);
        msIt->second._run.store(false);
    }
    msIt->second._threadCv.notify_all();
    msIt->second.removeVar(var);
    if(msIt->second.isEmpty()) {
      Common::Logger::globalInfo(Common::Logger::L1, __PRETTY_FUNCTION__,  "All Addresses deleted for IP : " + CharString(ip.c_str()));
      RAMS7200MSs.erase(msIt);
    }
  }
  
}
