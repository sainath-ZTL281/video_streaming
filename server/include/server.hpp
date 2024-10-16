#include <iostream>
#include <opencv2/opencv.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <csignal>
#include <mutex>
#include <atomic>
#include <vector>

#include <chrono>

using namespace cv;
using namespace std;
extern std::mutex cap_mutex;  


void handle_client(int new_socket, VideoCapture& cap);
