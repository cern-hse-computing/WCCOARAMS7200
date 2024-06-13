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

#ifndef CONSTANTS_HXX_
#define CONSTANTS_HXX_

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory>
#include <map>
#include <Common/Utils.hxx>
#include <Common/Logger.hxx>

namespace Common{

    /*!
    * \class Constants
    * \brief Class containing constant values used in driver
    */
    class Constants{
    public:

        static void setDrvName(std::string lp);
        static std::string& getDrvName();

        static std::string& getDrvVersion();

        // called in driver init to set the driver number dynamically
        static void setDrvNo(uint32_t no);
        // subsequentally called when writing buffers etc.
        static uint32_t getDrvNo();

        static void setLocalTsapPort(uint32_t port);
        static const uint32_t& getLocalTsapPort();

        static void setRemoteTsapPort(uint32_t port);
        static const uint32_t& getRemoteTsapPort();

        static void setPollingInterval(uint32_t pollingInterval);
        static const uint32_t& getPollingInterval();
        
        static const std::map<std::string,std::function<void(const char *)>>& GetParseMap();

        static uint32_t getMsCopyPort();

        static void setSmoothing(bool smoothing);
        static bool getSmoothing();

        static uint32_t getMaxIoFailures();
        static void setMaxIoFailures(uint32_t maxIoFailures);

        static uint32_t getCycleInterval();
        static void setCycleInterval(uint32_t cycleInterval);

    private:
        static std::string drv_name;
        static std::string drv_version;
        static uint32_t DRV_NO;   // WinCC OA manager number
        static uint32_t TSAP_PORT_LOCAL;
        static uint32_t TSAP_PORT_REMOTE;
        static uint32_t POLLING_INTERVAL;
        static bool SMOOTHING;
        static uint32_t MAX_IO_FAILURES;
        static uint32_t CYCLE_INTERVAL;

        static std::map<std::string, std::function<void(const char *)>> parse_map;
    };

    inline const std::map<std::string,std::function<void(const char *)>>& Constants::GetParseMap()
    {
        return parse_map;
    }

    inline void Constants::setDrvName(std::string dname){
        drv_name = dname;
    }

    inline std::string& Constants::getDrvName(){
        return drv_name;
    }

    inline std::string& Constants::getDrvVersion(){
        return drv_version;
    }

    inline void Constants::setDrvNo(uint32_t no){
        DRV_NO = no;
    }

    inline uint32_t Constants::getDrvNo(){
        return DRV_NO;
    }

    inline void Constants::setLocalTsapPort(uint32_t port){
        Common::Logger::globalInfo(Common::Logger::L1,__PRETTY_FUNCTION__,"Setting TSAP_PORT_LOCAL=" + CharString(port));
        TSAP_PORT_LOCAL = port;
    }

    inline const uint32_t& Constants::getLocalTsapPort(){
        return TSAP_PORT_LOCAL;
    }

    inline void Constants::setRemoteTsapPort(uint32_t port){
        Common::Logger::globalInfo(Common::Logger::L1,__PRETTY_FUNCTION__,"Setting TSAP_PORT_REMOTE=" + CharString(port));
        TSAP_PORT_REMOTE = port;
    }

    inline const uint32_t& Constants::getRemoteTsapPort(){
        return TSAP_PORT_REMOTE;
    }

    inline void Constants::setPollingInterval(uint32_t pollingInterval)
    {
        Common::Logger::globalInfo(Common::Logger::L1,__PRETTY_FUNCTION__,"Setting POLLING_INTERVAL=" + CharString(pollingInterval));
        POLLING_INTERVAL = pollingInterval;
    }

    inline const uint32_t& Constants::getPollingInterval()
    {
        return POLLING_INTERVAL;
    }

    inline void Constants::setSmoothing(bool smoothing) {
        SMOOTHING = smoothing;
    }

    inline bool Constants::getSmoothing() {
        return SMOOTHING;
    }

    inline uint32_t Constants::getMaxIoFailures() {
        return MAX_IO_FAILURES;
    }

    inline void Constants::setMaxIoFailures(uint32_t maxIoFailures) {
        MAX_IO_FAILURES = maxIoFailures;
    }

    inline uint32_t Constants::getCycleInterval() {
        return CYCLE_INTERVAL;
    }

    inline void Constants::setCycleInterval(uint32_t cycleInterval) {
        CYCLE_INTERVAL = cycleInterval;
    }

}//namespace
#endif /* CONSTANTS_HXX_ */
