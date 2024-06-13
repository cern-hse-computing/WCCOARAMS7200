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

#include <RAMS7200Drv.hxx>
#include <RAMS7200Resources.hxx>
#include "Common/Logger.hxx"
#include "Common/Constants.hxx"

#include <signal.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  // an instance of our own Resources class is needed
  RAMS7200Resources& resources = RAMS7200Resources::GetInstance();

  resources.init(argc, argv);

  RAMS7200Resources::setDriverType(Common::Constants::getDrvName().c_str());

  // the user wants to know std. commandline options
  if ( resources.getHelpFlag() )
  {
    resources.printHelp();
    return 0;
  }

  // the user wants to know std. debug flags
  if ( resources.getHelpDbgFlag() )
  {
    resources.printHelpDbg();
    return 0;
  }

  // the user wants to know std. report flags
  if ( resources.getHelpReportFlag() )
  {
    resources.printHelpReport();
    return 0;
  }

  RAMS7200Drv *driver = nullptr;

  try{
      // handle std. signals
      signal(SIGINT, Manager::sigHdl);
      signal(SIGTERM, Manager::sigHdl);

      // a pointer is needed, since the Manager dtor does a delete
      RAMS7200Drv *driver = new RAMS7200Drv;

      driver->mainProcedure(argc, argv);

      return 0;
  }
  catch(std::exception& e){
      Common::Logger::globalWarning("Exception");
      if(driver)
          driver->exit(0);
      Common::Logger::globalError(e.what());
  }
  return 0;

}
