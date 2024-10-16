#include "logger.h"
#include "server.hpp"

std::vector<thread> client_threads;

int main(int argc, char *argv[]) 
{
    if(argc < 2)
    {
        cerr << "\nUsage: ./server <port>\n" << endl;
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

    if (listen(server_fd, SOMAXCONN) < 0) 
    {
        cerr << "Listen failed" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Server listening for clients on port " << port << endl;
    //VideoCapture cap("http://10.10.3.43:8080/video"); 
    VideoCapture cap; 
    cap.open(0);

    if (!cap.isOpened()) 
    {
        cerr << "Cannot open the video stream." << endl;
        return -1;
    }

     cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
     cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    //namedWindow("Server Video Stream", WINDOW_AUTOSIZE);
    while (true) 
    {
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) 
        {
            cerr << "Accept failed" << endl;
            continue;
        }
        //cout << "New client connected "<< endl;
        client_threads.emplace_back(handle_client, new_socket, ref(cap));
    }
    for (auto& t : client_threads) 
    {
        if (t.joinable()) 
        {
            t.join();
        }
    }
    cap.release();
    close(server_fd);
    return 0;
}
