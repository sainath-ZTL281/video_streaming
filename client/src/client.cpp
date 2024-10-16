#include "client.hpp"
#include "logger.h"

bool stop = false;
#define buffer_size 1024

string get_timestamp() 
{
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);
    char buf[100];
    strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", localtime(&in_time_t));
    return string(buf);
}

void record_video(int sock) 
{
    vector<uchar> buffer;
    int frame_size;

    string filename = "client_video_" + get_timestamp() + ".mp4";

    // Receive frame size
    int bytes_received = recv(sock, (char*)&frame_size, sizeof(frame_size), 0);
    if (bytes_received <= 0) 
    {
        cerr << "Server has closed the connection. Stopping..." << endl;
        exit(0);
    }
    // Resize buffer to the received frame size
    buffer.resize(frame_size);
    LOG(LOG_LEVEL_INFO, "Frame size : %d", frame_size);
    //std::cout << "Frame size : " << frame_size << std::endl; 
    int total_bytes_received = 0;
    while (total_bytes_received < frame_size) 
    {
        int result = recv(sock, buffer.data() + total_bytes_received, frame_size - total_bytes_received, 0);
        if (result <= 0) 
        {
            cerr << "Error receiving frame data. Stopping..." << endl;
            return;
        }
        total_bytes_received += result;
    }
    if (total_bytes_received != frame_size) 
    {
        cerr << "Warning: Received frame size (" << total_bytes_received << ") does not match expected size (" << frame_size << ")." << endl;
    }

    send(sock, "ACK", 3, 0);
    Mat frame = imdecode(buffer, IMREAD_COLOR);
    if (frame.empty()) 
    {
        cerr << "Error decoding frame." << endl;
    }
    //int result = RecordFrame(sock,frame_size,buffer);
    VideoWriter video(filename, VideoWriter::fourcc('m', 'p', '4', 'v'), 30, Size(frame.cols, frame.rows));

    LOG(LOG_LEVEL_INFO,"\n\nRecording video... Press ESC to stop. \n");
    //cout << "Recording video... Press ESC to stop." << endl;
    while (true) 
    {
        // Receive frame size
        int bytes_received = recv(sock, (char*)&frame_size, sizeof(frame_size), 0);
        if (bytes_received <= 0) 
        {
            cerr << "Server has closed the connection. Stopping..." << endl;
            break;
        }

        buffer.resize(frame_size);
        //std::cout << "Frame size : " << frame_size << std::endl; 
        int total_bytes_received = 0;
        while (total_bytes_received < frame_size) 
        {
            int result = recv(sock, buffer.data() + total_bytes_received, frame_size - total_bytes_received, 0);
            if (result <= 0) 
            {
                cerr << "Error receiving frame data. Stopping..." << endl;
                break;
            }
            total_bytes_received += result;
        }
        if (total_bytes_received != frame_size) 
        {
            cerr << "Warning: Received frame size (" << total_bytes_received << ") does not match expected size (" << frame_size << ")." << endl;
            continue; 
        }

        send(sock, "ACK", 3, 0);
        Mat frame = imdecode(buffer, IMREAD_COLOR);
        if (frame.empty()) 
        {
            cerr << "Error decoding frame." << endl;
            break;
        }
        video.write(frame);

        imshow("Client Video Stream", frame);
        if (waitKey(30) == 27) 
        {
            cv::destroyAllWindows();
            cout << "Stopping recording..." << endl;
            break;
        }
    }
    video.release();
    cout << "Video saved as " << filename << endl;
}
void process_command(int sock) 
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
            record_video(sock);
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
    // Convert IP from text to binary
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
