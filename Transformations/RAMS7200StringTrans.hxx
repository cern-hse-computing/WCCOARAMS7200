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

#ifndef RAMS7200StringTrans_H
#define RAMS7200StringTrans_H

#include <Transformation.hxx>

// Our Transformation class for Text
// As the Transformation really depends on the format of data you send and
// receive in your protocol (see HWService), this template is just an
// example.
// Things you have to change are marked with TODO


namespace Transformations {

class RAMS7200StringTrans : public Transformation
{
  public:
    // TODO probably your ctor looks completely different ...
    //RAMS7200StringTrans();
    virtual ~RAMS7200StringTrans() {}

    virtual TransformationType isA() const;

    virtual TransformationType isA(TransformationType type) const;

    // (max) size of one item. This is needed by DrvManager to
    // create the buffer used in toPeriph and by the Low-Level-Compare
    // For our Text-Transformation we set it arbitrarly to 256 Bytes
    virtual int itemSize() const;

    // The type of Variable we are expecting here
    virtual VariableType getVariableType() const;

    // Clone of our class
    virtual Transformation *clone() const;

    // Conversion from PVSS to Hardware
    virtual PVSSboolean toPeriph(PVSSchar *dataPtr, PVSSuint len, const Variable &var, const PVSSuint subix) const;

    // Conversion from Hardware to PVSS
    virtual VariablePtr toVar(const PVSSchar *data, const PVSSuint dlen, const PVSSuint subix) const;
private:
    const static size_t _size = 255;

};

}

#endif
