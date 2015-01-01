/*
*
* OpenGL hardware capability viewer and database
*
* OpenGL capability viewer core
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

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <map>
#include <glCapsViewerCapsGroup.h>

using namespace std;

class glCapsViewerCore 
{
private:
	map<GLint, string> enumList;
public:
	vector<string> availableContextTypes;
	map<string, string> implementation;
	vector<glCapsViewerCapsGroup> capgroups;
	vector<string> extensions;
	vector<string> osextensions;
	vector<GLint> compressedFormats;
	string description = "";
	string submitter = "";
	string contextType = "";
	string readOperatingSystem();
	bool extensionSupported(string ext);
	void clear();
	void readImplementation();
	void readCapabilities();
	void readExtensions();
	void readCompressedFormats();
	void printExtensions();
	void readOsExtensions();
	void loadEnumList();
	string getEnumName(GLint glenum);
	string reportToXml();
	void exportXml(string fileName);
};

