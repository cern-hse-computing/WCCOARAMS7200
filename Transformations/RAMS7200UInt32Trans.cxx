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

#include <cstring>

#include "RAMS7200UInt32Trans.hxx"

#include "RAMS7200HWMapper.hxx"

#include "Common/Logger.hxx"
#include "Common/Utils.hxx"

#include <cmath>

#include <IntegerVar.hxx>

namespace Transformations {

TransformationType RAMS7200UInt32Trans::isA() const {
    return (TransformationType) RAMS7200DrvUInt32TransType;
}

TransformationType RAMS7200UInt32Trans::isA(TransformationType type) const {
	if (type == isA())
		return type;
	else
		return Transformation::isA(type);
}

Transformation *RAMS7200UInt32Trans::clone() const {
    return new RAMS7200UInt32Trans;
}

int RAMS7200UInt32Trans::itemSize() const {
	return size;
}

VariableType RAMS7200UInt32Trans::getVariableType() const {
	return INTEGER_VAR;
}

PVSSboolean RAMS7200UInt32Trans::toPeriph(PVSSchar *buffer, PVSSushort len, const Variable &var, const PVSSushort subix) const {

	if(var.isA() != INTEGER_VAR /* || subix >= Transformation::getNumberOfElements() */){
		ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
				ErrClass::ERR_PARAM, // Wrong parametrization
				ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
				"RAMS7200UInt32Trans", "toPeriph", // File and function name
				"Wrong variable type or wrong length: " + CharString(len) + ", subix: " + CharString(subix) // Unfortunately we don't know which DP
				);

		return PVSS_FALSE;
	}
	Common::Logger::globalInfo(Common::Logger::L2,"RAMS7200UInt32Trans::toPeriph : UInteger32 var received in transformation toPeriph, val is: ", std::to_string(((reinterpret_cast<const IntegerVar &>(var)).getValue())).c_str());
	reinterpret_cast<uint32_t *>(buffer)[subix] = Common::Utils::CopyNSwapBytes<uint32_t>(reinterpret_cast<const IntegerVar &>(var).getValue());
	return PVSS_TRUE;
}

VariablePtr RAMS7200UInt32Trans::toVar(const PVSSchar *buffer, const PVSSushort dlen, const PVSSushort subix) const {

	if(buffer == NULL || dlen%size > 0 || dlen < size*(subix+1)){
		ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
				ErrClass::ERR_PARAM, // Wrong parametrization
				ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
				"RAMS7200UInt32Trans", "toVar", // File and function name
				"Null buffer pointer or wrong length: " + CharString(dlen) // Unfortunately we don't know which DP
				);
		return NULL;
	}
	return new IntegerVar( Common::Utils::CopyNSwapBytes<uint32_t>(reinterpret_cast<const uint32_t*>(buffer)[subix]));
}

}//namespace
