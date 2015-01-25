#include <GL/glew.h>
#include <string>
#include <vector>

#pragma once
namespace capsViewer {

	using namespace std;

	enum internalFormatInfoType { infoTypeValue = 0, infoTypeSupport = 1 };

	class internalFormatInfoValue
	{
	public:
		internalFormatInfoType infoType;
		GLenum infoEnum;
		GLint infoValue;
		string infoString;
	};

	class internalFormatInfo
	{
	public:
		bool supported = false;
		GLenum textureFormat;
		vector<internalFormatInfoValue> formatInfoValues;
		internalFormatInfo(GLenum format);
		void addValueInfo(internalFormatInfoType type, GLenum infoenum, string infostring);
	};

}
