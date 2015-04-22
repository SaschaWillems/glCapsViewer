/*
*
* OpenGL hardware capability viewer and database
*
* OpenGL internal format description
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

#include "internalFormatTarget.h"
#ifdef USEEGL
#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#else
#include <GL/glew.h>
#endif

namespace capsViewer {

	using namespace std;

	internalFormatTarget::internalFormatTarget(GLenum target, vector<GLint> compressedFormats)
	{
		this->target = target;
		// Base formats
#ifndef USEEGL
		GLint baseFormats[] = { GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL, GL_STENCIL_INDEX, GL_RED, GL_RG, GL_RGB, GL_RGBA, GL_DEPTH_COMPONENT32F };
		for (auto& baseFormat : baseFormats) {
			this->textureFormats.push_back(internalFormatInfo(baseFormat));
		}
		// Add detected compressed texture formats
		for (auto& compressedFormat : compressedFormats) {
			this->textureFormats.push_back(internalFormatInfo(compressedFormat));
		}
#endif
	}

	void internalFormatTarget::getInternalFormatInfo(bool internalformatquery2)
	{
		// TODO : List of internalFormats from xml
		// TODO : Additional formats
		//internalFormats[GL_RGBA8] = "GL_RGBA8";
		//internalFormats[GL_RGBA16] = "GL_RGBA16";
		//internalFormats[GL_DEPTH_COMPONENT16] = "GL_DEPTH_COMPONENT16";
#ifndef USEEGL

		for (auto& textureFormat : textureFormats) {

			// Check if internal format is supported
			GLint formatSupported;
			glGetInternalformativ(target, textureFormat.textureFormat, GL_INTERNALFORMAT_SUPPORTED, 1, &formatSupported);
			textureFormat.supported = (formatSupported == GL_TRUE);

			if (!textureFormat.supported) {
				continue;
			}

			// Add value info description
			// TODO : from xml? Easier to maitain and expand
			// GL_ARB_internalformat_query
			textureFormat.addValueInfo(infoTypeValue, GL_INTERNALFORMAT_PREFERRED, "GL_INTERNALFORMAT_PREFERRED");
			textureFormat.addValueInfo(infoTypeValue, GL_READ_PIXELS_FORMAT, "GL_READ_PIXELS_FORMAT");
			textureFormat.addValueInfo(infoTypeValue, GL_READ_PIXELS_TYPE, "GL_READ_PIXELS_TYPE");
			textureFormat.addValueInfo(infoTypeValue, GL_TEXTURE_IMAGE_FORMAT, "GL_TEXTURE_IMAGE_FORMAT");
			textureFormat.addValueInfo(infoTypeValue, GL_TEXTURE_IMAGE_TYPE, "GL_TEXTURE_IMAGE_TYPE");
			textureFormat.addValueInfo(infoTypeValue, GL_GET_TEXTURE_IMAGE_FORMAT, "GL_GET_TEXTURE_IMAGE_FORMAT");
			textureFormat.addValueInfo(infoTypeValue, GL_GET_TEXTURE_IMAGE_TYPE, "GL_GET_TEXTURE_IMAGE_TYPE");

			// Compressed format block sizes
			GLint compressedFormat;
			glGetInternalformativ(target, textureFormat.textureFormat, GL_TEXTURE_COMPRESSED, 1, &compressedFormat);
			if (compressedFormat == GL_TRUE) {
				textureFormat.addValueInfo(infoTypeValue, GL_TEXTURE_COMPRESSED_BLOCK_WIDTH, "GL_TEXTURE_COMPRESSED_BLOCK_WIDTH");
				textureFormat.addValueInfo(infoTypeValue, GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT, "GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT");
				textureFormat.addValueInfo(infoTypeValue, GL_TEXTURE_COMPRESSED_BLOCK_SIZE, "GL_TEXTURE_COMPRESSED_BLOCK_SIZE");
			}

			if (internalformatquery2) {
				textureFormat.addValueInfo(infoTypeValue, GL_INTERNALFORMAT_SUPPORTED, "GL_INTERNALFORMAT_SUPPORTED");
				textureFormat.addValueInfo(infoTypeValue, GL_TEXTURE_COMPRESSED, "GL_TEXTURE_COMPRESSED");
				textureFormat.addValueInfo(infoTypeValue, GL_MAX_WIDTH, "GL_MAX_WIDTH");
				textureFormat.addValueInfo(infoTypeValue, GL_MAX_HEIGHT, "GL_MAX_HEIGHT");
				textureFormat.addValueInfo(infoTypeValue, GL_MAX_DEPTH, "GL_MAX_DEPTH");
				textureFormat.addValueInfo(infoTypeValue, GL_FRAMEBUFFER_BLEND, "GL_FRAMEBUFFER_BLEND");
				textureFormat.addValueInfo(infoTypeValue, GL_READ_PIXELS, "GL_READ_PIXELS");
				textureFormat.addValueInfo(infoTypeValue, GL_MANUAL_GENERATE_MIPMAP, "GL_MANUAL_GENERATE_MIPMAP");
				textureFormat.addValueInfo(infoTypeValue, GL_AUTO_GENERATE_MIPMAP, "GL_AUTO_GENERATE_MIPMAP");
				textureFormat.addValueInfo(infoTypeValue, GL_FILTER, "GL_FILTER");
			}

			// Support
			textureFormat.addValueInfo(infoTypeSupport, GL_VERTEX_TEXTURE, "GL_VERTEX_TEXTURE");
			textureFormat.addValueInfo(infoTypeSupport, GL_TESS_CONTROL_TEXTURE, "GL_TESS_CONTROL_TEXTURE");
			textureFormat.addValueInfo(infoTypeSupport, GL_TESS_EVALUATION_TEXTURE, "GL_TESS_EVALUATION_TEXTURE");
			textureFormat.addValueInfo(infoTypeSupport, GL_GEOMETRY_TEXTURE, "GL_GEOMETRY_TEXTURE");
			textureFormat.addValueInfo(infoTypeSupport, GL_FRAGMENT_TEXTURE, "GL_FRAGMENT_TEXTURE");
			textureFormat.addValueInfo(infoTypeSupport, GL_COMPUTE_TEXTURE, "GL_COMPUTE_TEXTURE");
			textureFormat.addValueInfo(infoTypeSupport, GL_TEXTURE_SHADOW, "GL_TEXTURE_SHADOW");
			textureFormat.addValueInfo(infoTypeSupport, GL_TEXTURE_GATHER, "GL_TEXTURE_GATHER");
			textureFormat.addValueInfo(infoTypeSupport, GL_TEXTURE_GATHER_SHADOW, "GL_TEXTURE_GATHER_SHADOW");
			textureFormat.addValueInfo(infoTypeSupport, GL_SHADER_IMAGE_LOAD, "GL_SHADER_IMAGE_LOAD");
			textureFormat.addValueInfo(infoTypeSupport, GL_SHADER_IMAGE_STORE, "GL_SHADER_IMAGE_STORE");
			textureFormat.addValueInfo(infoTypeSupport, GL_SHADER_IMAGE_ATOMIC, "GL_SHADER_IMAGE_ATOMIC");

			// Fetch values
			for (auto& formatInfoValue : textureFormat.formatInfoValues) {
				glGetInternalformativ(target, textureFormat.textureFormat, formatInfoValue.infoEnum, 1, &formatInfoValue.infoValue);
				// TODO : Check glerror
			}

		}

		/*
		for (auto& internalFormat : internalFormats) {


		for (auto& pname : pnames) {
		GLint param;
		glGetInternalformativ(target, internalFormat.first, pname.first, 1, &param);

		if ((pname.second == "GL_MAX_WIDTH") || (pname.second == "GL_MAX_HEIGHT") || (pname.second == "GL_MAX_DEPTH")) {
		if (param == 0) {
		continue;
		}
		}

		QTreeWidgetItem *paramItem = new QTreeWidgetItem(formatItem);
		paramItem->setText(0, QString::fromStdString(pname.second));
		string enumString;
		enumString = core.getEnumName(param);

		// Switch some values
		if ((pname.first == GL_INTERNALFORMAT_SUPPORTED) || (pname.first == GL_TEXTURE_COMPRESSED))  {
		enumString = (param == 0) ? "GL_FALSE" : "GL_TRUE";
		}

		paramItem->setText(1, QString::fromStdString(enumString));
		paramItem->addChild(formatItem);
		colorInternalFormatItem(paramItem, 1);
		}

		QTreeWidgetItem *subItem;
		// Different supports
		subItem = new QTreeWidgetItem(formatItem);
		subItem->setText(0, "Shader support");
		for (auto& supportType : supportList) {
		GLint param;
		glGetInternalformativ(target, internalFormat.first, supportType.first, 1, &param);
		string enumString;
		enumString = core.getEnumName(param);
		QTreeWidgetItem *paramItem = new QTreeWidgetItem(subItem);
		paramItem->setText(0, QString::fromStdString(supportType.second));
		paramItem->setText(1, QString::fromStdString(enumString));
		paramItem->addChild(subItem);
		colorInternalFormatItem(paramItem, 1);
		}

		targetItem->sortChildren(0, Qt::AscendingOrder);

		}
		*/
#endif
	}
}

