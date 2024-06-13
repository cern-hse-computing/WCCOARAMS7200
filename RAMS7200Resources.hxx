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

#ifndef RAMS7200RESOURCES_H_
#define RAMS7200RESOURCES_H_

// Our Resources class
// This class has two tasks:
//  - Interpret commandline and read config file
//  - Be an interface to internal datapoints

#include <DrvRsrce.hxx>

class RAMS7200Resources : public DrvRsrce
{
  public:

    static void init(int &argc, char *argv[]); // Initialize statics
    static PVSSboolean readSection();          // read config file
    static RAMS7200Resources& GetInstance();

    // Get the number of names we need the DpId for
    virtual int getNumberOfDpNames();

    // TODO in this template we do not use internal DPs in the driver
    // If you need DPs, then also some other methods must be implemented
    RAMS7200Resources(RAMS7200Resources const&)   = delete;
    void operator= (RAMS7200Resources const&)  = delete;
  private:
    RAMS7200Resources(){}

    static const CharString SECTION_NAME;
    static const CharString TSAP_PORT_LOCAL;
    static const CharString TSAP_PORT_REMOTE;
    static const CharString POLLING_INTERVAL;
    static const CharString SMOOTHING;
    static const CharString MAX_IO_FAILURES;
    static const CharString CYCLE_INTERVAL;
};

#endif
