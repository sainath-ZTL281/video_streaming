#include <iostream>
#include <opencv2/opencv.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <signal.h>
#include "logger.h"
#include <string>
#include <termios.h>
#include <fcntl.h>
#include <cstring> 
#include <thread>

#include <iostream>
#include <thread> 
#include <chrono> 

#include <algorithm>
#include <cctype>
#include<string.h>
#include <fcntl.h>    

// #include <sys/types.h>
// #include <netdb.h>

//#define PORT 8080

using namespace cv;
using namespace std;

int CreateSocket();
int ConnectToServer(int sock, const char *server_ip, int port);
void record_video(int sock);
//int RecordFrame(int,int,vector<uchar>);

void process_command(int sock);
//std::string getIPAddress() ;