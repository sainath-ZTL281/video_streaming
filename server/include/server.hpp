#include <iostream>
#include <opencv2/opencv.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <csignal>

//#define PORT 8080

using namespace cv;
using namespace std;
extern bool stop_recording; 

void handle_client(int new_socket, VideoCapture& cap);