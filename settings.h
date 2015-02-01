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
		QString proxyDns;
		QString proxyPort;
		QString proxyUserName;
		QString proxyUserPassword;
		bool proxyEnabled;
		void restore();
	};

}

