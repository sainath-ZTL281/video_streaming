// #include <iostream>
// #include <opencv2/opencv.hpp>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <unistd.h>
// #include <thread>
// #include <csignal>

// using namespace cv;
// using namespace std;
// extern bool stop_recording; 

// void handle_client(int new_socket, VideoCapture& cap);


#include <iostream>
#include <opencv2/opencv.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <csignal>
#include <mutex>

using namespace cv;
using namespace std;
extern bool stop_recording; 
extern std::mutex cap_mutex;  // Mutex for synchronizing access to VideoCapture

void handle_client(int new_socket, VideoCapture& cap);
