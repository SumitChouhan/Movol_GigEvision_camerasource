
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

/* mmap() header file */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <errno.h>
#include <sys/mman.h>
#include <string.h>



int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	Worker worker ;

	int fd ;
	struct stat mystat;
	void *pmap;

	
//	fd = creat();
	fd = open("test.txt",0_RDWR);

if (fd == -1)
{

perror("open");

exit(1);
}


if(fstate(fd, &mystate) < 0) 

{

perror ("fstat");
close(fd);
exit(1);

}

pmap = mmap(0, mystat, st_size, PROT_READ | PROT_WRIE, MAP_SHARED, fd, 0);

if (pmap == MAP_FAILED) 

{

perror("mmap");close(fd);
exit(1);

}


strncpy(pmap, "That", 4);

close(fd);









	return app.exec();

}
















