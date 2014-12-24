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

#include "glCapsViewerHttp.h"
#include <rapidxml.hpp>
#include <sstream>
#include <QMessageBox>
#include <QEventLoop>
#include <QHttpMultiPart>

glCapsViewerHttp::glCapsViewerHttp()
{
}


glCapsViewerHttp::~glCapsViewerHttp()
{
}

/// <summary>
/// Execute http get request 
/// </summary>
/// <param name="url">url for the http get request</param>
/// <returns>string of the http get request or empty string in case of failure</returns>
string glCapsViewerHttp::httpGet(string url)
{
	manager = new QNetworkAccessManager(NULL);

	QUrl qurl(QString::fromStdString(url));
	QNetworkReply* reply = manager->get(QNetworkRequest(qurl));

	QEventLoop loop;
	connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();

	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray bytes = reply->readAll();
		QString replyStr(bytes);
		delete(manager);
		return replyStr.toStdString();
	}
	else
	{
		QString err;
		err = reply->errorString();
		delete(manager);
		return "";
	}
}

/// <summary>
/// Execute http post
/// </summary>
/// <param name="url">url for the http post</param>
/// <param name="data">string data to post</param>
/// <returns>Server answer</returns>
string glCapsViewerHttp::httpPost(string url, string data) 
{
	manager = new QNetworkAccessManager(NULL);

	QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

	QHttpPart xmlPart;
	xmlPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"data\"; filename=\"glcapsviewerreport.xml\""));
	xmlPart.setBody(QString::fromStdString(data).toLatin1());
	multiPart->append(xmlPart);

	QUrl qurl(QString::fromStdString(url));
	QNetworkRequest request(qurl);
	QNetworkReply *reply = manager->post(request, multiPart);
	multiPart->setParent(reply);
	
	QEventLoop loop;
	connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();

	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray bytes = reply->readAll();
		QString replyStr(bytes);
		delete(manager);
		return replyStr.toStdString();
	}
	else
	{
		QString err;
		err = reply->errorString();
		delete(manager);
		return "";
	}

}


/// <summary>
/// Gets the Id of a report from the online database
/// </summary>
/// <param name="description">Description of the report to get the Id for</param>
/// <returns></returns>
int glCapsViewerHttp::getReportId(string description)
{
	string httpReply;
	stringstream urlss;
	urlss << baseUrl << "gl_checkreport.php?description=" << description;
	httpReply = httpGet(urlss.str());
	return (!httpReply.empty()) ? atoi(httpReply.c_str()) : -1;
}

/// <summary>
/// Checks if the report is present in the online database
/// </summary>
/// <param name="description">Description of the report to get the Id for</param>
/// <returns></returns>
bool glCapsViewerHttp::checkReportPresent(string description)
{
	int reportID = getReportId(description);
	return (reportID > -1) ? true : false;
}

/// <summary>
/// Fechtes an xml with all report data from the online database
/// </summary>
/// <param name="reportId">Description of the report to get the report xml for</param>
/// <returns>xml string</returns>
string glCapsViewerHttp::fetchReport(int reportId)
{
	string reportXml;
	stringstream urlss;
	urlss << baseUrl << "services/gl_getreport.php?reportId=" << reportId;
	reportXml = httpGet(urlss.str());
	return reportXml;
}

/// <summary>
/// Posts the given xml for a report to the database	
/// </summary>
/// <returns>todo</returns>
string glCapsViewerHttp::postReport(string xml)
{
	string httpReply;
	stringstream urlss;
	urlss << baseUrl << "/convertreport.php";
	httpReply = httpPost(urlss.str(), xml);
	return httpReply;
}

/// <summary>
/// Fetches all available devices from the online database and lists them in the ui
/// </summary>
/// <returns></returns>
vector<string> glCapsViewerHttp::fetchDevices() 
{
	vector<string> deviceList;
	string httpReply;
	stringstream urlss;
	urlss << baseUrl << "services/gl_getdevices.php";
	httpReply = httpGet(urlss.str());

	if (!httpReply.empty())
	{
		using namespace rapidxml;
		xml_document<> doc;
		xml_node<> * root_node;
		doc.parse<0>(&httpReply[0]);
		root_node = doc.first_node("devices");

		for (xml_node<> * device_node = root_node->first_node("device"); device_node; device_node = device_node->next_sibling("device"))
		{
			deviceList.push_back(device_node->value());
		}
	}

	return deviceList;
}

/// <summary>
/// Fetches all available reports for the given device
/// </summary>
/// <param name="device">Name of the device to select reports for (GL_RENDERER) </param>
/// <returns></returns>
vector<reportInfo> glCapsViewerHttp::fetchDeviceReports(string device)
{
	vector<reportInfo> reportList;
	string httpReply;
	stringstream urlss;
	urlss << baseUrl << "services/gl_getdevicereports.php?glrenderer=" << device;
	httpReply = httpGet(urlss.str());

	if (!httpReply.empty())
	{
		using namespace rapidxml;
		xml_document<> doc;
		xml_node<> * root_node;
		doc.parse<0>(&httpReply[0]);
		root_node = doc.first_node("reports");

		for (xml_node<> * device_node = root_node->first_node("report"); device_node; device_node = device_node->next_sibling("report"))
		{
			reportInfo report;
			report.device = device;
			report.version = device_node->value();
			report.operatingSystem = device_node->first_attribute("os")->value();
			report.reportId = atoi(device_node->first_attribute("id")->value());
			reportList.push_back(report);
		}
	};

	return reportList;
}

