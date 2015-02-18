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
#include <GL/glew.h>
#ifdef _WIN32
	#include <GL/wglew.h>
#endif
#include <GLFW/glfw3.h>
#include <QtWidgets/QApplication>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileInfo>


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


	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	// Create basic window first to see what context types are supported

	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	capsViewer.window = glfwCreateWindow(640, 480, "glCapsViewer", NULL, NULL);
	if (!capsViewer.window)
	{
		QMessageBox::critical(&capsViewer, QObject::tr("glCapsViewer - Error"), QObject::tr("Could not create GLFW window (1)!"));
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
		QMessageBox::critical(&capsViewer, QObject::tr("glCapsViewer - Error"), QObject::tr("Could not create GLFW window (2)!"));
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

	capsViewer.generateReport();

	return a.exec();
}
