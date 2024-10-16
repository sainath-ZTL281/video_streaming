#include "server.hpp"
#include "logger.h"

std::mutex cap_mutex; 
std::atomic<bool> stop_recording(false);

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

void handle_client(int new_socket, VideoCapture& cap) 
{
    Mat frame;
    vector<uchar> buffer;
    int frame_size;
    char hostname[1024] = {0};
    string client_hostname;

    int bytes_read  = read(new_socket, hostname, sizeof(hostname)-1);
    if (bytes_read > 0)
    {
        hostname[bytes_read] = '\0'; 
        client_hostname = hostname;
        LOG(LOG_LEVEL_INFO, "\n\nConnected Client: %s\n", client_hostname.c_str()); 
    }
    else
    {
        LOG(LOG_LEVEL_ERROR, "Failed to get the  client's hostname\n");
        close(new_socket);
        return;
    }
    string  server_hostname = GetTheHostName();
    send(new_socket, server_hostname.c_str(), server_hostname.length(), 0);

    char command[256];
    memset(command, 0, sizeof(command));

    int valread = recv(new_socket, command, 9,0);//sizeof(command), 0);
    std::string command1(command); 

    if (valread > 0 && strcmp(command, "save_data") == 0) 
    {
        cout << "Recording video at "<<client_hostname << endl;
        while (!stop_recording) 
        {
            {
                std::lock_guard<std::mutex> lock(cap_mutex); 
                //cap >> frame;
                if (!cap.read(frame)) 
                {
                    cerr << "Failed to read frame from camera" << endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    continue;
                }
            }
            if (frame.empty()) 
            {
                cerr << "Received empty frame. Retrying..." << endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            if (imencode(".jpg", frame, buffer)) 
            {
                frame_size = buffer.size();
                //cout << "Sending frame of size: " << frame_size << " bytes" << endl;
            } 
            else 
            {
                cerr << "Failed to encode frame." << endl;
                continue; 
            }
            if (send(new_socket, (char*)&frame_size, sizeof(frame_size), 0) <= 0)
            {
                cout << "Client disconnected while sending frame size." << endl;
                break;  
            }
            int sent_size = send(new_socket, buffer.data(), frame_size, 0);
            if (sent_size <= 0)
            {
                cout << "Client disconnected while sending frame data." << endl;
                break;  
            }
            //std::cout << "Sent size : " << sent_size << endl;
            if (sent_size < frame_size) 
            {
                break;
            }
            char ack[10] = {0}; 
            int ack_read = recv(new_socket, ack, sizeof(ack), 0);
            if (ack_read <= 0) 
            {
                break;  
            }
        }
    }
    close(new_socket);
    cout << "Client disconnected : " <<client_hostname<< endl;
}
