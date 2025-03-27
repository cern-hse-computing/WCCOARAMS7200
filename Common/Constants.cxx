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

#include "Constants.hxx"
#include "Logger.hxx"
#include "Utils.hxx"
#include "config.h"
#include <cstring>

namespace Common {

    std::string Constants::drv_name = "RAMS7200";
    uint32_t Constants::DRV_NO = 0;                         // Read from PVSS on driver startup
    uint32_t Constants::TSAP_PORT_LOCAL = 0;                // Read from PVSS on driver startup from config file
    uint32_t Constants::TSAP_PORT_REMOTE = 0;               // Read from PVSS on driver startupconfig file
    uint32_t Constants::POLLING_INTERVAL = 2;               // Read from PVSS on driver startupconfig file, default 2 seconds
    uint32_t Constants::MAX_IO_FAILURES = 1;                // Read from PVSS on driver startupconfig file, default 1 time
    uint32_t Constants::CYCLE_INTERVAL = 1;                 // Read from PVSS on driver startupconfig file, default 1 second
    bool Constants::SMOOTHING = true;                       // Read from PVSS on driver startupconfig file
    std::string Constants::drv_version = PROJECT_VER;

    // The map can be used to map a callback to a HwObject address
    std::map<std::string, std::function<void(const char*)>> Constants::parse_map =
    {
        {   "_DEBUGLVL",
            [](const char* data)
            {
                uint16_t retVal = Common::Utils::CopyNSwapBytes<uint16_t>(data);
                Common::Logger::globalInfo(Common::Logger::L1,__PRETTY_FUNCTION__, "setLogLvl :",std::to_string(retVal).c_str());
                Common::Logger::setLogLvl(retVal);
            }
        }
    };
}
