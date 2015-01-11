/*
*
* OpenGL hardware capability viewer and database
*
* Main window class
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

#include "glCapsViewer.h"
#include "glCapsViewerHttp.h"
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GLFW/glfw3.h>
#include <QDesktopServices>
#include <QtWidgets/QTextBrowser>
#include <QMessageBox>
#include <QStyleFactory>
#include <QDebug>
#include <QFileDialog>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QListWidgetItem>
#include <QTreeWidget>
#include <QComboBox>
#include <QInputDialog>
#include <sstream>  
#include <rapidxml.hpp>

glCapsViewer::glCapsViewer(QWidget *parent)
	: QMainWindow(parent)
{
	QApplication::setStyle(QStyleFactory::create("Fusion"));
	ui.setupUi(this);
	connect(ui.actionRefresh, SIGNAL(triggered()), this, SLOT(slotRefreshReport()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(slotClose()));
	connect(ui.actionSave_xml, SIGNAL(triggered()), this, SLOT(slotExportXml()));
	connect(ui.actionDatabase, SIGNAL(triggered()), this, SLOT(slotBrowseDatabase()));
	connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(slotAbout()));
	connect(ui.actionUpload, SIGNAL(triggered()), this, SLOT(slotUpload()));
	connect(ui.pushButtonRefreshDataBase, SIGNAL(released()), this, SLOT(slotRefreshDatabase()));
	connect(ui.listWidgetDatabaseDevices, SIGNAL(itemSelectionChanged()), this, SLOT(slotDatabaseDevicesItemChanged()));
	connect(ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotTabChanged(int)));
	connect(ui.comboBoxDeviceVersions, SIGNAL(currentIndexChanged(int)), this, SLOT(slotDeviceVersionChanged(int)));

	ui.tableWidgetDatabaseDeviceReport->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
	ui.tableWidgetDatabaseDeviceReport->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

	// TODO : Same as capslist.xml, check if exists and if not, download from server
	core.loadEnumList();
}

glCapsViewer::~glCapsViewer()
{

}

/// <summary>
///	Updates the report status label
/// </summary>
void glCapsViewer::updateReportState()
{
	glCapsViewerHttp glhttp;
	if (glhttp.checkReportPresent(core.description)) {
		ui.labelReportPresent->setText("<font color='#00813e'>Device already present in database, all fields up-to-date</font>");
		// Report present, check if it can be updated		
		int reportId = glhttp.getReportId(core.description);
		if (canUpdateReport(reportId)) {
			ui.labelReportPresent->setText("<font color='#0000FF'>Device already present in database, but can be updated with missing values!</font>");
		}
	}
	else {
		ui.labelReportPresent->setText("<font color='#bc0003'>Device not yet present in database</font>");
	}
	ui.labelReportPresent->setVisible(true);
}

/// <summary>
///	Reads information about implementation-dependent support for internal formats
/// TODO : Create xml structure instead of hardcoding
/// TODO : Data structures for xml export (and database upload)
/// TODO : Move to core
/// </summary>

void colorInternalFormatItem(QTreeWidgetItem *item, int column) {
	if (item->text(column) == "GL_NONE") 
		item->setTextColor(column, QColor::fromRgb(255, 0, 0));
	if (item->text(column) == "GL_CAVEAT_SUPPORT") 
		item->setTextColor(column, QColor::fromRgb(255, 150, 0));
	if (item->text(column) == "GL_FULL_SUPPORT") 
		item->setTextColor(column, QColor::fromRgb(0, 128, 0));
}

void glCapsViewer::getInternalFormatInfo()
{

	QTreeWidget *tree = ui.treeWidgetInternalFormats;
	tree->header()->resizeSection(0, 250);
	tree->clear();

	// Check if extension is supported
	if (!core.extensionSupported("GL_ARB_internalformat_query")) {
		QTreeWidgetItem *infoItem = new QTreeWidgetItem(tree);
		infoItem->setText(0, "Extension not supported");
		infoItem->setTextColor(0, QColor::fromRgb(255, 0, 0));
		return;
	}

	// internal texture formats
	// TODO : Just some quick testing, move to core
	// TODO : GL_ARB_INTERNALFORMAT_QUERY2 

	map<GLenum, string> targets;
	targets[GL_TEXTURE_1D] = "GL_TEXTURE_1D";
	targets[GL_TEXTURE_1D_ARRAY] = "GL_TEXTURE_1D_ARRAY";
	targets[GL_TEXTURE_2D] = "GL_TEXTURE_2D";
	targets[GL_TEXTURE_2D_ARRAY] = "GL_TEXTURE_2D_ARRAY";
	targets[GL_TEXTURE_3D] = "GL_TEXTURE_3D";
	targets[GL_TEXTURE_CUBE_MAP] = "GL_TEXTURE_CUBE_MAP";
	targets[GL_TEXTURE_CUBE_MAP_ARRAY] = "GL_TEXTURE_CUBE_MAP_ARRAY";
	targets[GL_TEXTURE_RECTANGLE] = "GL_TEXTURE_RECTANGLE";
	targets[GL_TEXTURE_BUFFER] = "GL_TEXTURE_BUFFER";
	targets[GL_RENDERBUFFER] = "GL_RENDERBUFFER";
	targets[GL_TEXTURE_2D_MULTISAMPLE] = "GL_TEXTURE_2D_MULTISAMPLE";
	targets[GL_TEXTURE_2D_MULTISAMPLE_ARRAY] = "GL_TEXTURE_2D_MULTISAMPLE_ARRAY";

	for (auto& target : targets) {

		QTreeWidgetItem *targetItem = new QTreeWidgetItem(tree);
		targetItem->setText(0, QString::fromStdString(target.second));

		map<GLenum, string> internalFormats;
		vector<string> internalFormatNames;
		// TODO : List of internalFormats from xml
		internalFormats[GL_DEPTH_COMPONENT] = "GL_DEPTH_COMPONENT";
		internalFormats[GL_DEPTH_STENCIL] = "GL_DEPTH_STENCIL";
		internalFormats[GL_STENCIL_INDEX] = "GL_STENCIL_INDEX";
		internalFormats[GL_RED] = "GL_RED";
		internalFormats[GL_RG] = "GL_RG";
		internalFormats[GL_RGB] = "GL_RGB";
		internalFormats[GL_RGBA] = "GL_RGBA";
  		internalFormats[GL_DEPTH_COMPONENT32F] = "GL_DEPTH_COMPONENT32F";
		//internalFormats[GL_RGBA8] = "GL_RGBA8";
		//internalFormats[GL_RGBA16] = "GL_RGBA16";
		//internalFormats[GL_DEPTH_COMPONENT16] = "GL_DEPTH_COMPONENT16";
		// Add all detected compressed formats
		for (auto& compressedFormat : core.compressedFormats) {
			internalFormats[compressedFormat] = core.getEnumName(compressedFormat);
		}

		map<GLenum, string> pnames;
		// GL_ARB_internalformat_query
		pnames[GL_INTERNALFORMAT_PREFERRED] = "GL_INTERNALFORMAT_PREFERRED";
		pnames[GL_READ_PIXELS_FORMAT] = "GL_READ_PIXELS_FORMAT";
		pnames[GL_READ_PIXELS_TYPE] = "GL_READ_PIXELS_TYPE";
		pnames[GL_TEXTURE_IMAGE_FORMAT] = "GL_TEXTURE_IMAGE_FORMAT";
		pnames[GL_TEXTURE_IMAGE_TYPE] = "GL_TEXTURE_IMAGE_TYPE";
		pnames[GL_GET_TEXTURE_IMAGE_FORMAT] = "GL_GET_TEXTURE_IMAGE_FORMAT";
		pnames[GL_GET_TEXTURE_IMAGE_TYPE] = "GL_GET_TEXTURE_IMAGE_TYPE";

		// Only for compressed
		pnames[GL_TEXTURE_COMPRESSED_BLOCK_WIDTH] = "GL_TEXTURE_COMPRESSED_BLOCK_WIDTH";
		pnames[GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT] = "GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT";
		pnames[GL_TEXTURE_COMPRESSED_BLOCK_SIZE] = "GL_TEXTURE_COMPRESSED_BLOCK_SIZE";

		// GL_ARB_internalformat_query2
		if (core.extensionSupported("GL_ARB_internalformat_query2")) {
			pnames[GL_INTERNALFORMAT_SUPPORTED] = "GL_INTERNALFORMAT_SUPPORTED";
			pnames[GL_TEXTURE_COMPRESSED] = "GL_TEXTURE_COMPRESSED";
			pnames[GL_MAX_WIDTH] = "GL_MAX_WIDTH";
			pnames[GL_MAX_HEIGHT] = "GL_MAX_HEIGHT";
			pnames[GL_MAX_DEPTH] = "GL_MAX_DEPTH";
			// pnames[GL_MAX_COMBINED_DIMENSIONS] = "GL_MAX_COMBINED_DIMENSIONS"; TODO : Usually GL_MAX...*GL_MAX...*GL_MAX, redundant info
			pnames[GL_FRAMEBUFFER_BLEND] = "GL_FRAMEBUFFER_BLEND";
			pnames[GL_READ_PIXELS] = "GL_READ_PIXELS";
			pnames[GL_MANUAL_GENERATE_MIPMAP] = "GL_MANUAL_GENERATE_MIPMAP";
			pnames[GL_AUTO_GENERATE_MIPMAP] = "GL_AUTO_GENERATE_MIPMAP";
			pnames[GL_FILTER] = "GL_FILTER";
		}

		map<GLenum, string> supportList;
		supportList[GL_VERTEX_TEXTURE] = "GL_VERTEX_TEXTURE";
		supportList[GL_TESS_CONTROL_TEXTURE] = "GL_TESS_CONTROL_TEXTURE";
		supportList[GL_TESS_EVALUATION_TEXTURE] = "GL_TESS_EVALUATION_TEXTURE";
		supportList[GL_GEOMETRY_TEXTURE] = "GL_GEOMETRY_TEXTURE";
		supportList[GL_FRAGMENT_TEXTURE] = "GL_FRAGMENT_TEXTURE";
		supportList[GL_COMPUTE_TEXTURE] = "GL_COMPUTE_TEXTURE";

		supportList[GL_TEXTURE_SHADOW] = "GL_TEXTURE_SHADOW";
		supportList[GL_TEXTURE_GATHER] = "GL_TEXTURE_GATHER";
		supportList[GL_TEXTURE_GATHER_SHADOW] = "GL_TEXTURE_GATHER_SHADOW";

		supportList[GL_SHADER_IMAGE_LOAD] = "GL_SHADER_IMAGE_LOAD";
		supportList[GL_SHADER_IMAGE_STORE] = "GL_SHADER_IMAGE_STORE";
		supportList[GL_SHADER_IMAGE_ATOMIC] = "GL_SHADER_IMAGE_ATOMIC";

		for (auto& internalFormat : internalFormats) {

			QTreeWidgetItem *formatItem = new QTreeWidgetItem(targetItem);
			formatItem->setText(0, QString::fromStdString(internalFormat.second));
			formatItem->addChild(targetItem);

			// Check if internal format is supported first
			GLint formatSupported;
			glGetInternalformativ(target.first, internalFormat.first, GL_INTERNALFORMAT_SUPPORTED, 1, &formatSupported);

			if (formatSupported == GL_FALSE) {
				formatItem->setText(1, "not supported");
				formatItem->setTextColor(1, QColor::fromRgb(100, 100, 100));
				formatItem->setTextColor(1, QColor::fromRgb(100, 100, 100));
				continue;
			}

			for (auto& pname : pnames) {
				GLint param;
				glGetInternalformativ(target.first, internalFormat.first, pname.first, 1, &param);

				if ((pname.second == "GL_MAX_WIDTH") || (pname.second == "GL_MAX_HEIGHT") || (pname.second == "GL_MAX_DEPTH")) {
					if (param == 0) {
						continue;
					}
				}

				QTreeWidgetItem *paramItem = new QTreeWidgetItem(formatItem);
				paramItem->setText(0, QString::fromStdString(pname.second));
				string enumString;
				enumString = core.getEnumName(param);

				// Switch some values
				if ((pname.first == GL_INTERNALFORMAT_SUPPORTED) || (pname.first == GL_TEXTURE_COMPRESSED))  {
					enumString = (param == 0) ? "GL_FALSE" : "GL_TRUE";
				}

				paramItem->setText(1, QString::fromStdString(enumString));
				paramItem->addChild(formatItem);
				colorInternalFormatItem(paramItem, 1);
			}

			QTreeWidgetItem *subItem;
			// Different supports
			subItem = new QTreeWidgetItem(formatItem);
			subItem->setText(0, "Shader support");
			for (auto& supportType : supportList) {
				GLint param;
				glGetInternalformativ(target.first, internalFormat.first, supportType.first, 1, &param);
				string enumString;
				enumString = core.getEnumName(param);
				QTreeWidgetItem *paramItem = new QTreeWidgetItem(subItem);
				paramItem->setText(0, QString::fromStdString(supportType.second));
				paramItem->setText(1, QString::fromStdString(enumString));
				paramItem->addChild(subItem);
				colorInternalFormatItem(paramItem, 1);
			}

			targetItem->sortChildren(0, Qt::AscendingOrder);

		}

	}
}

/// <summary>
///	Reads implementation details, extensions and capabilities
///	and displays the report
/// </summary>
void glCapsViewer::generateReport()
{
	ui.treeWidget->clear();
	ui.treeWidgetExtensions->clear();
	ui.listWidgetCompressedFormats->clear();

	core.readExtensions();
	core.readOsExtensions();
	core.readImplementation();
	core.readCapabilities();
	// TODO : Check if extension is present!
	core.readCompressedFormats();

	ui.labelDescription->setText(QString::fromStdString(core.description));

	updateReportState();

	// Implementation detail
	stringstream ss;

	QTreeWidget *tree = ui.treeWidget;
	tree->header()->resizeSection(0, 250);
	QTreeWidgetItem *impItem = new QTreeWidgetItem(tree);
	impItem->setText(0, "Implementation details");

	for (auto& s : core.implementation) {
		QTreeWidgetItem *treeItem = new QTreeWidgetItem(impItem);
		treeItem->setText(0, QString::fromStdString(s.first));
		treeItem->setText(1, QString::fromStdString(s.second));
		impItem->addChild(treeItem);
	}
	impItem->setExpanded(true);

	// Capabilities
	for (auto& group : core.capgroups) {
		if (!group.visible) {
			continue;
		}
		QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(group.name));
		item->setTextColor(QColor::fromRgb(0, 0, 255));

		QTreeWidgetItem *groupItem = new QTreeWidgetItem(tree);
		groupItem->setText(0, QString::fromStdString(group.name));

		if (group.supported) {
			groupItem->setText(1, QString::fromStdString(to_string(group.capabilities.size())));
		}
		else {
			groupItem->setText(1, "not available");
			groupItem->setTextColor(1, QColor::fromRgb(255, 0, 0));
		}

		if (group.supported) {
			for (auto& cap : group.capabilities)	{
				QTreeWidgetItem *capItem = new QTreeWidgetItem(groupItem);
				capItem->setText(0, QString::fromStdString(cap.first));
				capItem->setText(1, QString::fromStdString(cap.second));
				if (cap.second == "n/a") {
					capItem->setTextColor(0, QColor::fromRgb(100, 100, 100));
					capItem->setTextColor(1, QColor::fromRgb(100, 100, 100));
				}
				groupItem->addChild(capItem);
			}
		}
		else {
			QTableWidgetItem *item = new QTableWidgetItem("  Not supported");
			item->setTextColor(QColor::fromRgb(255, 0, 0));
		}
	}

	// Extensions
	tree = ui.treeWidgetExtensions;
	tree->header()->resizeSection(0, 250);

	stringstream cap;
	cap << "OpenGL extensions (" << core.extensions.size() << ")";
	QTreeWidgetItem *extItem = new QTreeWidgetItem(tree);
	extItem->setText(0, QString::fromStdString(cap.str()));

	for (auto& s : core.extensions) {
		if (s == "") continue;
		QTreeWidgetItem *capItem = new QTreeWidgetItem(extItem);
		capItem->setText(0, QString::fromStdString(s));
		extItem->addChild(capItem);
	}

	extItem->sortChildren(0, Qt::AscendingOrder);
	extItem->setExpanded(true);

	// OS 
	cap.str("");
	cap << "OS specific extensions (" << core.osextensions.size() << ")";
	extItem = new QTreeWidgetItem(tree);
	extItem->setText(0, QString::fromStdString(cap.str()));

	for (auto& s : core.osextensions) {
		if (s == "") continue;
		QTreeWidgetItem *capItem = new QTreeWidgetItem(extItem);
		capItem->setText(0, QString::fromStdString(s));
		extItem->addChild(capItem);
	}

	extItem->sortChildren(0, Qt::AscendingOrder);
	extItem->setExpanded(true);

	// Supported Compressed texture formats
	// TODO : Display error if ext not supported or list empty
	for (auto& compressedFormat : core.compressedFormats) {
		string formatString = core.getEnumName(compressedFormat);
		QListWidgetItem *formatItem = new QListWidgetItem(QString::fromStdString(formatString), ui.listWidgetCompressedFormats);
		formatItem->setSizeHint(QSize(formatItem->sizeHint().height(), 24));
	}

	getInternalFormatInfo();

	// Tab captions
	stringstream tabText;
	tabText << "Extensions (" << core.extensions.size() + core.osextensions.size() << ")";
	ui.tabWidgetDevice->setTabText(0, QString::fromStdString(tabText.str()));
	tabText.str("");
	tabText << "Compressed tex. formats (" << core.compressedFormats.size() << ")";
	ui.tabWidgetDevice->setTabText(1, QString::fromStdString(tabText.str()));
}

bool glCapsViewer::contextTypeSelection() 
{
	// Get available context types
	core.availableContextTypes.clear();
	core.availableContextTypes.push_back("OpenGL default");
	// Core context
	if (wglewIsSupported("WGL_ARB_create_context_profile")) {
		core.availableContextTypes.push_back("OpenGL core context");
	}
	// OpenGL ES context
	// GLES 1.0
	if (wglewIsSupported("WGL_EXT_create_context_es_profile")) {
		core.availableContextTypes.push_back("OpenGL ES 1.0 context");
	}
	// GLES 2.0
	if (wglewIsSupported("WGL_EXT_create_context_es2_profile")) {
		core.availableContextTypes.push_back("OpenGL ES 2.0 context");
		// GLES 3.0 (superset of ES 2.0)
		if (glewIsSupported("GL_ARB_ES3_compatibility")) {
			core.availableContextTypes.push_back("OpenGL ES 3.0 context");
		}
	}
	core.contextType = "default";
	if (core.availableContextTypes.size() > 1) {
		QStringList items;
		for (auto& s : core.availableContextTypes) {
			items << QString::fromStdString(s);
		}

		bool ok;
		QString item = QInputDialog::getItem(NULL, QObject::tr("Select render context to create"), QObject::tr("Context type:"), items, 0, false, &ok);
		if (ok && !item.isEmpty()) {

			if (item == "OpenGL core context") {
				glewExperimental = GL_TRUE;
				GLenum err = glewInit();
				// Get max. supported OpenGL version for versioned core context
				GLint glVersionMajor, glVersionMinor;
				glGetIntegerv(GL_MAJOR_VERSION, &glVersionMajor);
				glGetIntegerv(GL_MINOR_VERSION, &glVersionMinor);
				// Create core context
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glVersionMajor);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glVersionMinor);
				glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
				core.contextType = "core";
			}

			if (item == "OpenGL ES 2.0 context") {
				glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
				core.contextType = "es2";
			}

			if (item == "OpenGL ES 3.0 context") {
				glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
				core.contextType = "es3";
			}

			return true;

		}
		else {
			return false;
		}
	}
	else {
		return true;
	}
}

void glCapsViewer::slotRefreshReport()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
	core.clear();
	core.contextType = "regular";
	glfwMakeContextCurrent(window);
	if (core.availableContextTypes.size() > 1) {
		QStringList items;
		for (auto& s : core.availableContextTypes) {
			items << QString::fromStdString(s);
		}

		bool ok;
		QString item = QInputDialog::getItem(NULL, QObject::tr("Select render context to create"), QObject::tr("Context type:"), items, 0, false, &ok);
		if (ok && !item.isEmpty()) {

			if (item == "OpenGL core context") {
				glewExperimental = GL_TRUE;
				GLenum err = glewInit();
				// Get max. supported OpenGL version for versioned core context
				GLint glVersionMajor, glVersionMinor;
				glGetIntegerv(GL_MAJOR_VERSION, &glVersionMajor);
				glGetIntegerv(GL_MINOR_VERSION, &glVersionMinor);
				// Create core context
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glVersionMajor);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glVersionMinor);
				glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
				core.contextType = "core";
			}

			if (item == "OpenGL ES 2.0 context") {
				glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
				core.contextType = "es2";
			}

			glfwDestroyWindow(window);
		};
	}

	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	window = glfwCreateWindow(320, 240, "glCapsViewer", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	generateReport();

	QApplication::restoreOverrideCursor();
}

void glCapsViewer::refreshDeviceList()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
	glCapsViewerHttp glchttp;
	vector<string> deviceList;
	deviceList = glchttp.fetchDevices();
	ui.listWidgetDatabaseDevices->clear();
	for (auto& device : deviceList) {
		QListWidgetItem *deviceItem = new QListWidgetItem(QString::fromStdString(device), ui.listWidgetDatabaseDevices);
		deviceItem->setSizeHint(QSize(deviceItem->sizeHint().height(), 24));
		deviceItem->setData(Qt::UserRole, QString::fromStdString(device));
		// Highlight if same as current device
		if (device == core.implementation["Renderer"]) {
			stringstream ss;
			ss << device << " (Your device)";
			deviceItem->setText(QString::fromStdString(ss.str()));
			deviceItem->setTextColor(QColor::fromRgb(50, 180, 50));
		}
	}
	QApplication::restoreOverrideCursor();
}

bool glCapsViewer::canUpdateReport(int reportId) {

	// Download report and check against xml
	glCapsViewerHttp glchttp;
	string reportXml = glchttp.fetchReport(reportId);

	using namespace rapidxml;
	xml_document<> doc;
	xml_node<> * root_node;
	doc.parse<0>(&reportXml[0]);
	root_node = doc.first_node("report");

	bool capsMissing = false;
	bool compressedFormatsMissing = false;
	bool internalFormatsMissing = false;

	// Check if caps are missing
	vector<string> capsList;
	for (auto& capsGroup : core.capgroups) {
		for (auto& cap : capsGroup.capabilities) {
			if (cap.second != "n/a") {
				capsList.push_back(cap.first);
			}
		}
	}
	xml_node<> * implementation_node = root_node->first_node("implementation");
	for (xml_node<> * value_node = implementation_node->first_node(); value_node; value_node = value_node->next_sibling())
	{
		string node_name = value_node->name();
		string node_value = value_node->value();
		if ((node_value != "") && (std::find(capsList.begin(), capsList.end(), node_name) == capsList.end())) {
			capsMissing = true;
			break;
		}
	}

	// Check if compressed formats are present
	xml_node<> * compressedFormatsNode = root_node->first_node("compressedtextureformats");
	if (compressedFormatsNode->first_node("format") == NULL) {
		compressedFormatsMissing = true;
	}

	// TODO : Check internal formats

	return (capsMissing || compressedFormatsMissing || internalFormatsMissing);
}

void glCapsViewer::slotClose(){
	close();
}

void glCapsViewer::slotUpload(){
	glCapsViewerHttp glchttp;

	if (!glchttp.checkReportPresent(core.description)) {
		// TODO : Depending on context type? (ES / GL)

		bool ok;
		QString text = QInputDialog::getText(this, tr("Submitter name"), tr("Submitter <i>(your name/nick, can be left empty)</i>:"), QLineEdit::Normal, "", &ok);
		core.submitter = text.toStdString();
		if (ok) {
			QApplication::setOverrideCursor(Qt::WaitCursor);
			string xml = core.reportToXml();
			string reply = glchttp.postReport(xml);
			QApplication::restoreOverrideCursor();
			if (reply == "res_uploaded") {
				QMessageBox::information(this, tr("Report submitted"), tr("Your report has been uploaded to the database!\n\nThanks for your contribution!"));
				updateReportState();
			}
			else {
				QMessageBox::warning(this, tr("Error"), tr("The report could not be uploaded!"));
			}
		}
	}
	else {
		// Check if report can be updated
		bool canUpdate = false;
		//// TODO : Test, put into separate function
		int reportId = glchttp.getReportId(core.description);
		//stringstream capsParam;
		//for (auto& capGroup : core.capgroups) {
		//	for (auto& cap : capGroup.capabilities) {
		//		if (cap.second != "n/a") {
		//			capsParam << cap.first << ",";
		//		}
		//	}
		//}
		//canUpdate = glchttp.checkReportCanUpdate(reportId, capsParam.str());
		canUpdate = canUpdateReport(reportId);
		if (canUpdate) {
			QMessageBox::StandardButton reply;
			reply = QMessageBox::question(this, "Report outdated", "There is a report for your device present in the database, but it is missing some capabilities.\n\nDo you want to update the report?", QMessageBox::Yes | QMessageBox::No);
			if (reply == QMessageBox::Yes) {
				// Submitter name to be stored in report update log
				bool ok;
				QString text = QInputDialog::getText(this, tr("Submitter name"), tr("Submitter <i>(your name/nick, can be left empty)</i>:"), QLineEdit::Normal, "", &ok);
				core.submitter = text.toStdString();
				// TODO : Error handling
				if (ok) {
					string xml = core.reportToXml();
					string httpReply = glchttp.postReportForUpdate(xml);
					QMessageBox::information(this, tr("httpReply"), QString::fromStdString(httpReply));
					updateReportState();
				}
			}
		}

		if (!canUpdate) {
			QMessageBox::StandardButton reply;
			reply = QMessageBox::question(this, "Device already present", "A report for your device and OpenGL version is aleady present in the database.\n\nDo you want to open the report in your browser?", QMessageBox::Yes | QMessageBox::No);
			if (reply == QMessageBox::Yes) {
				reportId = glchttp.getReportId(core.description);
				stringstream ss;
				ss << "http://delphigl.de/glcapsviewer/gl_generatereport.php?reportID=" << to_string(reportId);
				QDesktopServices::openUrl(QUrl(QString::fromStdString(ss.str())));
			}
		}
	}
}

void glCapsViewer::slotExportXml(){
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "glCapsViewer_Report.xml", tr("xml (*.xml)"));
	core.exportXml(fileName.toStdString());
}

void glCapsViewer::slotBrowseDatabase() {
	QString link = "http://openglcaps.delphigl.de";
	QDesktopServices::openUrl(QUrl(link));
}

void glCapsViewer::slotRefreshDatabase() {
	refreshDeviceList();
}

void glCapsViewer::slotAbout() {
	QString aboutText = "<p>OpenGL hardware capability viewer (glCapsViewer)<br/><br/>"
		"Copyright (c) 2011-2015 by Sascha Willems<br/><br/>"
		"This tool is <b>FREEWARE</b><br/><br/>"
		"For usage and distribution details refer to the readme<br/><br/>"
		"<a href='http://www.saschawillems.de'>http://www.saschawillems.de</a></p>";
	QMessageBox::about(this, tr("About the OpenGL hardware capability viewer"), aboutText);
}

void glCapsViewer::slotTabChanged(int index)
{
	if (index == 1) {
		refreshDeviceList();
	}
}

/// <summary>
///	Fetches a list of available report version for currently selected device
/// </summary>
void glCapsViewer::slotDatabaseDevicesItemChanged() {
	// TODO : Check if ui.listWidgetDatabaseDevices->currentItem()->text() is assigned
	glCapsViewerHttp glchttp;
	vector<reportInfo> reportList;
	
	ui.comboBoxDeviceVersions->clear();
	QVariant data = ui.listWidgetDatabaseDevices->currentItem()->data(Qt::UserRole);
	QString deviceName = data.toString();
	reportList = glchttp.fetchDeviceReports(deviceName.toStdString());

	for (auto& report : reportList) {
		stringstream ss;
		ss << report.version << " (" << report.operatingSystem << ")";
		ui.comboBoxDeviceVersions->addItem(QString::fromStdString(ss.str()), QVariant(report.reportId));
	}
}

/// <summary>
///	Fetches the report for the currently selected device and report version
/// Displays it in table form
/// </summary>
void glCapsViewer::slotDeviceVersionChanged(int index) {
	// TODO : Error checking
	if (index < 0) {
		return;
	}
	QApplication::setOverrideCursor(Qt::WaitCursor);
	ui.labelDatabaseDeviceExtensions->setText("Extensions");
	int reportId = ui.comboBoxDeviceVersions->itemData(index).toInt();
	glCapsViewerHttp glchttp;
	string reportXml = glchttp.fetchReport(reportId);
	// Generate simple report

	QTableWidget *table = ui.tableWidgetDatabaseDeviceReport;
	table->setRowCount(0);
	table->setColumnWidth(0, 250);
	table->horizontalHeader()->setStretchLastSection(true);
	table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
	table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	table->verticalHeader()->setDefaultSectionSize(24);
	table->verticalHeader()->setVisible(false);

	using namespace rapidxml;
	xml_document<> doc;
	xml_node<> * root_node;
	doc.parse<0>(&reportXml[0]);
	root_node = doc.first_node("report");

	// Implementation
	xml_node<> * implementation_node = root_node->first_node("implementation");
	for (xml_node<> * value_node = implementation_node->first_node(); value_node; value_node = value_node->next_sibling())
	{
		string node_name = value_node->name();
		string node_value = value_node->value();
		table->insertRow(table->rowCount());
		table->setItem(table->rowCount() - 1, 0, new QTableWidgetItem(QString::fromStdString(node_name)));
		if (node_value == "") {
			table->setItem(table->rowCount() - 1, 1, new QTableWidgetItem("n/a"));
			table->item(table->rowCount() - 1, 0)->setTextColor(QColor::fromRgb(100, 100, 100));
			table->item(table->rowCount() - 1, 1)->setTextColor(QColor::fromRgb(100, 100, 100));
		}
		else {
			table->setItem(table->rowCount() - 1, 1, new QTableWidgetItem(QString::fromStdString(node_value)));
		}
	}

	// Extensions
	ui.listWidgetDatabaseDeviceExtensions->clear();
	xml_node<> * extension_node = root_node->first_node("extensions");
	int extCount = 0;
	for (xml_node<> * value_node = extension_node->first_node(); value_node; value_node = value_node->next_sibling())
	{
		QListWidgetItem *deviceItem = new QListWidgetItem(QString::fromStdString(value_node->value()), ui.listWidgetDatabaseDeviceExtensions);
		deviceItem->setSizeHint(QSize(deviceItem->sizeHint().height(), 24));
		extCount++;
	}
	stringstream ss;
	ss << "Extensions (" << extCount << ")";
	ui.labelDatabaseDeviceExtensions->setText(QString::fromStdString(ss.str()));
	QApplication::restoreOverrideCursor();
}
