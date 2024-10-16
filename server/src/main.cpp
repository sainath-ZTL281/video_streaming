#include "logger.h"
#include "server.hpp"

bool stop_recording = false;

int main(int argc, char *argv[]) 
{
    if(argc < 2)
    {
        cerr << "\nUsage: ./build/server <port>\n" << endl;
        return -1;
    }
    const int port = stoi(argv[1]);


    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        cerr << "Socket failed" << endl;
        exit(EXIT_FAILURE);
    }
    
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) 
    {
        cerr << "setsockopt failed" << endl;
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
        cerr << "Bind failed" << endl;
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) 
    {
        cerr << "Listen failed" << endl;
        exit(EXIT_FAILURE);
    }
    //LOG(LOG_LEVEL_INFO,"\n\nServer listening at: %s\n", getIPAddress() );
    //cout<<"Server listening at: "<<endl;
    //VideoCapture cap("http://10.10.3.43:8080/video"); 
    VideoCapture cap(0); 

    if (!cap.isOpened()) 
    {
        cerr << "Cannot open the video stream." << endl;
        return -1;
    }

    namedWindow("Server Video Stream", WINDOW_AUTOSIZE);
    
    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) >= 0) 
    {
        cout << "Client connected" << endl;
        thread(handle_client, new_socket, ref(cap)).detach();
    }

    cap.release();
    close(server_fd);
    return 0;
}
