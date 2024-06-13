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

#include "RAMS7200FloatTrans.hxx"

#include "Transformations/RAMS7200Int16Trans.hxx"

#include "RAMS7200HWMapper.hxx"

#include "Common/Logger.hxx"
#include "Common/Utils.hxx"

#include <cmath>

#include <FloatVar.hxx>

namespace Transformations{

TransformationType RAMS7200FloatTrans::isA() const {
    return (TransformationType) RAMS7200DrvFloatTransType;
}

TransformationType RAMS7200FloatTrans::isA(TransformationType type) const {
	if (type == isA())
		return type;
	else
		return Transformation::isA(type);
}

Transformation *RAMS7200FloatTrans::clone() const {
	return new RAMS7200FloatTrans;
}

int RAMS7200FloatTrans::itemSize() const {
	return size;
}

VariableType RAMS7200FloatTrans::getVariableType() const {
	return FLOAT_VAR;
}

PVSSboolean RAMS7200FloatTrans::toPeriph(PVSSchar *buffer, PVSSuint len, const Variable &var, const PVSSuint subix) const {

	if(var.isA() != FLOAT_VAR){
		ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
				ErrClass::ERR_PARAM, // Wrong parametrization
				ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
				"RAMS7200FloatTrans", "toPeriph", // File and function name
				"Wrong variable type" // Unfortunately we don't know which DP
				);

		return PVSS_FALSE;
	}
	reinterpret_cast<float *>(buffer)[subix] = Common::Utils::CopyNSwapBytes<float>((reinterpret_cast<const FloatVar &>(var)).getValue());
	return PVSS_TRUE;
}

VariablePtr RAMS7200FloatTrans::toVar(const PVSSchar *buffer, const PVSSuint dlen, const PVSSuint subix) const {

	if(buffer == NULL || dlen%size > 0 || dlen < size*(subix+1)){
		ErrHdl::error(ErrClass::PRIO_SEVERE, // Data will be lost
				ErrClass::ERR_PARAM, // Wrong parametrization
				ErrClass::UNEXPECTEDSTATE, // Nothing else appropriate
				"RAMS7200FloatTrans", "toVar", // File and function name
				"Null buffer pointer or wrong length: " + CharString(dlen) // Unfortunately we don't know which DP
				);
		return NULL;
	}

	return new FloatVar(Common::Utils::CopyNSwapBytes(reinterpret_cast<const float*>(buffer)[subix]));
}

}//namespace
