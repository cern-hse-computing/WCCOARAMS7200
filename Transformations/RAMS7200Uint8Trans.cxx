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

#include "RAMS7200Uint8Trans.hxx"

#include "RAMS7200HWMapper.hxx"

#include "Common/Logger.hxx"

#include <cmath>

#include <IntegerVar.hxx>

namespace Transformations{

TransformationType RAMS7200Uint8Trans::isA() const {
    return (TransformationType) RAMS7200DrvUint8TransType;
}

TransformationType RAMS7200Uint8Trans::isA(TransformationType type) const {
	if (type == isA())
		return type;
	else
		return Transformation::isA(type);
}

Transformation *RAMS7200Uint8Trans::clone() const {
    return new RAMS7200Uint8Trans;
}

int RAMS7200Uint8Trans::itemSize() const {
	return size;
}

VariableType RAMS7200Uint8Trans::getVariableType() const {
	return INTEGER_VAR;
}

PVSSboolean RAMS7200Uint8Trans::toPeriph(PVSSchar *buffer, PVSSuint len,	const Variable &var, const PVSSuint subix) const {

	if(var.isA() != INTEGER_VAR){
		ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
				ErrClass::ERR_PARAM, // Wrong parametrization
				ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
				"RAMS7200Uint8Trans", "toPeriph", // File and function name
				"Wrong variable type" // Unfortunately we don't know which DP
				);

		return PVSS_FALSE;
	}
	// this one is a bit special as the number is handled by wincc oa as int32, but we handle it as 8 bit unsigned integer
	// thus any info above the 8 first bits is lost
	*(reinterpret_cast<uint8_t *>(buffer + (subix * size))) = (uint8_t)(reinterpret_cast<const IntegerVar &>(var)).getValue();
	return PVSS_TRUE;
}

VariablePtr RAMS7200Uint8Trans::toVar(const PVSSchar *buffer, const PVSSuint dlen, const PVSSuint subix) const {

	if(buffer == NULL || dlen%size > 0 || dlen < size*(subix+1)){
		ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
				ErrClass::ERR_PARAM, // Wrong parametrization
				ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
				"RAMS7200Uint8Trans", "toVar", // File and function name
				"Null buffer pointer or wrong length: " + CharString(dlen) // Unfortunately we don't know which DP
				);
		return NULL;
	}
	// this one is a bit special as the number is handled by wincc oa as int32, but we handle it as 8 bit unsigned integer
	return new IntegerVar((int32_t)*reinterpret_cast<const uint8_t*>(buffer + (subix * size)));
}

}//namespace
