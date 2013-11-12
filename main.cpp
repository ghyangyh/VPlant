//#include "QDebugStream.h"
#include "sketchtreemainwin.h"
#include <QtGui/QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SketchTreeMainWin w;
	w.resize(1024,800);

	//QDebugStream qout(std::cout,w.getTextEdit());
	w.show();
	std::cout<<"******Variational Tree Modelling System Information Output******"<<std::endl;
	return a.exec();
}
