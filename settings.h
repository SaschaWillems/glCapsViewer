#pragma once

#include <QSettings>
#include <QString>

namespace capsViewer {

	using namespace std;

	class settings
	{
	public:
		settings();
		~settings();
		QString submitterName;
		void restore();
	};

}

