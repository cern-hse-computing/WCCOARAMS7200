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
#include <RAMS7200HWMapper.hxx>
#include <RAMS7200HWService.hxx>

#include <HWObject.hxx>

//------------------------------------------------------------------------------------

void RAMS7200Drv::install_HWMapper()
{
  hwMapper = new RAMS7200HWMapper;
}

//--------------------------------------------------------------------------------

void RAMS7200Drv::install_HWService()
{
  hwService = new RAMS7200HWService;
}

//--------------------------------------------------------------------------------

HWObject * RAMS7200Drv::getHWObject() const
{
  return new HWObject();
}

//--------------------------------------------------------------------------------

void RAMS7200Drv::install_AlertService()
{
  DrvManager::install_AlertService();
}
