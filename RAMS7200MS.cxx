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

#include "RAMS7200MS.hxx"
#include "Common/S7Utils.hxx"
#include "Common/Logger.hxx"
#include <algorithm>


RAMS7200MSVar::RAMS7200MSVar(std::string varName, int pollTime, TS7DataItem type) : varName(varName), pollTime(pollTime), _toPlc(type), _toDP(type){
    _toPlc.pdata = nullptr;
    _toDP.pdata = nullptr;
}

void RAMS7200MS::addVar(std::string varName, int pollTime)
{
    std::lock_guard lock{_rwmutex};
    auto var = RAMS7200MSVar(varName, pollTime, Common::S7Utils::TS7DataItemFromAddress(varName, false));
    vars.emplace(varName, std::move(var));
}

void RAMS7200MS::removeVar(std::string varName)
{
    std::lock_guard lock{_rwmutex};
    auto it = vars.find(varName);
    if(it != vars.end()) {
      Common::S7Utils::TS7DeallocateDataItem(it->second._toDP);
      Common::S7Utils::TS7DeallocateDataItem(it->second._toPlc);
      vars.erase(it);
    }
}

void RAMS7200MS::queuePLCItem(const std::string& varName, void* item)
{
  std::lock_guard lock{_rwmutex};

  if (vars.count(varName) == 0) {
    Common::Logger::globalWarning(__PRETTY_FUNCTION__, "Undefined variable:", varName.c_str());
    delete[] static_cast<char*>(item);
    return;
  }
  auto old_data = std::exchange(vars.at(varName)._toPlc.pdata, item);
  if (old_data != nullptr) {
    delete[] static_cast<char*>(old_data);
    Common::Logger::globalInfo(Common::Logger::L1, "Overwriting old data for:", CharString(_ip.c_str()) + varName.c_str());
  }
}
