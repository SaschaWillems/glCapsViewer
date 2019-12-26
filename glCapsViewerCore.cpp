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

#include <QXmlStreamReader>
#include <QFile>

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef _WIN32
	#include <Windows.h>
#endif 
#ifdef __linux__
	#include <GL/glx.h>
	#include <sys/utsname.h>
#endif
#include <time.h> 

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <iterator>

#include <capsGroup.h>
#include "glCapsViewerCore.h"

using namespace std;

uint32_t toHex(std::string str)
{
	return std::strtoul(str.c_str(), 0, 16);
}

struct iequal
{
    bool operator()(int c1, int c2) const
    {
        return std::toupper(c1) == std::toupper(c2);
    }
};

bool iequals(const std::string& str1, const std::string& str2)
{
    return std::equal(str1.begin(), str1.end(), str2.begin(), iequal());
}

template <class Container>
void split(const std::string& str, Container& cont, char delim = ' ')
{
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        cont.push_back(token);
    }
}

/// <summary>
/// Checks if an extension is supported (GL or os specific)
/// </summary>
/// <param name="ext">Name of the extension to check</param>
/// <returns>true if extension is present</returns>
bool glCapsViewerCore::extensionSupported(string ext) 
{
	for (auto& currExt : extensions) {
		if (iequals(currExt, ext)) {
			return true;
		}
	}

	for (auto& currExt : osextensions) {
		if (iequals(currExt, ext)) {
			return true;
		}
	}

	return false;
}

void glCapsViewerCore::clear() {
	extensions.clear();
	osextensions.clear();
	implementation.clear();
	capgroups.clear();
	compressedFormats.clear();
	description = "";
	submitter = "";	
}

/// <summary>
/// Reads the name of the operating system.
/// </summary>
/// <returns>Short name of the operating system</returns>
// TODO : Mac OSX
string glCapsViewerCore::readOperatingSystem()
{
	#ifdef _WIN32
		HKEY key;
		if (RegOpenKey(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows NT\\CurrentVersion\\"), &key) == ERROR_SUCCESS) {
			TCHAR value[MAX_PATH];
			DWORD type = REG_SZ;
			DWORD value_length = MAX_PATH;
			RegQueryValueEx(key, L"ProductName", NULL, &type, (LPBYTE)&value, &value_length);
			value[value_length] = wchar_t(0);
			char sz[1024] = { '\0' };
			wcstombs(sz, value, sizeof(sz));
			string osname = sz;
			RegCloseKey(key);
			// TODO : Clean up os name
			if (osname.find("Windows 10") != string::npos)
				return "Windows 10";
			if (osname.find("Windows 8") != string::npos)
				return "Windows 8";
			if (osname.find("Windows 7") != string::npos)
				return "Windows 7";
			if (osname.find("Vista") != string::npos)
				return "Windows Vista";
			if (osname.find("XP") != string::npos)
				return "Windows XP";
			if (osname.find("2000") != string::npos)
				return "Windows 2000";
			return "Windows (unknown)";
		} else {
			return "Windows (unknown)";
		}
	#endif
	#ifdef __linux__
		struct utsname osname;
		uname(&osname);
		stringstream osinfo;
		osinfo << osname.sysname << " " << osname.release << " (" << osname.machine << ")";
		return osinfo.str();
	#endif
	// TODO : MacOSX
}

void glCapsViewerCore::readExtensions()
{
	// Use glGetStringi if available (GL 3.x)
	if ((GL_VERSION_3_0) && (glGetStringi != NULL)) {
		GLint numExtensions;
		glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
		for (int i = 0; i < numExtensions; i++) {
			const GLubyte* glExt = glGetStringi(GL_EXTENSIONS, i);
			string ext = reinterpret_cast<const char*>(glExt);
			extensions.push_back(ext);
		}
	}
	else {
		const GLubyte* glExtensions = glGetString(GL_EXTENSIONS);
		string extensionString = reinterpret_cast<const char*>(glExtensions);
		split(extensionString, extensions, ' ');
	}
}

void glCapsViewerCore::printExtensions()
{
	cout << "gl extensions (" << extensions.size() << ") : \n";
	for (auto& s : extensions)
		std::cout << '"' << s << '"' << '\n';
}

void glCapsViewerCore::readOsExtensions() 
{
#ifdef _WIN32
	typedef const char* (WINAPI * PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC hdc);
	PFNWGLGETEXTENSIONSSTRINGARBPROC  pwglGetExtensionsStringARB = 0;
	#define wglGetExtensionsStringARB pwglGetExtensionsStringARB

	wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
	if (wglGetExtensionsStringARB)
	{
		const char* wglExtensions = wglGetExtensionsStringARB(wglGetCurrentDC());
		if (wglExtensions)
		{
			std::string wglExtensionString = reinterpret_cast<const char*>(wglExtensions);
			split(wglExtensionString, osextensions, ' ');
		}
	}
#endif
#ifdef __linux__
	Display *dpy(XOpenDisplay(NULL));
	int screen = XDefaultScreen(dpy);
	const char* glXExtensions = glXQueryExtensionsString(dpy, screen);
	if (glXExtensions)
	{
		std::string glXExtensionString = reinterpret_cast<const char*>(glXExtensions);
		split(glXExtensionString, osextensions, ' ');
	}
#endif
	// TODO : MacOSX
}

void glCapsViewerCore::readImplementation()
{
	implementation["Operating system"] = readOperatingSystem();
	implementation["Vendor"] = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	implementation["Renderer"] = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	implementation["OpenGL version"] = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	implementation["Shading language version"] = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION_ARB));

	stringstream ss;
	ss << implementation["Vendor"] << " " << implementation["Renderer"] << " " << implementation["OpenGL version"] << " (" << implementation["Operating system"] << ")";
	description = ss.str();

	// Not visible, but will be exported to xml
	capsViewer::capsGroup capsGroup;
	capsGroup.name = "implementation";
	capsGroup.supported = true;
	capsGroup.visible = false;
	capsGroup.addCapability("GL_VENDOR", GL_VENDOR, "glstring", 1);
	capsGroup.addCapability("GL_RENDERER", GL_RENDERER, "glstring", 1);
	capsGroup.addCapability("GL_VERSION", GL_VERSION, "glstring", 1);
	capsGroup.addCapability("GL_SHADING_LANGUAGE_VERSION", GL_SHADING_LANGUAGE_VERSION, "glstring", 1);
	capgroups.push_back(capsGroup);
}


/// <summary>
/// Gets the enum string of the passed OpenGL enum
/// </summary>
/// <param name="formatEnum">OpenGL enum of the to get the string for</param>
/// <returns>String containing the name of the enum, or enum value as string if enum was not found</returns>
string glCapsViewerCore::getEnumName(GLint glenum)
{
	
	if (enumList.count(glenum) > 0) {
		return enumList[glenum];
	}
	else {
		return to_string(glenum);
	}
}

void glCapsViewerCore::readCompressedFormats()
{
	GLint numFormats;
	glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &numFormats);
	GLint* formats;
	formats = new GLint[numFormats];
	glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, formats);
	for (int i = 0; i < numFormats; i++) {
		compressedFormats.push_back(formats[i]);
	}
	delete[] formats;
}

void glCapsViewerCore::readInternalFormats()
{
	internalFormatTargets.clear();
	// TODO : Move to xml
	GLenum targets[] = { GL_TEXTURE_1D, GL_TEXTURE_1D_ARRAY, GL_TEXTURE_2D, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_3D, GL_TEXTURE_CUBE_MAP,
		GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_RECTANGLE, GL_TEXTURE_BUFFER, GL_RENDERBUFFER, GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_2D_MULTISAMPLE_ARRAY };
	for (auto& target : targets) {
		internalFormatTargets.push_back(capsViewer::internalFormatTarget(target, compressedFormats));
	}

	bool internalformatquery2 = extensionSupported("GL_ARB_internalformat_query2");
	for (auto& target : internalFormatTargets) {
		target.getInternalFormatInfo(internalformatquery2);
	}

}

void glCapsViewerCore::readSPIRVExtensions()
{
    SPIRVExtensions.clear();
    GLint numExtensions;
    glGetIntegerv(GL_NUM_SPIR_V_EXTENSIONS, &numExtensions);
    for (int i = 0; i < numExtensions; i++) {
        const GLubyte* glExt = glGetStringi(GL_SPIR_V_EXTENSIONS, i);
        string ext = reinterpret_cast<const char*>(glExt);
        SPIRVExtensions.push_back(ext);
    }
}

/// <summary>
/// Loads mapping list of OpenGL enum values and strings from xml file
/// </summary>
bool glCapsViewerCore::loadEnumList()
{
    Q_INIT_RESOURCE(glCapsViewer);
    QFile enumListxml(":/glcapsviewer/Resources/enumList.xml");
    enumListxml.open(QFile::ReadOnly);
    auto buffer = enumListxml.readAll();
	buffer.push_back('\0');
    QXmlStreamReader xmlStream(buffer.data());
	
	while (!xmlStream.atEnd())  {

		if (xmlStream.isStartElement())
		{
			QString xmlTag(xmlStream.name().toString());

			if (xmlTag == "enum") {
				QXmlStreamAttributes attrib = xmlStream.attributes();
				QString enumVal = attrib.value("value").toString();
				QString enumName = xmlStream.readElementText();
				// Convert from hex value
				uint32_t enumValue = toHex(enumVal.toStdString());
				enumList[enumValue] = enumName.toStdString();
			}
		}

		xmlStream.readNext();

	}

	return (!xmlStream.hasError());
}


string glCapsViewerCore::reportToXml() 
{
	QString xmlStr;
	QXmlStreamWriter xmlWriter(&xmlStr);
	xmlWriter.setAutoFormatting(true);
	xmlWriter.writeStartDocument();

	xmlWriter.writeStartElement("implementationinfo");
	xmlWriter.writeTextElement("fileversion", QString::fromStdString(fileVersion));
	xmlWriter.writeTextElement("appversion", QString::fromStdString(appVersion));
	xmlWriter.writeTextElement("description", QString::fromStdString(description));
	xmlWriter.writeTextElement("contexttype", QString::fromStdString(contextType));

	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	char buffer[256];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
	string datetimestr = reinterpret_cast<const char*>(buffer);
	xmlWriter.writeTextElement("date", QString::fromStdString(datetimestr));
	xmlWriter.writeTextElement("submitter", QString::fromStdString(submitter));
	xmlWriter.writeTextElement("os", QString::fromStdString(implementation["Operating system"]));
	xmlWriter.writeTextElement("comment", QString::fromStdString(comment));

	// Extensions
	xmlWriter.writeStartElement("extensions");
	for (auto& ext : extensions) {
		xmlWriter.writeTextElement("extension", QString::fromStdString(ext));
	}
	for (auto& ext : osextensions) {
		xmlWriter.writeTextElement("extension", QString::fromStdString(ext));
	}
	xmlWriter.writeEndElement();

	// Implementation info and caps
	xmlWriter.writeStartElement("caps");
	for (auto& capgroup : capgroups) 
	{
		for (auto& cap : capgroup.capabilities) 
		{
			xmlWriter.writeStartElement("cap");
			xmlWriter.writeAttribute("id", QString::fromStdString(cap.first));
			xmlWriter.writeTextElement("value", QString::fromStdString(cap.second));
			xmlWriter.writeEndElement();
		}
	}
	xmlWriter.writeEndElement();

	// compressed texture formats
	xmlWriter.writeStartElement("compressedtextureformats");
	for (auto& compressedFormat : compressedFormats) {
		xmlWriter.writeTextElement("compressedtextureformat", QString::fromStdString(to_string(compressedFormat)));
	}
	xmlWriter.writeEndElement();

    // SPIR-V Extensions
    xmlWriter.writeStartElement("spirvextensions");
    for (auto& ext : SPIRVExtensions) {
        xmlWriter.writeTextElement("spirvextension", QString::fromStdString(ext));
    }
    xmlWriter.writeEndElement();

	// Internal formats
	// TODO : Not yet in database
#ifdef INTERNALFORMATINFO
	xmlWriter.writeStartElement("internalformatinformation");
	for (auto& internalFormatTarget : internalFormatTargets) {
		xmlWriter.writeStartElement("target");
		string targetName = getEnumName(internalFormatTarget.target);
		xmlWriter.writeAttribute("name", QString::fromStdString(targetName));

		for (auto& internalTextureFormat : internalFormatTarget.textureFormats) {
			string formatName = getEnumName(internalTextureFormat.textureFormat);
			xmlWriter.writeStartElement("format");
			xmlWriter.writeAttribute("name", QString::fromStdString(formatName));
			xmlWriter.writeAttribute("supported", internalTextureFormat.supported ? "true" : "false");

			for (auto& formatInfoValue : internalTextureFormat.formatInfoValues) {
				string infoVal = to_string(formatInfoValue.infoValue);
				xmlWriter.writeStartElement("value");
				xmlWriter.writeAttribute("name", QString::fromStdString(formatInfoValue.infoString));
				xmlWriter.writeCharacters(QString::fromStdString(infoVal));
				xmlWriter.writeEndElement(); // value
			}

			xmlWriter.writeEndElement(); // format
		}
		xmlWriter.writeEndElement(); // target
	}
	xmlWriter.writeEndElement(); // internalformatinformation
#endif

	xmlWriter.writeEndElement(); // root

	return xmlStr.toStdString();
}

void glCapsViewerCore::exportXml(string fileName)
{
	string xml = reportToXml();
	ofstream destfile;
	destfile.open(fileName);
	destfile << xml;
	destfile.close();
}

void glCapsViewerCore::readCapabilities()
{

	ifstream enumListxml("capslist.xml");
	vector<char> buffer((istreambuf_iterator<char>(enumListxml)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	QXmlStreamReader xmlStream(&buffer[0]);
	QXmlStreamAttributes nodeAttribs;

	while (!xmlStream.atEnd())  {

		xmlStream.readNext();

		if (xmlStream.name() == "category") {
			// TODO : wgl and glx need to be checked different (wglewIsSupported, etc.)
			nodeAttribs = xmlStream.attributes();
			QString catName = nodeAttribs.value("name").toString();
			capsViewer::capsGroup capsGroup;
			capsGroup.name = catName.toStdString();
			capsGroup.supported = false;

			while (!xmlStream.atEnd()) {

				xmlStream.readNext();

				if ((xmlStream.name() == "requirements") && (xmlStream.isStartElement())) {
					QXmlStreamAttributes nodeAttribs = xmlStream.attributes();
					string reqExt = nodeAttribs.value("extension").toString().toStdString();
					string reqVersion = nodeAttribs.value("version").toString().toStdString();

					// Check extension
					if (!reqExt.empty()) {
						capsGroup.supported = extensionSupported(reqExt);
					}

					if (!reqVersion.empty()) {
						replace(reqVersion.begin(), reqVersion.end(), '.', '_');
						stringstream glewVersion;
						glewVersion << "GL_VERSION_" << reqVersion;
						capsGroup.supported = glewIsSupported(glewVersion.str().c_str());
					}

					if ((reqExt.empty()) && (reqVersion.empty())) {
						capsGroup.supported = true;
					}
				}

				if (capsGroup.supported) 
				{
					if ((xmlStream.name() == "cap") && (xmlStream.isStartElement())) 
					{
						QXmlStreamAttributes nodeAttribs = xmlStream.attributes();
						string capName = nodeAttribs.value("name").toString().toStdString();
						string capType = nodeAttribs.value("type").toString().toStdString();
						string capEnumStr = nodeAttribs.value("enum").toString().toStdString();
						int capComponents = nodeAttribs.value("components").toInt();
						uint32_t capEnum = toHex(nodeAttribs.value("enum").toString().toStdString());
						capsGroup.addCapability(capName, capEnum, capType, capComponents);
					}
				}

				if (xmlStream.name() == "category") 
				{
					break;
				}

			}

			capgroups.push_back(capsGroup);

		}

	}
}
