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

#include "RAMS7200BoolTrans.hxx"

#include "RAMS7200HWMapper.hxx"

#include "Common/Logger.hxx"

#include <cmath>

#include "BitVar.hxx"

namespace Transformations{

TransformationType RAMS7200BoolTrans::isA() const {
    return (TransformationType) RAMS7200DrvBoolTransType;
}

TransformationType RAMS7200BoolTrans::isA(TransformationType type) const {
	if (type == isA())
		return type;
	else
		return Transformation::isA(type);
}

Transformation *RAMS7200BoolTrans::clone() const {
    return new RAMS7200BoolTrans;
}

int RAMS7200BoolTrans::itemSize() const {
	return size;
}

VariableType RAMS7200BoolTrans::getVariableType() const {
	return BIT_VAR;
}


PVSSboolean RAMS7200BoolTrans::toPeriph(PVSSchar *buffer, PVSSuint len,	const Variable &var, const PVSSuint subix) const {

	if(var.isA() != BIT_VAR){
		ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
				ErrClass::ERR_PARAM, // Wrong parametrization
				ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
				"REMUSDRVBoolTrans", "toPeriph", // File and function name
				"Wrong variable type "  + CharString(subix)// Unfortunately we don't know which DP
				);
		return PVSS_FALSE;
	}

	*reinterpret_cast<bool *>(buffer) = (reinterpret_cast<const BitVar&>(var)).getValue();

	return PVSS_TRUE;
}

VariablePtr RAMS7200BoolTrans::toVar(const PVSSchar *buffer, const PVSSuint dlen, const PVSSuint subix) const {
	if(dlen && buffer != NULL)
		return new BitVar(*reinterpret_cast<const bool*>(buffer));

	ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
			ErrClass::ERR_PARAM, // Wrong parametrization
			ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
			"RAMS7200BoolTrans", "toVar", // File and function name
			"Wrong buffer" // Unfortunately we don't know which DP
			);
	return NULL;
}

}//namespace
