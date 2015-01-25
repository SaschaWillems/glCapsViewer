#include "internalFormatInfo.h"

namespace capsViewer {

	internalFormatInfo::internalFormatInfo(GLenum format)
	{
		this->textureFormat = format;
	}

	void internalFormatInfo::addValueInfo(internalFormatInfoType type, GLenum infoenum, string infostring)
	{
		internalFormatInfoValue value;
		value.infoType = type;
		value.infoEnum = infoenum;
		value.infoString = infostring;
		formatInfoValues.push_back(value);
	}

}