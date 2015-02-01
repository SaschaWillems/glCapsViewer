#pragma once
#include <QDialog>

namespace capsViewer {

	class settingsDialog : public QDialog
	{
		Q_OBJECT
	public:
		settingsDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);
		~settingsDialog();
	private slots:
		void slotAccept();
		void slotCancel();
	};

}



