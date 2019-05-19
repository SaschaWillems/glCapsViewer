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
#include <capsGroup.h>
#include <internalFormatTarget.h>

using namespace std;

class glCapsViewerCore 
{
private:
	map<GLint, string> enumList;
public:
    const string appVersion = "OpenGL Hardware Capability Viewer 1.2 - Copyright 2011-2019 by Sascha Willems (www.saschawillems.de)";
    const string appAbout = "glCapsViewer 1.2";
    const string fileVersion = "4.0";
	vector<string> availableContextTypes;
	map<string, string> implementation;
	vector<capsViewer::capsGroup> capgroups;
	vector<string> extensions;
	vector<string> osextensions;
	vector<GLint> compressedFormats;
	vector<capsViewer::internalFormatTarget> internalFormatTargets;
    vector<string> SPIRVExtensions;
	string description = "";
	string submitter = "";
	string comment = "";
	string contextType = "";
	string readOperatingSystem();
	bool extensionSupported(string ext);
	void clear();
	void readImplementation();
	void readCapabilities();
	void readExtensions();
	void readCompressedFormats();
	void readInternalFormats();
    void readSPIRVExtensions();
	void printExtensions();
	void readOsExtensions();
	bool loadEnumList();
	string getEnumName(GLint glenum);
	string reportToXml();
	void exportXml(string fileName);
};

