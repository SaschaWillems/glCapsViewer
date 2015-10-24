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
#include "settingsDialog.h"
#include "settings.h"
#include "internalFormatTarget.h"
#include <GL/glew.h>
#ifdef _WIN32
	#include <GL/wglew.h>
#endif
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
#include <QXmlStreamReader>
#ifdef __linux__
	#include <GL/glxew.h>
#endif

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
	connect(ui.actionSettings, SIGNAL(triggered()), this, SLOT(slotSettings()));
	connect(ui.actionUpload, SIGNAL(triggered()), this, SLOT(slotUpload()));
	connect(ui.actionDevice, SIGNAL(triggered()), this, SLOT(slotShowDeviceOnline()));
	connect(ui.pushButtonRefreshDataBase, SIGNAL(released()), this, SLOT(slotRefreshDatabase()));
	connect(ui.listWidgetDatabaseDevices, SIGNAL(itemSelectionChanged()), this, SLOT(slotDatabaseDevicesItemChanged()));
	connect(ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotTabChanged(int)));
	connect(ui.comboBoxDeviceVersions, SIGNAL(currentIndexChanged(int)), this, SLOT(slotDeviceVersionChanged(int)));

	ui.tableWidgetDatabaseDeviceReport->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
	ui.tableWidgetDatabaseDeviceReport->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

	// Extension tree model and filter proxy
	ui.treeViewExtensions->setModel(&extensionFilterProxy);
	extensionFilterProxy.setSourceModel(&extensionTreeModel);
	connect(ui.lineEditeExtensions, SIGNAL(textChanged(QString)), this, SLOT(slotFilterExtensions(QString)));

	// Implementation tree model and filter proxy
	ui.treeViewImplementation->setModel(&implementationFilterProxy);
	implementationFilterProxy.setSourceModel(&implementationTreeModel);
	connect(ui.lineEditImplementation, SIGNAL(textChanged(QString)), this, SLOT(slotFilterImplementation(QString)));

	appSettings.restore();

	if (!core.loadEnumList()) 
	{
		QMessageBox::warning(this, tr("Error"), tr("Could not load enum list!"));
		// TODO : Instead of exiting, download from server and try to load again
		exit(EXIT_FAILURE);
	}

	#ifdef DEVDATABASE
		stringstream newTitle;
		newTitle << this->windowTitle().toStdString() << " - ! Connected to development database !";
		this->setWindowTitle(QString::fromStdString(newTitle.str()));
	#endif
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

	ui.labelReportPresent->setText("<font color='#000000'>Connecting to database...</font>");
	ui.labelReportPresent->setVisible(true);
	ui.actionDevice->setEnabled(false);

	QApplication::setOverrideCursor(Qt::WaitCursor);
	if (!glhttp.checkServerConnection()) {
		ui.labelReportPresent->setText("<font color='#FF0000'>Could not connect to the OpenGL hardware database!\n\nPlease check your internet connection and proxy settings!</font>");
		ui.labelReportPresent->setVisible(true);
		QApplication::restoreOverrideCursor();
		return;
	}

	if (glhttp.checkReportPresent(core.description)) {
		ui.actionDevice->setEnabled(true);
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
	QApplication::restoreOverrideCursor();
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

void glCapsViewer::displayCapabilities()
{
	QStandardItem *rootItem;
	QStandardItem *parentItem;

	// Implementation detail
	rootItem = implementationTreeModel.invisibleRootItem();
	QList<QStandardItem *> captionItems;
	captionItems << new QStandardItem("Key");
	captionItems << new QStandardItem("Value");
	rootItem->appendRow(captionItems);

	parentItem = new QStandardItem("Implementation details");
	rootItem->appendRow(parentItem);

	for (auto& s : core.implementation)
	{
		QList<QStandardItem *> rowItems;
		rowItems << new QStandardItem(QString::fromStdString(s.first));
		rowItems << new QStandardItem(QString::fromStdString(s.second));
		parentItem->appendRow(rowItems);

	}

	// Capabilities
	for (auto& group : core.capgroups)
	{
		if (!group.visible) 
			continue;
		QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(group.name));
		item->setTextColor(QColor::fromRgb(0, 0, 255));

		QList<QStandardItem *> rowItems;
		rowItems << new QStandardItem(QString::fromStdString(group.name));

		if (group.supported) 
		{
			rowItems << new QStandardItem(QString::number(group.capabilities.size()));
			for (auto& cap : group.capabilities)
			{
				QList<QStandardItem *> capsItems;
				capsItems << new QStandardItem(QString::fromStdString(cap.first));
				capsItems << new QStandardItem(QString::fromStdString(cap.second));
				if (cap.second == "n/a")
				{
					capsItems[0]->setForeground(QColor::fromRgb(100, 100, 100));
					capsItems[1]->setForeground(Qt::red);
				}
				rowItems[0]->appendRow(capsItems);
			}
		}
		else 
		{
			rowItems << new QStandardItem("not available");
			rowItems[1]->setForeground(Qt::red);
		}

		rootItem->appendRow(rowItems);
	}

	ui.treeViewImplementation->expandAll();
	ui.treeViewImplementation->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}


void glCapsViewer::displayExtensions()
{
	QStandardItem *rootItem = extensionTreeModel.invisibleRootItem();
	QStandardItem *parentItem;

	parentItem = new QStandardItem("OpenGL extensions (" + QString::number(core.extensions.size()) + ")");
	rootItem->appendRow(parentItem);
	for (auto& s : core.extensions)
	{
		if (s == "") continue;
		parentItem->appendRow(new QStandardItem(QString::fromStdString(s)));
	}

	parentItem = new QStandardItem("OS specific extensions (" + QString::number(core.osextensions.size()) + ")");
	rootItem->appendRow(parentItem);
	for (auto& s : core.osextensions)
	{
		if (s == "") continue;
		parentItem->appendRow(new QStandardItem(QString::fromStdString(s)));
	}
	ui.treeViewExtensions->expandAll();
}

void glCapsViewer::displayInternalFormatInfo()
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

	for (auto& target : core.internalFormatTargets) {

		QTreeWidgetItem *targetItem = new QTreeWidgetItem(tree);
		targetItem->setText(0, QString::fromStdString(core.getEnumName(target.target)));

		for (auto& textureFormat : target.textureFormats) {
			QTreeWidgetItem *formatItem = new QTreeWidgetItem(targetItem);
			formatItem->setText(0, QString::fromStdString(core.getEnumName(textureFormat.textureFormat)));
			formatItem->addChild(targetItem);

			if (!textureFormat.supported) {
				formatItem->setText(1, "not supported");
				formatItem->setTextColor(1, QColor::fromRgb(100, 100, 100));
				formatItem->setTextColor(1, QColor::fromRgb(100, 100, 100));
				continue;
			}

			for (auto& formatInfoValue : textureFormat.formatInfoValues) {
				if (formatInfoValue.infoType == capsViewer::infoTypeValue) {
					QTreeWidgetItem *paramItem = new QTreeWidgetItem(formatItem);
					paramItem->setText(0, QString::fromStdString(formatInfoValue.infoString));
					string enumString;
					enumString = core.getEnumName(formatInfoValue.infoValue);
					// Switch some values
					if ((formatInfoValue.infoEnum == GL_INTERNALFORMAT_SUPPORTED) || (formatInfoValue.infoEnum == GL_TEXTURE_COMPRESSED))  {
						enumString = (formatInfoValue.infoValue == 0) ? "GL_FALSE" : "GL_TRUE";
					}
					paramItem->setText(1, QString::fromStdString(enumString));
					paramItem->addChild(formatItem);
					colorInternalFormatItem(paramItem, 1);
				}
			}

			QTreeWidgetItem *supportItem = new QTreeWidgetItem(formatItem);
			supportItem->setText(0, "Shader support");

			for (auto& formatInfoValue : textureFormat.formatInfoValues) {
				if (formatInfoValue.infoType == capsViewer::infoTypeSupport) {
					QTreeWidgetItem *valueItem = new QTreeWidgetItem(supportItem);
					valueItem->setText(0, QString::fromStdString(formatInfoValue.infoString));
					string enumString;
					enumString = core.getEnumName(formatInfoValue.infoValue);
					valueItem->setText(1, QString::fromStdString(enumString));
					valueItem->addChild(formatItem);
					colorInternalFormatItem(valueItem, 1);
				}
			}
		}

		targetItem->sortChildren(0, Qt::AscendingOrder);
	}
}

/// <summary>
///	Reads implementation details, extensions and capabilities
///	and displays the report
/// </summary>
void glCapsViewer::generateReport()
{
	ui.listWidgetCompressedFormats->clear();

	extensionTreeModel.clear();
	implementationTreeModel.clear();

	core.readExtensions();
	core.readOsExtensions();
	core.readImplementation();
	core.readCapabilities();
	core.readCompressedFormats();
	if (core.extensionSupported("GL_ARB_internalformat_query")) 
		core.readInternalFormats();

	ui.labelDescription->setText(QString::fromStdString(core.description));

	updateReportState();

	stringstream ss;

	displayCapabilities();
	displayExtensions();

	// Supported Compressed texture formats
	for (auto& compressedFormat : core.compressedFormats) 
	{
		string formatString = core.getEnumName(compressedFormat);
		QListWidgetItem *formatItem = new QListWidgetItem(QString::fromStdString(formatString), ui.listWidgetCompressedFormats);
		formatItem->setSizeHint(QSize(formatItem->sizeHint().height(), 24));
	}

	// TODO : Disabled for testing
//	displayInternalFormatInfo();

	// Tab captions
	stringstream tabText;
	tabText << "Extensions (" << core.extensions.size() + core.osextensions.size() << ")";
	ui.tabWidgetDevice->setTabText(1, QString::fromStdString(tabText.str()));
	tabText.str("");
	tabText << "Compressed tex. formats (" << core.compressedFormats.size() << ")";
	ui.tabWidgetDevice->setTabText(2, QString::fromStdString(tabText.str()));
}

bool glCapsViewer::contextTypeSelection() 
{
	// Get available context types
	core.availableContextTypes.clear();
	core.availableContextTypes.push_back("OpenGL default");

#ifdef _WIN32	
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
#endif
#ifdef __linux__
	// Core context
	if (glxewIsSupported("GLX_ARB_create_context_profile")) {
		core.availableContextTypes.push_back("OpenGL core context");
	}
	// OpenGL ES context
	// GLES 1.0
	if (glxewIsSupported("GLX_EXT_create_context_es_profile")) {
		core.availableContextTypes.push_back("OpenGL ES 1.0 context");
	}
	// GLES 2.0
	if (glxewIsSupported("GLX_EXT_create_context_es2_profile")) {
		core.availableContextTypes.push_back("OpenGL ES 2.0 context");
		// GLES 3.0 (superset of ES 2.0)
		if (glewIsSupported("GL_ARB_ES3_compatibility")) {
			core.availableContextTypes.push_back("OpenGL ES 3.0 context");
		}
	}
#endif
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

	bool capsMissing = false;
	bool compressedFormatsMissing = false;
	bool internalFormatsMissing = false;

	// Gather client-side available caps
	vector<string> capsList;
	for (auto& capsGroup : core.capgroups) {
		for (auto& cap : capsGroup.capabilities) {
			if (cap.second != "n/a") {
				capsList.push_back(cap.first);
			}
		}
	}

	QXmlStreamReader xmlReader(&reportXml[0]);

	vector<string> capsMissingDatabase;
	vector<int> compressedFormatsDatabase;
	while (!xmlReader.atEnd()) {
		if ((xmlReader.isStartElement()) && (xmlReader.name() == "implementation")) {
			while (!xmlReader.atEnd()) {
				xmlReader.readNext();
				if (xmlReader.name() == "implementation") {
					break;
				}
				QString nodeName = xmlReader.name().toString();
				QString nodeValue = xmlReader.readElementText();
				if (nodeValue == "") {
					capsMissingDatabase.push_back(nodeName.toStdString());
				}

			}
		}
		if ((xmlReader.isStartElement()) && (xmlReader.name() == "format")) {
			compressedFormatsDatabase.push_back(atoi(xmlReader.readElementText().toStdString().c_str()));
		}
		xmlReader.readNext();
	}

	// Check for missing caps
	for (auto& capMissingDatabase : capsMissingDatabase) {
		if (std::find(capsList.begin(), capsList.end(), capMissingDatabase) != capsList.end()) {
			capsMissing = true;
			break;
		}
	}

	// Check for missing compressed formats
	for (auto& compressedFormatClient : core.compressedFormats) {
		if (std::find(compressedFormatsDatabase.begin(), compressedFormatsDatabase.end(), compressedFormatClient) == compressedFormatsDatabase.end()) {
			compressedFormatsMissing = true;
			break;
		}
	}

	// TODO : Check internal formats

	return (capsMissing || compressedFormatsMissing || internalFormatsMissing);
}

void glCapsViewer::slotClose(){
	close();
}

void glCapsViewer::slotUpload(){
	glCapsViewerHttp glchttp;

	if (!glchttp.checkServerConnection()) {
		QMessageBox::warning(this, tr("Error"), tr("Could not connect to the OpenGL hardware database!\n\nPlease check your internet connection and proxy settings!"));
		return;
	}

	if (!glchttp.checkReportPresent(core.description)) {
		// TODO : Depending on context type? (ES / GL)
		bool ok;
		QString text = QInputDialog::getText(this, tr("Submitter name"), tr("Submitter <i>(your name/nick, can be left empty)</i>:"), QLineEdit::Normal, appSettings.submitterName, &ok);
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
		int reportId = glchttp.getReportId(core.description);
		canUpdate = canUpdateReport(reportId);
		if (canUpdate) {
			QMessageBox::StandardButton reply;
			reply = QMessageBox::question(this, "Report outdated", "There is a report for your device present in the database, but it is missing some capabilities.\n\nDo you want to update the report?", QMessageBox::Yes | QMessageBox::No);
			if (reply == QMessageBox::Yes) {
				// Submitter name to be stored in report update log
				bool ok;
				QString text = QInputDialog::getText(this, tr("Submitter name"), tr("Submitter <i>(your name/nick, can be left empty)</i>:"), QLineEdit::Normal, appSettings.submitterName, &ok);
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
				ss << glCapsViewerHttp::getBaseUrl() << "gl_generatereport.php?reportID=" << to_string(reportId);
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
	QString link = QString::fromStdString(glCapsViewerHttp::getBaseUrl());
	QDesktopServices::openUrl(QUrl(link));
}

void glCapsViewer::slotShowDeviceOnline() {
	glCapsViewerHttp glchttp;
	int reportId = glchttp.getReportId(core.description);
	stringstream ss;
	ss << glchttp.getBaseUrl() << "gl_generatereport.php?reportID=" << to_string(reportId);
	QDesktopServices::openUrl(QUrl(QString::fromStdString(ss.str())));
}

void glCapsViewer::slotRefreshDatabase() {
	refreshDeviceList();
}

void glCapsViewer::slotAbout() {
	stringstream aboutText;
	aboutText << "<p>OpenGL hardware capability viewer (glCapsViewer)<br/><br/>"
		"Copyright (c) 2011-2015 by Sascha Willems<br/><br/>"
		"This tool is <b>FREEWARE</b><br/><br/>"
		"For usage and distribution details refer to the readme<br/><br/>"
		"<a href='http://www.gpuinfo.org'>www.gpuinfo.org</a><br><br>"
		"<a href='http://www.saschawillems.de'>www.saschawillems.de</a><br><br>";
	aboutText << "GLFW : " << glfwGetVersionString() << "<br>";
	aboutText << "GLEW : " << glewGetString(GLEW_VERSION);
	aboutText << "</p>";
	QMessageBox::about(this, tr("About the OpenGL hardware capability viewer"), QString::fromStdString(aboutText.str()));
}

void glCapsViewer::slotSettings() {
	capsViewer::settingsDialog dialog(appSettings);
	dialog.setModal(true);
	dialog.exec();
	appSettings.restore();
}

void glCapsViewer::slotTabChanged(int index)
{
	if (index == 1) {
		refreshDeviceList();
	}
}

void glCapsViewer::slotFilterExtensions(QString text)
{
	QRegExp regExp(text, Qt::CaseInsensitive, QRegExp::RegExp);
	extensionFilterProxy.setFilterRegExp(regExp);
}

void glCapsViewer::slotFilterImplementation(QString text)
{
	QRegExp regExp(text, Qt::CaseInsensitive, QRegExp::RegExp);
	implementationFilterProxy.setFilterRegExp(regExp);
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

	QXmlStreamReader xmlReader(&reportXml[0]);

	while (!xmlReader.atEnd()) {

		if ((xmlReader.isStartElement()) && (xmlReader.name() == "implementation")) {
			while (!xmlReader.atEnd()) {
				xmlReader.readNext();
				if (xmlReader.name() == "implementation") {
					break;
				}
				table->insertRow(table->rowCount());
				table->setItem(table->rowCount() - 1, 0, new QTableWidgetItem(xmlReader.name().toString()));
				QString nodeValue = xmlReader.readElementText();
				if (nodeValue == "") {
					table->setItem(table->rowCount() - 1, 1, new QTableWidgetItem("n/a"));
					table->item(table->rowCount() - 1, 0)->setTextColor(QColor::fromRgb(100, 100, 100));
					table->item(table->rowCount() - 1, 1)->setTextColor(QColor::fromRgb(100, 100, 100));
				}
				else {
					table->setItem(table->rowCount() - 1, 1, new QTableWidgetItem(nodeValue));
				}
			}
		}

		int extCount = 0;
		if ((xmlReader.isStartElement()) && (xmlReader.name() == "extensions")) {
			while (!xmlReader.atEnd()) {
				xmlReader.readNext();
				if (xmlReader.name() != "extension") {
					break;
				}
				QListWidgetItem *deviceItem = new QListWidgetItem(xmlReader.readElementText(), ui.listWidgetDatabaseDeviceExtensions);
				deviceItem->setSizeHint(QSize(deviceItem->sizeHint().height(), 24));
				extCount++;
			}
			stringstream ss;
			ss << "Extensions (" << extCount << ")";
			ui.labelDatabaseDeviceExtensions->setText(QString::fromStdString(ss.str()));
		}

		xmlReader.readNext();
	}

	QApplication::restoreOverrideCursor();
}
