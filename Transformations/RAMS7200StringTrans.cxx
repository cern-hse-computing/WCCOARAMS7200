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

// Our transformation class PVSS <--> Hardware
#include "RAMS7200StringTrans.hxx"
#include <ErrHdl.hxx>     // The Error handler Basics/Utilities
#include <TextVar.hxx>    // The TextVar class


#include "RAMS7200HWMapper.hxx"

#include "Common/Logger.hxx"

//----------------------------------------------------------------------------
namespace Transformations {


//RAMS7200StringTrans::RAMS7200StringTrans() : Transformation() { }

TransformationType RAMS7200StringTrans::isA() const
{
  return (TransformationType) RAMS7200DrvStringTransType;
}

TransformationType RAMS7200StringTrans::isA(TransformationType type) const {
    if (type == isA())
        return type;
    else
        return Transformation::isA(type);
}



//----------------------------------------------------------------------------

Transformation *RAMS7200StringTrans::clone() const
{
  return new RAMS7200StringTrans;
}

//----------------------------------------------------------------------------
// Our item size. The max we will use is 256 Bytes.
// This is an arbitrary value! A Transformation for a long e.g. would return 4

int RAMS7200StringTrans::itemSize() const
{
  return _size;
}

//----------------------------------------------------------------------------
// Our preferred Variable type. Data will be converted to this type
// before toPeriph is called.

VariableType RAMS7200StringTrans::getVariableType() const
{
  return TEXT_VAR;
}

PVSSboolean RAMS7200StringTrans::toPeriph(PVSSchar *buffer, PVSSuint len,
                                      const Variable &var, const PVSSuint subix) const
{

  // Be paranoic, check variable type
  if ( var.isA() != TEXT_VAR )
  {
    // Throw error message
    ErrHdl::error(
      ErrClass::PRIO_SEVERE,             // Data will be lost
      ErrClass::ERR_PARAM,               // Wrong parametrization
      ErrClass::UNEXPECTEDSTATE,         // Nothing else appropriate
      "RAMS7200StringTrans", "toPeriph",     // File and function name
      "Wrong variable type for data"     // Unfortunately we don't know which DP
    );

    return PVSS_FALSE;
  }

    const TextVar tv = reinterpret_cast<const TextVar &>(var);
    if(tv.getString().len() >= _size){
      ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
          ErrClass::ERR_PARAM, // Wrong parametrization
          ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
          "RAMS7200StringTrans", "toPeriph", // File and function name
          "String too long" // Unfortunately we don't know which DP
          );

      return PVSS_FALSE;
    }
  memset(buffer, 0, len);
  snprintf( (char *) buffer, len, "%s", tv.getValue());
  return PVSS_TRUE;
}

VariablePtr RAMS7200StringTrans::toVar(const PVSSchar *buffer, const PVSSuint dlen,
                                   const PVSSuint /* subix */) const
{
  return new TextVar((const char*)buffer, (PVSSuint)strnlen((const char*)buffer, dlen));
}


}//namespace
