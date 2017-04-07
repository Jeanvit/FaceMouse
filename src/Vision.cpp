#include <stdio.h>
#include <iostream>
#include <Windows.h>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/core/ocl.hpp>

using namespace std;
using namespace cv;

/* Headers */
void faceDetection (Mat frame);
int cameraImage ();


/** Global variables */
String faceXML = "C:\\Mingw_build\\install\\etc\\haarcascades\\haarcascade_frontalface_alt.xml";
String eyeXML = "C:\\Mingw_build\\install\\etc\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";
string window = "Camera";
CascadeClassifier faceCascade;
CascadeClassifier eyesCascade;
int cameraNumber = 0;

/* main */
int main (int argc, const char** argv){

	cv::ocl::setUseOpenCL(true);
	cameraImage();
	return 1;
}


int cameraImage(){
	VideoCapture cap( cameraNumber);
	cout << "Res x " << cap.get(CV_CAP_PROP_FRAME_WIDTH);
	cout << "Res y " << cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	Mat frame;
	//==================
	// size of screen (primary monitor) without taskbar or desktop toolbars
	RECT DesktopRect,windowRect;
	   HWND hDesktop=::GetDesktopWindow();
	   // Gets the Desktop window rect or screen resolution in pixels
	   ::GetWindowRect(hDesktop, &DesktopRect);
	   cout <<  " X"<<DesktopRect.right<<  " Y"<< DesktopRect.bottom;




	//==================
	if(!faceCascade.load(faceXML)){
		printf("Error loading faceCascade XML\n");
		return -1;
	}
	if(!eyesCascade.load(eyeXML)){
		printf("Error loading eyeCascade XML\n");
		return -2;
	}
	int x=0;
	while( true ){
		//SetCursorPos(x++,900);
	    cap >> frame;
	    faceDetection(frame);
	    int c = waitKey(10);
	     if( (char)c == 'c' ) { break; }
	}
	return 1;
}

void faceDetection (Mat frame){
	vector<Rect> faces;
	Mat convertedFrame;
	cvtColor( frame, convertedFrame, CV_BGR2GRAY );
	equalizeHist( convertedFrame, convertedFrame );
	faceCascade.detectMultiScale( convertedFrame, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
	for (size_t i = 0 ; i < faces.size(); i ++ ){
		 Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
		 ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
		 Mat faceROI = convertedFrame( faces[i] );
		 vector<Rect> eyes;
		 eyesCascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );
		 for( size_t j = 0; j < eyes.size(); j++ )
		      {
		        Point center( faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5 );
		        int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
		        circle( frame, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );

				 cout <<  " Posicao face"<<faces[i].x <<  " Posicao olhos"<< eyes[j].x;
		      }
	}
	imshow(window, frame);

}
