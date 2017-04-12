#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <math.h>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/core/ocl.hpp>

using namespace std;
using namespace cv;

/* Headers */
void faceDetection (Mat frame);
int cameraImage ();
void moveMouse (int x, int y, int totalPixels);

/** Global variables */
String faceXML = "C:\\Mingw_build\\install\\etc\\haarcascades\\haarcascade_frontalface_alt.xml";
String eyeXML = "C:\\Mingw_build\\install\\etc\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";
string window = "Camera";
CascadeClassifier faceCascade;
CascadeClassifier eyesCascade;
int cameraNumber = 0;

class Mouse{
private:
	int x,y;
	public:
	    void set_XYposition (int,int);
	    int getX();
	    int getY();
	    Mouse();
	    ~Mouse();
};

Mouse::Mouse(){
	x=0;
	y=0;
}

Mouse::~Mouse(){
}

void Mouse::set_XYposition(int positionX, int positionY){
	SetCursorPos(positionX,positionY);
}

int Mouse::getX(){
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	return (int)cursorPos.x;
}

int Mouse::getY(){
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	return (int)cursorPos.y;
}

void moveTo(int x, int y){
	SetCursorPos(x,y);
}
void goToTopRight (){
	RECT DesktopRect;
	HWND hDesktop=::GetDesktopWindow();
	::GetWindowRect(hDesktop, &DesktopRect);
	SetCursorPos(DesktopRect.right,0);
}

void goToTopLeft (){
	RECT DesktopRect;
	HWND hDesktop=::GetDesktopWindow();
	::GetWindowRect(hDesktop, &DesktopRect);
	SetCursorPos(0,0);
}

void goToBottonpRight (){
	RECT DesktopRect;
	HWND hDesktop=::GetDesktopWindow();
	::GetWindowRect(hDesktop, &DesktopRect);
	SetCursorPos(DesktopRect.right,DesktopRect.bottom);
}

void goToBottonLeft (){
	RECT DesktopRect;
	HWND hDesktop=::GetDesktopWindow();
	::GetWindowRect(hDesktop, &DesktopRect);
	SetCursorPos(0,DesktopRect.bottom);
}

void goToCenter (){
	RECT DesktopRect;
	HWND hDesktop=::GetDesktopWindow();
	::GetWindowRect(hDesktop, &DesktopRect);
	SetCursorPos(DesktopRect.right/2,DesktopRect.bottom/2);
}

int currentXPosition(){
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	float currentXPos = cursorPos.x;
	return (int)currentXPos;
}
int currentYPosition(){
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	float currentYPos = cursorPos.y;
	return (int)currentYPos;
}

bool CheckLeftMouseButtonStatus()
{
   if ((GetKeyState(VK_LBUTTON) & 0x80) != 0){
      return true;
   }else return false;
}

bool CheckRightMouseButtonStatus()
{
   if ((GetKeyState(VK_RBUTTON) & 0x80) != 0)
      return true;
   else return false;
}

void LeftClick()
{
	INPUT In={0};													// Create our input.

	In.type        = INPUT_MOUSE;									// Let input know we are using the mouse.
	In.mi.dwFlags  = MOUSEEVENTF_LEFTDOWN;							// We are setting left mouse button down.
	SendInput( 1, &In, sizeof(INPUT) );								// Send the input.

	ZeroMemory(&In,sizeof(INPUT));									// Fills a block of memory with zeros.
	In.type        = INPUT_MOUSE;									// Let input know we are using the mouse.
	In.mi.dwFlags  = MOUSEEVENTF_LEFTUP;								// We are setting left mouse button up.
	SendInput( 1, &In, sizeof(INPUT) );								// Send the input.
}

void RightClick()
{
	INPUT    In={0};													// Create our input.

	In.type        = INPUT_MOUSE;						// Let input know we are using the mouse.
	In.mi.dwFlags  = MOUSEEVENTF_RIGHTDOWN;							// We are setting left mouse button down.
	SendInput( 1, &In, sizeof(INPUT) );								// Send the input.

	ZeroMemory(&In,sizeof(INPUT));									// Fills a block of memory with zeros.
	In.type        = INPUT_MOUSE;									// Let input know we are using the mouse.
	In.mi.dwFlags  = MOUSEEVENTF_RIGHTUP;								// We are setting left mouse button up.
	SendInput( 1, &In, sizeof(INPUT) );								// Send the input.
}
//==================================
/* main */
int main (int argc, const char** argv){

	cv::ocl::setUseOpenCL(true);
	cameraImage();
	return 1;
}


int cameraImage(){
	VideoCapture cap(cameraNumber);
	cout << "Camera x resolution: " << cap.get(CV_CAP_PROP_FRAME_WIDTH) << "\n";
	cout << "Camera y resolution: " << cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	Mat frame;
	//==================
	// size of screen (primary monitor) without taskbar or desktop toolbars
	RECT DesktopRect;
	HWND hDesktop=::GetDesktopWindow();
	   // Gets the Desktop window rect or screen resolution in pixels
	::GetWindowRect(hDesktop, &DesktopRect);
	cout <<  "\nScreen X resolution: "<<DesktopRect.right<<  "\nScreen Y resolution: "<< DesktopRect.bottom<<"\n";

	//==================
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

		 //moveMouse(faces[i].x,faces[i].y,0);
		 RightClick();
		 /*for( size_t j = 0; j < eyes.size(); j++ )
		      {
		        Point center( faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5 );
		        int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
		        circle( frame, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );

				 cout <<  " Posicao face"<< ((((float)faces[i].x)/640)*1920) <<  " Posicao olhos"<< eyes[j].x;

		      }*/
	}
	imshow(window, frame);
}

void moveMouse (int x, int y, int totalPixels){
	int positionX,positionY;

	positionX=(((float)x)/640)*1920;
	positionY=(((float)y)/480)*1080;
	SetCursorPos(positionX*2,positionY*4);
	/*POINT cursorPos;
	int totalMovementX,totalMovementY;
	GetCursorPos(&cursorPos);
	float currentXPos = cursorPos.x;
	float currentYPos = cursorPos.y;
	totalMovementX=positionX-currentXPos;
	totalMovementY=positionY-currentYPos;
	 if (totalMovementX>0)
		 for (size_t i=0;i<=totalMovementX;i++){
			 SetCursorPos(currentXPos++*2,currentYPos);
		 }
	 else for(size_t i=totalMovementX;i!=0;i++){
		 SetCursorPos(currentXPos--*2,currentYPos);
	 }
	 if (totalMovementY>0)
	 	for (size_t i=0;i<=totalMovementY;i++){
	 			 SetCursorPos(currentXPos,currentYPos++*2);}
	  else for (size_t i=totalMovementY;i!=0;i++){
	 		 SetCursorPos(currentXPos,currentYPos--*2);}*/
}
