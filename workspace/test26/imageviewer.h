#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H
/*
#include <QLCDNumber>
#include <QTimer>
#include <QTime>
#include <QObject>
#include <QTimer>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QtDebug>
#include <QPainter>
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/times.h>
#include <arpa/inet.h>
#include <iostream>
#include "./PvApi.h"

#define _STDCALL

#define FRAMESCOUNT 6

typedef struct 
{
    unsigned long   UID;
    tPvHandle       Handle;
    tPvFrame        Frames[FRAMESCOUNT];
    bool            Abort;
    unsigned long   Discarded; //Count of missing frames.

    pthread_t       ThHandle;
    
} tCamera;


class Worker 
//: public QObject
{
   // Q_OBJECT


public:
	int cnt;

	bool failed;
	int Index;
	unsigned long Last;

	//QLabel *label;// = new QLabel("test");
	//QPixmap buffer;	

	int r;

	// global camera data
	tPvErr errCode;
	tCamera GCamera;

  


    unsigned long FrameCompleted, FrameDropped, PacketReceived, PacketMissed;
    float FrameRate;
	unsigned long imageSize;
    tPvErr Err;

	// wait for camera to be plugged in
	void WaitForCamera()
	{
		std::cout<<"Waiting for a camera";
		while((PvCameraCount() == 0) && !GCamera.Abort)
		{
		//	std::cout<<".";
			usleep(250);
		}
	}

	// get the first camera found
	// return value: true == success, false == fail
	bool CameraGet()
	{
		tPvUint32 count,connected;
		tPvCameraInfoEx list;
  

		//regardless if connected > 1, we only set UID of first camera in list
		count = PvCameraListEx(&list,1,&connected, sizeof(tPvCameraInfoEx));
		if(count == 1)
		{
		    GCamera.UID = list.UniqueId;
		   // std::cout<<"Got camera : " << list.SerialNumber;
  

		    return true;

		}
		else
		{
			std::cout<<"CameraGet: Failed to find a camera";
  

			return false;
		}
	}


	// open camera, allocate memory
	// return value: true == success, false == fail
	bool CameraSetup()
	{
		tPvErr errCode;
		bool failed = false;
		unsigned long FrameSize = 0;
  

		//open camera
		if ((errCode = PvCameraOpen(GCamera.UID,ePvAccessMaster,&(GCamera.Handle))) != ePvErrSuccess)
		{  

			if (errCode == ePvErrAccessDenied)
			{     

				std::cout<<"PvCameraOpen returned ePvErrAccessDenied:";
				std::cout<<"Camera already open as Master, or camera wasn't properly closed and still waiting to HeartbeatTimeout.";	
			}
			else
				std::cout<<"PvCameraOpen err: " << errCode;
  

			return false;
		}

		// Calculate frame buffer size
		if((errCode = PvAttrUint32Get(GCamera.Handle,"TotalBytesPerFrame",&FrameSize)) != ePvErrSuccess)
		{
			std::cout<<"CameraSetup: Get TotalBytesPerFrame err: " << errCode;

			return false;
		}

		// allocate the frame buffers
		for(int i=0;i<FRAMESCOUNT && !failed;i++)
		{
			GCamera.Frames[i].ImageBuffer = new char[FrameSize];
			if(GCamera.Frames[i].ImageBuffer)
			{
				GCamera.Frames[i].ImageBufferSize = FrameSize;

  

			}
			else
			{
			//	std::cout<<"CameraSetup: Failed to allocate buffers.";
  

				failed = true;
			}
		}
  


		return !failed;
	}

	// setup and start streaming with PvCaptureWaitForFrameDone
	// Note this function doesn't return until Ctrl+C abort.
	// return value: true == success (code exits on Ctrl+C abort), false == fail (other failure)
	bool CameraStart()
	{
		tPvErr errCode;
		bool failed = false;
		int Index = 0;
		unsigned long Last = 0;
  

		std::cout<<"CameraStart() Start";
		// NOTE: This call sets camera PacketSize to largest sized test packet, up to 8228, that doesn't fail
		// on network card. Some MS VISTA network card drivers become unresponsive if test packet fails. 
		// Use PvUint32Set(handle, "PacketSize", MaxAllowablePacketSize) instead. See network card properties
		// for max allowable PacketSize/MTU/JumboFrameSize. 
		if((errCode = PvCaptureAdjustPacketSize(GCamera.Handle,8228)) != ePvErrSuccess)
		{
			std::cout<<"CameraStart: PvCaptureAdjustPacketSize err: " <<errCode;
  

			return false;
		}else
			std::cout<<"PvCaptureAdjustPacketSize : " << errCode;

		// start driver capture stream 
		if((errCode = PvCaptureStart(GCamera.Handle)) != ePvErrSuccess)
		{
			std::cout<<"CameraStart: PvCaptureStart err: " << errCode;
  

			return false;
		}else
			std::cout<<"PvCaptureStart : " << errCode;
  

	
		// queue frames. No FrameDoneCB callback function. 
		std::cout<<"queue frames : " << FRAMESCOUNT;
		for(int i=0;i<FRAMESCOUNT && !failed; i++)
		{             

			if((errCode = PvCaptureQueueFrame(GCamera.Handle,&(GCamera.Frames[i]),NULL)) != ePvErrSuccess)
			{  

				std::cout<<"CameraStart: PvCaptureQueueFrame err : " << errCode;
				failed = true;
			}else
				std::cout<<"PvCaptureQueueFrame : " << i;

		}

		if (failed)
		{
		//	std::cout<<"queue frames Failed!!";
			return false;
		}

// my_edit

if   ( PvAttrEnumSet(GCamera.Handle,"PixelFormat","Rgb24") != ePvErrSuccess) 
{

std::cout<<"problem in setting pixel format in camera ";

//return false;
}else 
std::cout<<"set the camera in rgb24  " << errCode;	

PvAttrEnumSet(GCamera.Handle,"BandwidthCtrlMode","SCPD");

PvAttrUint32Set(GCamera.Handle,"Height",720);

PvAttrUint32Set(GCamera.Handle,"Width",1280);

// my_edit
		
		// set the camera in freerun trigger, continuous mode, and start camera receiving triggers
		if((PvAttrEnumSet(GCamera.Handle,"FrameStartTriggerMode","Freerun") != ePvErrSuccess) ||
			(PvAttrEnumSet(GCamera.Handle,"AcquisitionMode","Continuous") != ePvErrSuccess) ||
			(PvCommandRun(GCamera.Handle,"AcquisitionStart") != ePvErrSuccess))
		{		
			std::cout<<"CameraStart: failed to set camera attributes";
  

			return false;
		}else
			
			{

std::cout<<"set the camera in freerun trigger, continuous mode, and start camera receiving triggers  " << errCode;	 


}		  



		//Loop until failure or CRTL+C abort
		while(!failed && !GCamera.Abort)
		{

			//Wait for [Index] frame of FRAMESCOUNT num frames
			//wait for frame to return from camera to host
			while((errCode = PvCaptureWaitForFrameDone(GCamera.Handle,&GCamera.Frames[Index],2000)) == ePvErrTimeout)
			{
  

				std::cout<<"Waiting for frame to return to host...";
			}

			if(errCode != ePvErrSuccess)
		    { 
				//likely camera unplugged
  

				std::cout<<"PvCaptureWaitForFrameDone err: " << errCode;
				failed = true;
			}
			else
			{
				if (GCamera.Frames[Index].Status != ePvErrSuccess)
				{
					std::cout<<"Frame: " << GCamera.Frames[Index].FrameCount << "Error: " << GCamera.Frames[Index].Status;	
  

				}
		
				// if frame hasn't been cancelled, requeue frame
				if(GCamera.Frames[Index].Status != ePvErrCancelled)
				{
					//Check for gaps in FrameCount due to image returning from camera with no frame queued.
					//This should never happen, as we use a multiple frame circular buffer. 
					if(Last + 1 != GCamera.Frames[Index].FrameCount)
					{
						
						//Note missing frame
						GCamera.Discarded++;
  

					}
   
					Last = GCamera.Frames[Index].FrameCount;
  

		            
					//Requeue [Index] frame of FRAMESCOUNT num frames
					if ((errCode = PvCaptureQueueFrame(GCamera.Handle,&GCamera.Frames[Index],NULL)) != ePvErrSuccess)
					{
						std::cout<<"PvCaptureQueueFrame err : " << errCode;
  

						failed = true;
					}
				
					//Increment [Index]
					Index++;
					if(Index==FRAMESCOUNT)
						Index = 0;                   
				}
				else
				{
					

					//Cancelled
					failed = true;
				}
			}

			if(!GCamera.Abort &&
					  ((Err = PvAttrUint32Get(GCamera.Handle,"StatFramesCompleted",&FrameCompleted)) == ePvErrSuccess) &&
					  ((Err = PvAttrUint32Get(GCamera.Handle,"StatFramesDropped",&FrameDropped)) == ePvErrSuccess) &&
					  ((Err = PvAttrUint32Get(GCamera.Handle,"StatPacketsMissed",&PacketMissed)) == ePvErrSuccess) &&
					  ((Err = PvAttrUint32Get(GCamera.Handle,"StatPacketsReceived",&PacketReceived)) == ePvErrSuccess) &&
					  ((Err = PvAttrFloat32Get(GCamera.Handle,"StatFrameRate",&FrameRate)) == ePvErrSuccess) &&
					  ((Err = PvAttrUint32Get(GCamera.Handle, "TotalBytesPerFrame", &imageSize)) == ePvErrSuccess) )
			{      


				std::cout<<"FrmCmp : "<<FrameCompleted << " FrmDrp :" <<FrameDropped << " PckCmp :" <<PacketReceived << " PckMss :" <<PacketMissed << " FrmRt :" <<FrameRate << " imageSize :" <<imageSize;
				//usleep(100);
			}else
			//	std::cout<<"failed!!!";

			std::cout<<"Frame Width :"<<GCamera.Frames[0].Width << "Frame Height :"<<GCamera.Frames[0].Height << "Format :"<<GCamera.Frames[0].Format;
            std::cout << "Frame Width :"<<GCamera.Frames[0].Width << "Frame Height :"<<GCamera.Frames[0].Height << "Format :"<<GCamera.Frames[0].Format;
  

		}

		return !failed;
	}

    Worker() 
	{
		
		Index = 0;
		failed = false;
		Last = 0;
		
		//image(1292,964,QImage::Format_Indexed8);

		if((errCode = PvInitialize()) != ePvErrSuccess)
		{
			std::cout<<"PvInitialize" << errCode ;
  

		}else
		{
			std::cout<<"PvInitialize" << errCode ;
  

		}
		//IMPORTANT: Initialize camera structure. See tPvFrame in PvApi.h for more info.
		memset(&GCamera,0,sizeof(tCamera));

		// set the CTRL+C handler
		//SetConsoleCtrlHandler(&CtrlCHandler, TRUE);


		// wait for a camera to be plugged
		WaitForCamera();

		if(CameraGet())
		{
			std::cout<<"CameraGet";
  

		}else
		{
			std::cout<<"CameraGet Failed!!!!";
		}
		if(CameraSetup())
		{
			std::cout<<"CameraSetup";
  

		}else
		{
			std::cout<<"CameraSetup Failed!!!!";
		}



		tPvErr errCode;

		std::cout<<"CameraStart() Start";
  

		// NOTE: This call sets camera PacketSize to largest sized test packet, up to 8228, that doesn't fail
		// on network card. Some MS VISTA network card drivers become unresponsive if test packet fails. 
		// Use PvUint32Set(handle, "PacketSize", MaxAllowablePacketSize) instead. See network card properties
		// for max allowable PacketSize/MTU/JumboFrameSize. 
		if((errCode = PvCaptureAdjustPacketSize(GCamera.Handle,8228)) != ePvErrSuccess)
		{  

			std::cout<<"CameraStart: PvCaptureAdjustPacketSize err: " <<errCode;
			//return false;
		}else
			std::cout<<"PvCaptureAdjustPacketSize : " << errCode;

		// start driver capture stream 
		if((errCode = PvCaptureStart(GCamera.Handle)) != ePvErrSuccess)
		{  

			std::cout<<"CameraStart: PvCaptureStart err: " << errCode;
			//return false;
		}else
			std::cout<<"PvCaptureStart : " << errCode;
	
		// queue frames. No FrameDoneCB callback function. 
		std::cout<<"queue frames : " << FRAMESCOUNT;
		for(int i=0;i<FRAMESCOUNT && !failed; i++)
		{            
 
			if((errCode = PvCaptureQueueFrame(GCamera.Handle,&(GCamera.Frames[i]),NULL)) != ePvErrSuccess)
			{
				std::cout<<"CameraStart: PvCaptureQueueFrame err : " << errCode;
  

				failed = true;
			}else
				std::cout<<"PvCaptureQueueFrame : " << i;
		}

		if (failed)
		{
			std::cout<<"queue frames Failed!!";
			//return false;
		}

// my_edit

if   ( PvAttrEnumSet(GCamera.Handle,"PixelFormat","Rgb24") != ePvErrSuccess) 
{

std::cout<<"problem in setting pixel format in camera ";
  


//return false;
}else 
std::cout<<"set the camera in rgb24  " << errCode;	

//PvAttrEnumSet(GCamera.Handle,"BandwidthCtrlMode","SCPD ");
  

PvAttrUint32Set(GCamera.Handle,"Height",720);

PvAttrUint32Set(GCamera.Handle,"Width",1280);
// my_edit

		// set the camera to 5 FPS, continuous mode, and start camera receiving triggers					
		if((PvAttrFloat32Set(GCamera.Handle,"FrameRate",15) != ePvErrSuccess) ||
			(PvAttrEnumSet(GCamera.Handle,"FrameStartTriggerMode","FixedRate") != ePvErrSuccess) ||
			(PvAttrEnumSet(GCamera.Handle,"AcquisitionMode","Continuous") != ePvErrSuccess)) 
//||
	//		(PvCommandRun(GCamera.Handle,"AcquisitionStart") != ePvErrSuccess))
		{		
			std::cout<<"CameraStart: failed to set camera attributes";
  

			//return false;
		}else
			std::cout<<"set the camera in freerun trigger, continuous mode, and start camera receiving triggers  " << errCode;	 
		
	//	label = new QLabel("test");
		
	//    connect(&timer, SIGNAL(timeout()), this, SLOT(doWork()));
	//    timer.start(1);
    }
 	
//private slots:

};

#endif
