/*
*
* OpenGL hardware capability viewer and database
*
* OpenGL internal format info
*
* Copyright (C) 2011-2015 by Sascha Willems (www.saschawillems.de)
*
* This code is free software, you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License version 3 as published by the Free Software Foundation.
*
* Please review the following information to ensure the GNU Lesser
* General Public License version 3 requirements will be met:
* http://opensource.org/licenses/lgpl-3.0.html
*
* The code is distributed WITHOUT ANY WARRANTY; without even the
* implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE.  See the GNU LGPL 3.0 for more details.
*
*/

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