#include "settingsDialog.h"
#include "settings.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QSettings>

namespace capsViewer {

	QHBoxLayout* createLabeledEdit(QString caption, QString name) {
		QHBoxLayout* hLayout = new QHBoxLayout();

		QLabel* lbl = new QLabel();
		lbl->setText(caption);
		hLayout->addWidget(lbl);

		QLineEdit* edit = new QLineEdit();
		edit->setObjectName(name);
		hLayout->addWidget(edit);

		return hLayout;
	}

	QHBoxLayout* createCheckBox(QString caption, QString name) {
		QHBoxLayout* hLayout = new QHBoxLayout();

		QCheckBox* chkbox = new QCheckBox();
		chkbox->setText(caption);
		chkbox->setObjectName(name);
		hLayout->addWidget(chkbox);

		return hLayout;
	}

	settingsDialog::settingsDialog(settings appSet, QWidget * parent, Qt::WindowFlags f)
	{
		appSettings = appSet;

		QGridLayout* mainGrid = new QGridLayout;
		QVBoxLayout* topLayout = new QVBoxLayout;
		//topLayout->addWidget(new QTextEdit);

		QLabel* lbl = new QLabel();
		lbl->setText("Global options");
		lbl->setStyleSheet("font: 75 11pt;");
		topLayout->addWidget(lbl);
		mainGrid->addLayout(topLayout, 0, 0);

		// Submitter name
		mainGrid->addLayout(createLabeledEdit("Submitter name:", "editSubmitterName"), 1, 0);

		topLayout = new QVBoxLayout;
		lbl = new QLabel();
		lbl->setText("Proxy Settings");
		lbl->setStyleSheet("font: 75 11pt;");
		topLayout->addWidget(lbl);
		mainGrid->addLayout(topLayout, 2, 0);

		mainGrid->addLayout(createLabeledEdit("DNS Name / IP:", "editProxyDns"), 3, 0);
		mainGrid->addLayout(createLabeledEdit("Port:", "editProxyPort"), 4, 0);
		mainGrid->addLayout(createLabeledEdit("User name (if required):", "editProxyUser"), 5, 0);
		mainGrid->addLayout(createLabeledEdit("Password (if required):", "editProxyPassword"), 6, 0);

		mainGrid->addLayout(createCheckBox("Use proxy settings for upload", "checkBoxUseProxy"), 7, 0);
		
		QHBoxLayout* hLayout = new QHBoxLayout;
		QPushButton* btn;
		for (int i = 0; i<2; i++) {
			if (i == 0) {
				btn = new QPushButton("OK");
				connect(btn, SIGNAL(clicked()), this, SLOT(slotAccept()));
			}
			else if (i == 1) {
				btn = new QPushButton("Cancel");
				connect(btn, SIGNAL(clicked()), this, SLOT(slotCancel()));
			}
			hLayout->addWidget(btn);
		}
		mainGrid->addLayout(hLayout, 8, 0);
		setLayout(mainGrid);

		this->setWindowTitle("Settings");

		// Restore settings
		QSettings settings("saschawillems", "glcapsviewer");
		QLineEdit* edit;
		this->findChild<QLineEdit*>("editSubmitterName", Qt::FindChildrenRecursively)->setText(settings.value("global/submitterName", "").toString());
		this->findChild<QLineEdit*>("editProxyDns", Qt::FindChildrenRecursively)->setText(settings.value("proxy/dns", "").toString());
		this->findChild<QLineEdit*>("editProxyPort", Qt::FindChildrenRecursively)->setText(settings.value("proxy/port", "").toString());
		this->findChild<QLineEdit*>("editProxyUser", Qt::FindChildrenRecursively)->setText(settings.value("proxy/user", "").toString());
		this->findChild<QLineEdit*>("editProxyPassword", Qt::FindChildrenRecursively)->setText(settings.value("proxy/password", "").toString());
		this->findChild<QCheckBox*>("checkBoxUseProxy", Qt::FindChildrenRecursively)->setChecked(settings.value("proxy/enabled", "false").toBool());
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