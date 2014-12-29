/*
*
* OpenGL hardware capability viewer and database
*
* Server http communication class implementation
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
#include <map>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDateTime>
#include <QFile>
#include <QDebug>

using namespace std;

class reportInfo
{
	public:
		string device;
		string version;
		string operatingSystem;
		int reportId;
};

class glCapsViewerHttp :
	public QObject
{
	Q_OBJECT
private:
	string baseUrl = "http://delphigl.de/glcapsviewer/";
	QNetworkAccessManager *manager;
	string httpGet(string url);
	string httpPost(string url, string data);
	string encodeUrl(string url);
public:
	int getReportId(string description);
	bool checkReportPresent(string description);
	bool checkReportCanUpdate(int reportId, string caps);
	vector<string> fetchDevices();
	vector<reportInfo> fetchDeviceReports(string device);
	string fetchReport(int reportId);
	string postReport(string xml);
	string postReportForUpdate(string xml);
	string fetchCapsList();
	glCapsViewerHttp();
	~glCapsViewerHttp();
};

