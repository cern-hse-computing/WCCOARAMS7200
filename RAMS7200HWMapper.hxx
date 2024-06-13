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


#ifndef RAMS7200HWMAPPER_H_
#define RAMS7200HWMAPPER_H_

#include <HWMapper.hxx>
#include <unordered_map>

#include "RAMS7200MS.hxx"

#define RAMS7200DrvBoolTransType (TransUserType)
#define RAMS7200DrvUint8TransType (TransUserType + 1)
#define RAMS7200DrvInt16TransType (TransUserType + 2)
#define RAMS7200DrvInt32TransType (TransUserType + 3)
#define RAMS7200DrvFloatTransType (TransUserType + 4)
#define RAMS7200DrvStringTransType (TransUserType + 5)

using newMSCB = std::function<void(RAMS7200MS&)>;

class RAMS7200HWMapper : public HWMapper
{
  public:
    virtual PVSSboolean addDpPa(DpIdentifier &dpId, PeriphAddr *confPtr);
    virtual PVSSboolean clrDpPa(DpIdentifier &dpId, PeriphAddr *confPtr);

    std::unordered_map<std::string, RAMS7200MS>& getRAMS7200MSs(){return RAMS7200MSs;}
    void setNewMSCallback(newMSCB cb){_newMSCB = cb;}

  private:
    void addAddress(const std::string &ip, const std::string &var, const std::string &pollTime);
    void removeAddress(const std::string& ip, const std::string& var, const std::string &pollTime);
    std::unordered_map<std::string, RAMS7200MS> RAMS7200MSs;
    newMSCB _newMSCB{nullptr};

    enum Direction
    {
        DIRECTION_OUT = 1,
        DIRECTION_IN = 2,
        DIRECTION_INOUT = 6,
    };
};

#endif
