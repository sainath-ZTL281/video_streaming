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

using namespace cv;
using namespace std;

int CreateSocket();
int ConnectToServer(int sock, const char *server_ip, int port);
void record_video(int sock);

void process_command(int sock);