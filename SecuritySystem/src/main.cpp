#include "camera.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <cstdio>
#include <sys/time.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>
#include <sys/select.h>


// sphinxbase includes
#include <sphinxbase/err.h>
#include <sphinxbase/ad.h>

// pocketsphinx include
#include <pocketsphinx.h>

using namespace cv;

static ps_decoder_t *ps;
static cmd_ln_t *config;

Mat emailPhoto1, emailPhoto2, emailPhoto3;
int motionLoopCounter = 0;
int largestMotion = 20;
int isArmed = 1;
int isVoiceControlled = 0;
static int serial_port = 0;

// Capture size
#define WIDTH 512
#define HEIGHT 512
/* THE FOLLOWING FUNCTION CHECKS FOR MOTION BY COMPARING THREE FRAMES
AND GENERATING A BINARY IMAGE WHERE WHITE PIXELS REPRESENT PIXELS THAT
SHOWED CHANGE FROM FRAME-TO-FRAME AND BLACK PIXELS WERE THOSE THAT REMAINED
UNCHANGED BETWEEN FRAMES. WHITE PIXELS ARE COUNTED TO DETERMINE WHETHER
MOVEMENT OCCURRED. RETURNS 1 IF PIXEL COUNT IS > 0. */
static bool init_serial(char *port)
{
	printf("Opening serial port %s\n", port);

	// Open the serial port for reading and writing
	// Using non-blocking so 'read' won't block the code
	serial_port = open(port, O_RDWR, O_NONBLOCK);

	// If the serial port doesn't open return an error
	if (serial_port == -1) {
		fprintf(stderr, "Failed to open serial port:\n%s\n", strerror(errno));
		return false;
	}

	// Configure the serial port
	termios tio; // termios is a struct defined in termios.h
	memset(&tio, 0, sizeof(termios)); // Zero out the tio structure
	tio.c_cflag = CS8; // Select 8 data bits
	tio.c_cc[VMIN] = 0; // Demand at least 1 char from every call to read(), i.e. block execution until a char is received
	cfsetospeed(&tio, B9600); // baud rate for output
	cfsetispeed(&tio, B9600); // baud rate for input
	tcsetattr(serial_port, TCSANOW, &tio); // Apply these settings

	// Done
	return true;
}

// THIS FUNCTION SETS THE GLOBAL LARGEST MOTION VARIABLE TO A DESIRED VALUE
void setLargestMotion(int motionThreshold){
	largestMotion = motionThreshold;
}

// THIS FUNCTION INCREMENTS THE GLOBAL MOTION LOOP COUNTER VARIABLE
void increaseMotionCounter(){
	++motionLoopCounter;
}

// THIS FUNCTION RESETS THE GLOBAL MOTION LOOP COUNTER VARIABLE
void resetMotionCounter(){
	motionLoopCounter = 0;
}

// THIS FUNCTION RETURNS THE CURRENT VALUE OF THE GLOBAL MOTION LOOP COUNTER VARIABLE
int getMotionCounterValue(){
	return motionLoopCounter;
}

// THIS FUNCTION SAVES PHOTOS FOR EMAILING USING THE GLOBAL PHOTO VARIABLES
void saveEmailPhotos(){
	imwrite("/home/pi/PiCapture/src/frame1.jpg", emailPhoto1);
	imwrite("/home/pi/PiCapture/src/frame2.jpg", emailPhoto2);
	imwrite("/home/pi/PiCapture/src/frame3.jpg", emailPhoto3);
}

// THIS FUNCTION EMAILS THE PHOTOS
void emailPhotos(){
	printf("Email Photo Sequence Begins\n");
	system("python emailSystem.py");
}

// THIS FUNCTION PROVIDES A MEANS FOR MAKING THE CODE WAIT TEMPORARILY
void delay(int iterations){
	for (int i = 0; i < iterations; ++i);
}

// THE FOLLOWING FUNCTION CHECKS FOR MOTION AND RETURNS TRUE IF IT IS DETECTED
bool checkForMotion(Mat previousFrame1, Mat previousFrame2, Mat currentFrame){
	Mat difference1, difference2, result;
	int pixelcount = 0;
	int min_x = 0;
	int max_x = 0;
	int min_y = 0;
	int max_y = 0;

	// calculate difference per element in the image matrices
	absdiff(previousFrame1, currentFrame, difference1);
	absdiff(previousFrame2, currentFrame, difference2);

	// bitwise logical and operation, eliminates noise in result
	// should minimise false readings
	bitwise_and(difference1, difference2, result);

	// convert to greyscale, reduces noise a little more
	cvtColor(result, result, CV_BGR2GRAY);

	// threshold the difference image, eliminates last of noise
	threshold(result, result, 25, 255, CV_THRESH_BINARY);

	// look at each pixel in the image
	for (int x = 0; x < WIDTH; ++x){
		for (int y = 0; y < HEIGHT; ++y){
			// check if the pixel is white
			if(result.at<uchar>(Point(x, y)) == 255){
				// check whether the pixel is at a min/max position
				if (pixelcount == 0){
					min_x = x;
					max_x = x;
					min_y = y;
					max_y = y;
				} else {
					if (x < min_x) {
						min_x = x;
					} else if (x > max_x){
						max_x = x;
					}

					if (y < min_y){
						min_y = y;
					} else if (y > max_y){
						max_y = y;
					}
				}
				// count the white pixels
				++pixelcount;
			}
		}
	}
	printf("%i\n",pixelcount);

	// Expand the min and max x points for a bounding rectangle that won't
	// cut off any details of the thing moving
	if (min_x > 10) {
		min_x -= 10;
	} else if (max_x < WIDTH - 10){
		max_x += 10;
	}

	// Expand the min and max y points for a bounding rectangle that won't
	// cut off any details of the thing moving
	if (min_y > 10) {
		min_y -= 10;
	} else if (max_y < HEIGHT - 10){
		max_y += 10;
	}

	// draw a bounding rectangle around the movement in each frame
	rectangle(currentFrame, Point(min_x, min_y), Point(max_x, max_y), Scalar(255), 5);
	rectangle(previousFrame1, Point(min_x, min_y), Point(max_x, max_y), Scalar(255), 5);
	rectangle(previousFrame2, Point(min_x, min_y), Point(max_x, max_y), Scalar(255), 5);

	// save for debugging and emailing
	if (pixelcount > largestMotion){
		emailPhoto1 = previousFrame1.clone();
		emailPhoto2 = previousFrame2.clone();
		emailPhoto3 = currentFrame.clone();
		largestMotion = pixelcount;
	}

	if (pixelcount > 20){
		return true;
	} else {
		return false;
	}
}

/* THE FOLLOWING FUNCTION CAPTURES AN IMAGE FOR THE STREAM UNLESS THE
IMAGE CAPTURE FAILS */
int createStreamImage(Mat originalImage){
	// Save the image yay
	imwrite("/home/pi/PiCapture/src/t.jpg", originalImage);
	char tempImage[] = "t.jpg";
	char currentImage[] = "c.jpg";
	int result;
	result = rename ( tempImage, currentImage );
	return 1;
}

int main(int argc, char *argv[])
{
	// Initialise the serial port
	if (!init_serial(argv[1])) {
		return 1;
	}

	int pirBytesRead;
	char pirChar;

	// Open a socket for receiving
	// This is initialization
	int udpSocket, numSocketBytes;
	struct addrinfo hints, *res;
	char buf[2];
	struct sockaddr_storage theirAddress;
	socklen_t addressLength;

	// Load up address structs
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	// Get address info
	getaddrinfo(NULL, "3501", &hints, &res);

	// Create a socket and set it to non-block
	udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
	fcntl(udpSocket, F_SETFL, O_NONBLOCK);

	// Bind socket to the required port
	bind(udpSocket, res->ai_addr, res->ai_addrlen);

	// Free the address info
	freeaddrinfo(res);

	// Get address length
	addressLength = sizeof theirAddress;

	// THIS IS THE VOICE CONTROL SET UP //
	config = cmd_ln_init(NULL, ps_args(), TRUE, "-lm", "/home/pi/PiCapture/src/commands.lm", "-dict", "/home/pi/PiCapture/src/commands.dic", NULL);

	// Configure PocketSphinx
	ps_default_search_args(config);
	ps = ps_init(config);

	// Start listening to the microphone
	ad_rec_t *ad;
	int16 adbuf[2048];
	uint8 utt_started, in_speech;
	int32 k;
	char const *hyp;
	char message[256];

	// Check that the device has been opened properly
	if ((ad = ad_open_dev(cmd_ln_str_r(config, "-adcdev"),
		(int) cmd_ln_float32_r(config,
			"-samprate"))) == NULL){
				// Throw fatal error if it hasn't
		E_FATAL("Failed to open audio device\n");
		}

	// Check that recording is working
if (ad_start_rec(ad) < 0) {
				// Throw fatal error if it isn't
	E_FATAL("Failed to start recording\n");
}

	// Check that utterance has been started
if (ps_start_utt(ps) < 0){
				// Throw a fatal error if it hasn't
	E_FATAL("Failed to start utterance\n");
}

	// utterance hasn't started yet
utt_started = FALSE;

	// Open the camera at 30fps, one level of downsampling, and BGR conversion
CCamera* cam = StartCamera(WIDTH, HEIGHT, 30, 1, true);
	// some matrices to save our images into
Mat originalImage, previousFrame1, previousFrame2, currentFrame;

	// Let the user know we're ready to go
system("espeak -v en/en-sc -s 130 \"Welcome, we are ready to go!\"");

for (;;) {

			// Capture an image for the stream
	originalImage = cam->Capture(0);
	createStreamImage(originalImage);

			// If no image is captured, continue anyway but print an error
	if (!originalImage.data) {
		printf("Stream image capture failed\n");
		continue;
	}

			// Check the socket for data
	if ((numSocketBytes = recvfrom(udpSocket, buf, 4 , 0,
		(struct sockaddr *)&theirAddress, &addressLength)) > 0) {
		printf("Packet is %d bytes long\n", numSocketBytes);
	printf("Packet contains %s\n", buf);
	sscanf(buf, "%i,%i", &isArmed, &isVoiceControlled);
	}
			// Read from the serial port to see if PIR sensor is detecting motion
	pirBytesRead = read(serial_port, &pirChar, 1);
			// PIR sends 'h' when it detects motion; run motion detection when that happens
if (isVoiceControlled == 0 && isArmed == 1 && pirChar == 'h'){
	printf("Motion from PIR Detected\n");
				// If motion has been detected by the PIR, capture three frames for motion detection
	previousFrame1 = cam->Capture(0);
	delay(800);
	previousFrame2 = cam->Capture(0);
	delay(1500);
	currentFrame = cam->Capture(0);

					// Call motion detection function to check for motion
	bool motion = checkForMotion(previousFrame1, previousFrame2, currentFrame);
	if (motion) {
		printf("Motion Detected from Camera\n");
					// Increase motion loop counter
		increaseMotionCounter();

					// Only email images every 5 captures
					// Images sent are the ones with the largest motion detection
		if (getMotionCounterValue() == 2){
			pid_t sendEmailFork = fork();
			if (sendEmailFork == 0){
							// Save images for the child program to use
				saveEmailPhotos();
				emailPhotos();
				return 1;
			}
		} else if (getMotionCounterValue() > 2) {
						// Delay slightly before resetting the largest motion
			resetMotionCounter();
			setLargestMotion(20);
		}
	} else {
		printf("No Motion Detected from Camera\n");
	}

				// Allow openCV to process GUI events
	waitKey(10);
	}

if (isVoiceControlled == 1){
			// throw fatal error if audio breaks
	if ((k = ad_read(ad, adbuf, 2048)) < 0){
		E_FATAL("Failed to read audio\n");
	}

	ps_process_raw(ps, adbuf, k, FALSE, FALSE);
	in_speech = ps_get_in_speech(ps);

			// If the user is speaking, start the utterance
	if (in_speech && !utt_started) {
		utt_started = TRUE;
	}

			// If the user has stopped speaking after the utterance has started
	if (!in_speech && utt_started) {
					// End the utterance and process the speech contents
		ps_end_utt(ps);

					// this function gets the string that was heard
		hyp = ps_get_hyp(ps, NULL);
					// If there is a message, do things with it
		if (hyp) {
						// Get the contents of the message and put it in a string
			sscanf(hyp, "%s", message);
						printf("%s\n", message); // just for debugging

						// compare the message with commands
						if (strcmp(message, "ARM") == 0){
							isArmed = 1;
							printf("User said - ARM");
						} else if (strcmp(message, "DISARM") == 0){
							isArmed = 0;
							printf("User said DISARM");
						} else if (strcmp(message, "TAKEPHOTO") == 0){
							previousFrame1 = cam->Capture(0);
							for (int i = 0; i < 800; ++i);
								previousFrame2 = cam->Capture(0);
							for (int i = 0; i < 1500; ++i);
								currentFrame = cam->Capture(0);
							int motion = checkForMotion(previousFrame1, previousFrame2, currentFrame);
							if (motion == 1) {
								pid_t sendEmailFork = fork();
								if (sendEmailFork == 0){
											// Save images for the child program to use
									saveEmailPhotos();
									emailPhotos();
									return 1;
								}
							}
						} else if (strcmp(message, "GOODBYE") == 0){
							isVoiceControlled = 0;
						} else {
							system("espeak -v en/en-sc -s 130 \"Sorry, could you please repeat that?\"");
						}
					}
					memset(message, 0, 256);

						// If the utterance didn't start properly throw a fatal error
					if (ps_start_utt(ps) < 0){
						E_FATAL("Failed to start utterance\n");
						utt_started = FALSE;
						//printf("READY....\n");
					}
				}
}
}


		close(udpSocket);
		StopCamera();
		ad_close(ad);
		ps_free(ps);
		cmd_ln_free_r(config);
	}
