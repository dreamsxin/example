#include <iostream>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <cv.h>
#include <highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

  
int main(int argc, char** argv)  
{  
   
    CvCapture* capture;  
   
   
    if(1 == argc)  
    {  
        capture = cvCreateCameraCapture(0);  
    }  
    else  
    {  
        capture = cvCreateCameraCapture(atoi(argv[1]));  
    }  
  
    assert(NULL != capture);  
  
    Mat frame; 
    Mat frame_hash; 
    
    int iAvg;
    int arr[64], arr1[64]; 
    int iDiffNum;

	char tm[64]; 
	
	int delay=150000;
	     
    while(true)  
    {  
       
        frame = cvQueryFrame(capture);     
        resize(frame,frame,Size(200,150),0,0,CV_INTER_LINEAR);
               
        usleep(delay); 
    
        resize(frame, frame_hash, Size(8, 8), 0, 0, INTER_CUBIC);
    	cvtColor(frame_hash, frame_hash, CV_BGR2GRAY);
    	
    	iAvg = 0;
        for (int i = 0; i < 8; i++)
        {
            uchar* data1 = frame_hash.ptr<uchar>(i);
            int tmp = i * 8;
    
            for (int j = 0; j < 8; j++) 
            {
                int tmp1 = tmp + j;
                arr1[tmp1] = data1[j];
                iAvg += arr1[tmp1];
            }
        }

        iAvg /= 64;
  
        for (int i = 0; i < 64; i++) 
        {
            arr1[i] = ((arr1[i] >= iAvg) ? 1 : 0);
        }

  
	    iDiffNum=0;
        for (int i = 0; i < 64; i++)
        {
            if (arr[i] != arr1[i])
            {
                ++iDiffNum;
            }
            arr[i] = arr1[i] ; 
        }

       
        if(iDiffNum>8)
        { 
            time_t t = time(0); 
            strftime(tm, sizeof(tm), "%Y%m%d-%X",localtime(&t) ); 
     
            strcat(tm,".jpg");  
            imwrite(tm,frame); 
        }         
    }  
    
    return 0;  
}  
