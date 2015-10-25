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



