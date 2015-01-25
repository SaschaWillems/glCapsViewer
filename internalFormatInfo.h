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

#include <GL/glew.h>
#include <string>
#include <vector>

#pragma once
namespace capsViewer {

	using namespace std;

	enum internalFormatInfoType { infoTypeValue = 0, infoTypeSupport = 1 };

	class internalFormatInfoValue
	{
	public:
		internalFormatInfoType infoType;
		GLenum infoEnum;
		GLint infoValue;
		string infoString;
	};

	class internalFormatInfo
	{
	public:
		bool supported = false;
		GLenum textureFormat;
		vector<internalFormatInfoValue> formatInfoValues;
		internalFormatInfo(GLenum format);
		void addValueInfo(internalFormatInfoType type, GLenum infoenum, string infostring);
	};

}
