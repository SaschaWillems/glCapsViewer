/*
*
* OpenGL hardware capability viewer and database
*
* OpenGL internal format description
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

#pragma once

#include <string>
#include <map>
#include <GL/glew.h>
#include <vector>
#include "internalFormatInfo.h"


#pragma once
namespace capsViewer {

	using namespace std;

	class internalFormatTarget
	{
	public:
		GLenum target;
		map<GLenum, string> internalFormats;
		vector<capsViewer::internalFormatInfo> textureFormats;
		internalFormatTarget(GLenum target, vector<GLint> compressedFormats);
		void getInternalFormatInfo(bool internalformatquery2);
	};

}

