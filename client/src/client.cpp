#include "client.hpp"
#include "logger.h"

bool stop = false;
#define buffer_size 1024

string GetTheHostName()
{
    char hostname[HOST_NAME_MAX];
    if (gethostname(hostname, HOST_NAME_MAX) == 0) 
    {
        return string(hostname);
    } 
    else 
    {
        return "";
    }
}

string GetTimestamp() 
{
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);
    char buf[100];
    strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", localtime(&in_time_t));
    return string(buf);
}

void RecordVideo(int sock) 
{
    vector<uchar> buffer;
    int frame_size;

    string filename = "client_video_" + GetTimestamp() + ".mp4";

    int bytes_received = recv(sock, (char*)&frame_size, sizeof(frame_size), 0);
    if (bytes_received <= 0) 
    {
        LOG(LOG_LEVEL_ERROR,"\nServer has closed the connection. Stopping...");
        exit(0);
    }
    buffer.resize(frame_size);
    //LOG(LOG_LEVEL_INFO, "\nFrame size : %d", frame_size);
    int total_bytes_received = 0;
    while (total_bytes_received < frame_size) 
    {
        int result = recv(sock, buffer.data() + total_bytes_received, frame_size - total_bytes_received, 0);
        if (result <= 0) 
        {
            LOG(LOG_LEVEL_ERROR,"\nError receiving frame data. Stopping...");
            return;
        }
        total_bytes_received += result;
    }
    if (total_bytes_received != frame_size) 
    {
        LOG(LOG_LEVEL_ERROR,"Warning: Received frame size (%d) does not match expected size (%d). ",total_bytes_received, frame_size);
    }

    send(sock, "ACK", 3, 0);
    Mat frame = imdecode(buffer, IMREAD_COLOR);
    if (frame.empty()) 
    {
        LOG(LOG_LEVEL_ERROR,"\nError decoding frame.");
    }
    VideoWriter video(filename, VideoWriter::fourcc('m', 'p', '4', 'v'), 30, Size(frame.cols, frame.rows));

    LOG(LOG_LEVEL_INFO,"\n\nRecording video... Press ESC to stop. \n");
    while (true) 
    {
        int bytes_received = recv(sock, (char*)&frame_size, sizeof(frame_size), 0);
        if (bytes_received <= 0) 
        {
            LOG(LOG_LEVEL_ERROR,"\nServer has closed the connection. Stopping...");
            break;
        }

        buffer.resize(frame_size);
        int total_bytes_received = 0;
        while (total_bytes_received < frame_size) 
        {
            int result = recv(sock, buffer.data() + total_bytes_received, frame_size - total_bytes_received, 0);
            if (result <= 0) 
            {
                LOG(LOG_LEVEL_ERROR,"\nError receiving frame data. Stopping...");
                break;
            }
            total_bytes_received += result;
        }
        if (total_bytes_received != frame_size) 
        {
            LOG(LOG_LEVEL_ERROR,"Warning: Received frame size (%d) does not match expected size (%d). ",total_bytes_received, frame_size);
            continue; 
        }

        send(sock, "ACK", 3, 0);
        Mat frame = imdecode(buffer, IMREAD_COLOR);
        if (frame.empty()) 
        {
            LOG(LOG_LEVEL_ERROR,"\nError decoding frame.");
            break;
        }
        video.write(frame);

        imshow("Client Video Stream", frame);
        if (waitKey(30) == 27) 
        {
            cv::destroyAllWindows();
            LOG(LOG_LEVEL_INFO,"\n\nStopping recording...\n");
            send(sock, "STOP", 4, 0); 
            break;
        }
    }
    video.release();
    LOG(LOG_LEVEL_INFO,"\n\nVideo saved as %s\n",filename.c_str());
}
void ProcessCommand(int sock) 
{
    char buffer[1024] = {0};
    string command;
    while (1) 
    {
        LOG(LOG_LEVEL_INFO,"\n\nEnter 'save_data' to start recording or 'exit' to quit: ");
        cin >> command;
        if (command == "save_data") 
        {
            send(sock, command.c_str(), command.length(), 0);
            RecordVideo(sock);
        } 
        else if (command == "exit") 
        {
            LOG(LOG_LEVEL_INFO,"\n\nExiting application...\n");
            break;
        }
        else
        {
            LOG(LOG_LEVEL_INFO,"\n\nInvalid Command!!, Please enter the correct command\n");
        }
        memset(buffer, 0, sizeof(buffer));
    }
}
int CreateSocket()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
         LOG(LOG_LEVEL_ERROR, "Failed to create socket\n");
    }
    return sock;
}

int ConnectToServer(int sock, const char *server_ip, int port)
{
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
    {
        LOG(LOG_LEVEL_ERROR, "Invalid address/Address not supported\n");
        return -1;
    }
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        LOG(LOG_LEVEL_ERROR, "Failed to connect: %d\n", errno);
        return -1;
    }
    return 0;
}
