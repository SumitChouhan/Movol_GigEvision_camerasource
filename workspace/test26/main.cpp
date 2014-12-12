
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "imageviewer.h"
#include "./PvApi.h"

int main(int argc, char *argv[])
{


    Worker worker;
    bool failed;
	int Index = 0;
	unsigned long Last;
	//QLabel *label;// = new QLabel("test");
	//QPixmap buffer;	
	// global camera data
	tPvErr errCode;
	tCamera GCamera;
    unsigned long FrameCompleted, FrameDropped, PacketReceived, PacketMissed;
    float FrameRate;
	unsigned long imageSize;
    tPvErr Err;
    int i = 0;


if ( i ==0 )
{

PvCommandRun(GCamera.Handle,"AcquisitionStart");

i = i +1;
}

        while((errCode = PvCaptureWaitForFrameDone(GCamera.Handle,&GCamera.Frames[Index],2000)) == ePvErrTimeout)
        {
         std::cout<<"Waiting for frame to return to host...";

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
                    std::cout<<"[RSH][" <<__FILE__<< ":" <<__LINE__<< ":" <<__FUNCTION__<<"()] : ";
                    //std::cout<<"PvCaptureQueueFrame err : " << errCode;
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
            std::cout<<"Frame Width :"<<GCamera.Frames[0].Width << "Frame Height :"<<GCamera.Frames[0].Height << "Format :"<<GCamera.Frames[0].Format;
        }
        else
        {
            std::cout<<"failed!!!";
        }
}


    return (0);

}

