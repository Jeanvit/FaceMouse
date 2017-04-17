#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <math.h>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;
//===============================================================================================================================
/* Headers */
void faceDetection (Mat frame, VideoCapture auxCap);
int cameraImage (VideoCapture cap);
void moveMouse (int x, int y, int Xintensity, int Yintensity, int cameraXSize, int cameraYSize);
//===============================================================================================================================
/** Global variables */
String faceXML = "C:\\Mingw_build\\install\\etc\\haarcascades\\haarcascade_frontalface_alt.xml";
String eyeXML = "C:\\Mingw_build\\install\\etc\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";
string window = "Camera";
CascadeClassifier faceCascade;
CascadeClassifier eyesCascade;
int cameraNumber = 1;
//===============================================================================================================================
/* main */
int main (int argc, const char** argv){
	VideoCapture ca(cameraNumber);
	cout << "Camera x resolution: " << ca.get(CV_CAP_PROP_FRAME_WIDTH) << "\n";
	cout << "Camera y resolution: " << ca.get(CV_CAP_PROP_FRAME_HEIGHT);

	cameraImage(ca);
	return 1;
}
//===============================================================================================================================
int cameraImage(VideoCapture cap){
	/* Setting the camera*/
	Mat frame;
	if(!faceCascade.load(faceXML)){
		printf("Error loading faceCascade XML\n");
		return -1;
	}
	if(!eyesCascade.load(eyeXML)){
		printf("Error loading eyeCascade XML\n");
		return -2;
	}
	while( true ){
	    cap >> frame;
	    flip(frame,frame,1);
	    faceDetection(frame,cap);
	    int c = waitKey(10);
	    if( (char)c == 'c' ) { break; }
	}
	return 1;
}
//===============================================================================================================================
void faceDetection (Mat frame, VideoCapture auxCap){
	/* detecting faces*/
	vector<Rect> faces;
	Mat convertedFrame;
	cvtColor( frame, convertedFrame, CV_BGR2GRAY );
	equalizeHist( convertedFrame, convertedFrame );
	faceCascade.detectMultiScale( convertedFrame, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
	/* Drawing a circle on detected faces*/
	for (size_t i = 0 ; i < faces.size(); i ++ ){
		 Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
		 ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
		 Mat faceROI = convertedFrame( faces[i] );
		 vector<Rect> eyes;
		 eyesCascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );

		 moveMouse(faces[i].x,faces[i].y,2,4,auxCap.get(CV_CAP_PROP_FRAME_WIDTH),auxCap.get(CV_CAP_PROP_FRAME_HEIGHT));
		 /* for eyes detection */
		 /*for( size_t j = 0; j < eyes.size(); j++ )
		      {
		        Point center( faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5 );
		        int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
		        circle( frame, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );


		      }*/
	}
	imshow(window, frame);
}
//===============================================================================================================================
void moveMouse (int x, int y, int Xintensity, int Yintensity, int cameraXSize, int cameraYSize){
	/* Moving the mouse*/
	// size of screen (primary monitor) without taskbar or desktop toolbars
	RECT DesktopRect;
	int positionX,positionY;
	HWND hDesktop=::GetDesktopWindow();

	// Gets the Desktop window rect or screen resolution in pixels
	::GetWindowRect(hDesktop, &DesktopRect);

	positionX=(((float)x)/cameraXSize)*DesktopRect.right;
	positionY=(((float)y)/cameraYSize)* DesktopRect.bottom;
	SetCursorPos(positionX*Xintensity,positionY*Yintensity);
}
//===============================================================================================================================
