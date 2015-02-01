#include "settings.h"

namespace capsViewer {

	using namespace std;

	settings::settings()
	{
	}


	settings::~settings()
	{
	}

	void settings::restore()
	{
		QSettings settings("saschawillems", "glcapsviewer");
		submitterName = settings.value("global/submitterName", "").toString();
	}

}
