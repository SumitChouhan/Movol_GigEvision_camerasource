
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include <QApplication>
#include <QMainWindow>
#include <qwidget.h>

#include <QObject>
#include <QTimer>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include "imageviewer.h"
#include "./PvApi.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	Worker worker ;
	return app.exec();

}
















