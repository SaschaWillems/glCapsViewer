#pragma once
#include <QDialog>
#include "settings.h"

namespace capsViewer {

	class settingsDialog : public QDialog
	{
		Q_OBJECT
	private:
		settings appSettings;
	public:
		settingsDialog(settings appSet, QWidget * parent = 0, Qt::WindowFlags f = 0);
		~settingsDialog();
	private slots:
		void slotAccept();
		void slotCancel();
	};

}



