/*
*
* OpenGL hardware capability viewer and database
*
* Settings dialog
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

#include "settingsDialog.h"
#include "settings.h"

#include <QFormLayout>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QSettings>
#include <QDialogButtonBox>

namespace capsViewer {

	QLineEdit *createLineEdit(QString name)
	{
		QLineEdit* edit = new QLineEdit();
		edit->setObjectName(name);
		return edit;
	}

	QCheckBox *createCheckBox(QString name)
	{
		QCheckBox* checkbox = new QCheckBox();
		checkbox->setObjectName(name);
		return checkbox;
	}

	settingsDialog::settingsDialog(settings appSet, QWidget * parent, Qt::WindowFlags f)
	{
		appSettings = appSet;

		QFormLayout *formLayout = new QFormLayout;

		QLabel* labelCaption = new QLabel();
		labelCaption->setText("General settings");
		labelCaption->setStyleSheet("font: 75 11pt;");

		formLayout->addRow(labelCaption);
		formLayout->addRow(tr("Submitter:"), createLineEdit("editSubmitterName"));

		labelCaption = new QLabel();
		labelCaption->setText("Proxy Settings");
		labelCaption->setStyleSheet("font: 75 11pt;");
		formLayout->addRow(labelCaption);

		formLayout->addRow(tr("DNS Name / IP:"), createLineEdit("editProxyDns"));
		formLayout->addRow(tr("Port:"), createLineEdit("editProxyPort"));
		formLayout->addRow(tr("User name (if required):"), createLineEdit("editProxyUser"));
		formLayout->addRow(tr("Password (if required):"), createLineEdit("editProxyPassword"));
		formLayout->addRow(tr("Use proxy settings for upload"), createCheckBox("checkBoxUseProxy"));

		QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		connect(buttonBox, SIGNAL(accepted()), this, SLOT(slotAccept()));
		connect(buttonBox, SIGNAL(rejected()), this, SLOT(slotCancel()));

		formLayout->addWidget(buttonBox);

		setLayout(formLayout);
		setWindowTitle("Settings");

		// Restore settings
		QSettings settings("saschawillems", "glcapsviewer");
		QLineEdit* edit;
		findChild<QLineEdit*>("editSubmitterName", Qt::FindChildrenRecursively)->setText(settings.value("global/submitterName", "").toString());
		findChild<QLineEdit*>("editProxyDns", Qt::FindChildrenRecursively)->setText(settings.value("proxy/dns", "").toString());
		findChild<QLineEdit*>("editProxyPort", Qt::FindChildrenRecursively)->setText(settings.value("proxy/port", "").toString());
		findChild<QLineEdit*>("editProxyUser", Qt::FindChildrenRecursively)->setText(settings.value("proxy/user", "").toString());
		findChild<QLineEdit*>("editProxyPassword", Qt::FindChildrenRecursively)->setText(settings.value("proxy/password", "").toString());
		findChild<QCheckBox*>("checkBoxUseProxy", Qt::FindChildrenRecursively)->setChecked(settings.value("proxy/enabled", "false").toBool());
	}


	settingsDialog::~settingsDialog()
	{
	}

	void settingsDialog::slotAccept()
	{
		QSettings settings("saschawillems", "glcapsviewer");
		QLineEdit* edit;
		edit = this->findChild<QLineEdit*>("editSubmitterName", Qt::FindChildrenRecursively);
		settings.setValue("global/submitterName", edit->text());

		edit = this->findChild<QLineEdit*>("editProxyDns", Qt::FindChildrenRecursively);
		settings.setValue("proxy/dns", edit->text());
		edit = this->findChild<QLineEdit*>("editProxyPort", Qt::FindChildrenRecursively);
		settings.setValue("proxy/port", edit->text());
		edit = this->findChild<QLineEdit*>("editProxyUser", Qt::FindChildrenRecursively);
		settings.setValue("proxy/user", edit->text());
		edit = this->findChild<QLineEdit*>("editProxyPassword", Qt::FindChildrenRecursively);
		settings.setValue("proxy/password", edit->text());
		QCheckBox* checkbox = this->findChild<QCheckBox*>("checkBoxUseProxy", Qt::FindChildrenRecursively);
		settings.setValue("proxy/enabled", checkbox->isChecked());

		this->close();
	}

	void settingsDialog::slotCancel()
	{
		this->close();
	}

}