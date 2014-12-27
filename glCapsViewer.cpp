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
}

glCapsViewer::~glCapsViewer()
{

}

/// <summary>
///	Reads implementation details, extensions and capabilities
///	and displays the report
/// </summary>
void glCapsViewer::generateReport()
{

	ui.treeWidget->clear();
	ui.treeWidgetExtensions->clear();

	core.readExtensions();
	core.readOsExtensions();
	core.readImplementation();
	core.readCapabilities();

	ui.labelDescription->setText(QString::fromStdString(core.description));

	glCapsViewerHttp glhttp;
	if (glhttp.checkReportPresent(core.description)) {
		// TODO : Check update state
		ui.labelReportPresent->setText("<font color='#00813e'>Device already present in database</font>");
		// TODO : Test
		int reportId = glhttp.getReportId(core.description);
		stringstream capsParam;
		for (auto& capGroup : core.capgroups) {
			for (auto& cap : capGroup.capabilities) {
				if (cap.second != "n/a") {
					capsParam << cap.first << ",";
				}
			}
		}
		if (glhttp.checkReportCanUpdate(reportId, capsParam.str())) {
			ui.labelReportPresent->setText("<font color='#0000FF'>Device already present in database, but can be updated with missing values!</font>");
		}
	}
	else {
		ui.labelReportPresent->setText("<font color='#bc0003'>Device not yet present in database</font>");
	}
	ui.labelReportPresent->setVisible(true);

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
		ss.str("");
		ss << "  " << s;
		QTreeWidgetItem *capItem = new QTreeWidgetItem(extItem);
		capItem->setText(0, QString::fromStdString(ss.str()));
		extItem->addChild(capItem);
	}

	extItem->setExpanded(true);

	// OS 
	cap.str("");
	cap << "OS specific extensions (" << core.osextensions.size() << ")";
	extItem = new QTreeWidgetItem(tree);
	extItem->setText(0, QString::fromStdString(cap.str()));

	for (auto& s : core.osextensions) {
		QTreeWidgetItem *capItem = new QTreeWidgetItem(extItem);
		capItem->setText(0, QString::fromStdString(s));
		extItem->addChild(capItem);
	}

	extItem->setExpanded(true);
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
	if (wglewIsSupported("WGL_EXT_create_context_es2_profile")) {
		core.availableContextTypes.push_back("OpenGL ES 2.0 context");
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
	// TODO : not finished yet
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
			}
			else {
				// TODO : Show error
			}
		}
	}
	else {
		// Check if report can be updated
		bool canUpdate = false;
		// TODO : Test
		int reportId = glchttp.getReportId(core.description);
		stringstream capsParam;
		for (auto& capGroup : core.capgroups) {
			for (auto& cap : capGroup.capabilities) {
				if (cap.second != "n/a") {
					capsParam << cap.first << ",";
				}
			}
		}
		canUpdate = glchttp.checkReportCanUpdate(reportId, capsParam.str());
		if (canUpdate) {
			QMessageBox::StandardButton reply;
			reply = QMessageBox::question(this, "Report outdated", "There is a report for your device present in the database, but it is missing some capabilities.\n\nDo you want to update the report?", QMessageBox::Yes | QMessageBox::No);
			if (reply == QMessageBox::Yes) {
				// Submitter name to be stored in report update log
				bool ok;
				QString text = QInputDialog::getText(this, tr("Submitter name"), tr("Submitter <i>(your name/nick, can be left empty)</i>:"), QLineEdit::Normal, "", &ok);
				core.submitter = text.toStdString();
				// TODO : WIP
				if (ok) {
					string xml = core.reportToXml();
					string httpReply = glchttp.postReportForUpdate(xml);
					QMessageBox::information(this, tr("httpReply"), QString::fromStdString(httpReply));
				}
			}
		}

		// 
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
