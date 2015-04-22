/*
*
* OpenGL hardware capability viewer and database
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

#include "glCapsViewer.h"
#include "glCapsViewerCore.h"
#include "glCapsViewerHttp.h"
#include <sstream>  
#include <fstream>
#include <QtWidgets/QApplication>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileInfo>
#ifdef USEEGL
	#include <EGL/egl.h>
	#include <EGL/eglplatform.h>
	#include <GLES2/gl2.h>
	#include <GLES2/gl2platform.h>
#else
	#include <GLFW/glfw3.h>
	#include <GL/glew.h>
	#ifdef _WIN32
		#include <GL/wglew.h>
	#endif
#endif

void glfw_error_callback(int error, const char* description)
{
#ifndef USEEGL
	stringstream errorStr;
	errorStr << "GLFW has reported an error\n\nError:\n" << description;
	switch (error)
	{
	case GLFW_VERSION_UNAVAILABLE :
		errorStr << "\n\nDetails:\nThe requested OpenGL or OpenGL ES version is not available on this machine.";
		break;
	case GLFW_FORMAT_UNAVAILABLE:
		errorStr << "\n\nDetails:\nThe requested pixel format is not supported.";
		break;
	case GLFW_API_UNAVAILABLE :
		errorStr << "\n\nDetails:\nThe requested client API is not supported. Make sure you have OpenGL drivers installed.";
		break;
	default:
		break;
	}
	errorStr << "\n\nGLFW version:\n" << glfwGetVersionString();
	QMessageBox::critical(NULL, "glCapsViewer - Error", QString::fromStdString(errorStr.str()));
#endif
}

HWND createWindow(int width, int height) {
	HINSTANCE hInstance = GetModuleHandle(NULL);
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = &DefWindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = 0;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"eglsamplewnd";
	wcex.hIconSm = NULL;
	//wcex.lpfnWndProc = wndProc;

	RegisterClassEx(&wcex);
	RECT rect = { 0, 0, width, height };
	int style = WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;
	AdjustWindowRect(&rect, style, FALSE);

	HWND hwnd = CreateWindow(L"eglsamplewnd", L"EGL OpenGL ES 2.0 example", style, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, GetModuleHandle(NULL), NULL);
	ShowWindow(hwnd, SW_SHOW);

	return hwnd;
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	glCapsViewer capsViewer;
	capsViewer.ui.labelReportPresent->setText("...");
	capsViewer.show();

	// Check for capability list xml
	QFileInfo capsXmlFile("./capslist.xml");
	if (!capsXmlFile.exists()) {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(&capsViewer, "File missing", "glCapsViewer could not locate the xml file containing the OpenGL capability list!\n\nDo you want to download it?", QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes) {
			glCapsViewerHttp glchttp;
			string capsXml = glchttp.fetchCapsList();
			std::ofstream destfile;
			destfile.open("./capslist.xml");
			destfile << capsXml;
			destfile.close();
			// TODO : Error checking
			QMessageBox::information(&capsViewer, "Download complete", "The xml file containing the OpenGL capability list has been downloaded!");
		}
		else {
			exit(-1);
		}
	}

	/*

	if (!glfwInit()) {
		QMessageBox::critical(&capsViewer, "Critical error", "Could not initialize glfw!");
		exit(EXIT_FAILURE);
	}

	// Create basic window first to see what context types are supported

	glfwSetErrorCallback(glfw_error_callback);
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	capsViewer.window = glfwCreateWindow(640, 480, "glCapsViewer", NULL, NULL);
	if (!capsViewer.window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(capsViewer.window);
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		QMessageBox::critical(&capsViewer, QObject::tr("glCapsViewer - Error"), QObject::tr("Could not initialize GLEW!"));
		return -1;
	}

	if (!capsViewer.contextTypeSelection()) {
		capsViewer.close();
		exit(-1);
	}

	//Create a window and create its OpenGL context
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	capsViewer.window = glfwCreateWindow(320, 240, "glCapsViewer", NULL, NULL);

	//If the window couldn't be created
	if (!capsViewer.window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(capsViewer.window);

	//Initialize GLEW
	glewExperimental = GL_TRUE;
	err = glewInit();

	//If GLEW hasn't initialized
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return -1;
	}
	*/

	HWND hwnd = (HWND)capsViewer.winId();
	HDC hdc = GetDC(hwnd);

	//HWND hwnd = createWindow(320, 240);
	//HDC hdc = GetDC(hwnd);

	EGLDisplay eglDisplay = eglGetDisplay(hdc);
	if (eglDisplay == EGL_NO_DISPLAY) {
		QMessageBox::critical(&capsViewer, QObject::tr("glCapsViewer - Error"), QObject::tr("Could not get valid egl display!"));
		return -1;
	}
	EGLint eglVersionMajor, eglVersionMinor;
	eglInitialize(eglDisplay, &eglVersionMajor, &eglVersionMinor);
	eglBindAPI(EGL_OPENGL_ES_API);

	EGLint configAttributes[] =
	{
		EGL_BUFFER_SIZE, 0,
		EGL_RED_SIZE, 5,
		EGL_GREEN_SIZE, 6,
		EGL_BLUE_SIZE, 5,
		EGL_ALPHA_SIZE, 0,
		EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
		EGL_CONFIG_CAVEAT, EGL_DONT_CARE,
		EGL_CONFIG_ID, EGL_DONT_CARE,
		EGL_DEPTH_SIZE, 24,
		EGL_LEVEL, 0,
		EGL_MAX_SWAP_INTERVAL, EGL_DONT_CARE,
		EGL_MIN_SWAP_INTERVAL, EGL_DONT_CARE,
		EGL_NATIVE_RENDERABLE, EGL_DONT_CARE,
		EGL_NATIVE_VISUAL_TYPE, EGL_DONT_CARE,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_SAMPLE_BUFFERS, 0,
		EGL_SAMPLES, 0,
		EGL_STENCIL_SIZE, 0,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_TRANSPARENT_TYPE, EGL_NONE,
		EGL_TRANSPARENT_RED_VALUE, EGL_DONT_CARE,
		EGL_TRANSPARENT_GREEN_VALUE, EGL_DONT_CARE,
		EGL_TRANSPARENT_BLUE_VALUE, EGL_DONT_CARE,
		EGL_NONE
	};

	EGLint surfaceAttributes[] = { EGL_NONE };
	EGLint contextAttributes[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };

	EGLint nrOfConfigs;
	EGLConfig windowConfig;
	eglChooseConfig(eglDisplay, configAttributes, &windowConfig, 1, &nrOfConfigs);
	EGLSurface eglSurface = eglCreateWindowSurface(eglDisplay, windowConfig, hwnd, surfaceAttributes);
	if (eglSurface == EGL_NO_SURFACE) {
		QMessageBox::critical(&capsViewer, QObject::tr("glCapsViewer - Error"), QObject::tr("Could not create egl surface!"));
	}

	EGLContext eglContext = eglCreateContext(eglDisplay, windowConfig, NULL, contextAttributes);
	if (eglContext == EGL_NO_CONTEXT) {
		QMessageBox::critical(&capsViewer, QObject::tr("glCapsViewer - Error"), QObject::tr("Could not create egl context!"));
		return 1;
	}

	EGLint attr[] = {
		EGL_BUFFER_SIZE, 16,
		EGL_RENDERABLE_TYPE,
		EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	int eglNumConfigs;
	EGLConfig *eglConfig = new EGLConfig;
	int elgNumConfig;
	if (!eglChooseConfig(eglDisplay, attr, eglConfig, sizeof(eglConfig), &eglNumConfigs)) {
		QMessageBox::critical(&capsViewer, QObject::tr("glCapsViewer - Error"), QObject::tr("Could not create egl configuration!"));
		return 1;
	}

	if (eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext) != EGL_TRUE) {
		QMessageBox::critical(&capsViewer, QObject::tr("glCapsViewer - Error"), QObject::tr("Could not make egl context current!"));
		return 1;
	}

	glClearColor(0.0, 0.0, 0.2, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

//	eglSwapBuffers(eglDisplay, eglSurface);

	//glewExperimental = GL_FALSE;
	//GLenum err = glewInit();

	////If GLEW hasn't initialized
	//if (err != GLEW_OK)
	//{
	//	stringstream ss;
	//	ss << "Fatal glew error : " << glewGetErrorString(err);
	//	QMessageBox::critical(&capsViewer, QObject::tr("glCapsViewer - Error"), QString::fromStdString(ss.str()));
	//	return -1;
	//}

	capsViewer.generateReport();

	return a.exec();
}
