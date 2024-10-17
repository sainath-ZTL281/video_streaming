#include "server.hpp"
#include "logger.h"

std::mutex cap_mutex; 
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

void HandleClient(int new_socket, VideoCapture& cap) 
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
    while(1)
    {
        int valread = recv(new_socket, command, 9,0);
        if (valread > 0 && strcmp(command, "save_data") == 0) 
        {
            LOG(LOG_LEVEL_INFO, "\n\nRecording started by client: %s\n", client_hostname.c_str());
            bool recording = true;
            while(recording)
            {        
                {
                    std::lock_guard<std::mutex> lock(cap_mutex); 
                    if (!cap.read(frame)) 
                    {
                        LOG(LOG_LEVEL_ERROR,"\nfailed to read frame from server camera");
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        continue;
                    }
                }
                if (frame.empty()) 
                {
                    LOG(LOG_LEVEL_ERROR,"\nReceived empty frame. Retrying...");
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    continue;
                }
                if (imencode(".jpg", frame, buffer)) 
                {
                    frame_size = buffer.size();
                } 
                else 
                {
                    LOG(LOG_LEVEL_ERROR, "\nFailed to encode frame.");
                    continue; 
                }
                if (send(new_socket, (char*)&frame_size, sizeof(frame_size), 0) <= 0)
                {
                    LOG(LOG_LEVEL_INFO,"\nClient disconnected while sending frame size.");
                    break;  
                }
                int sent_size = send(new_socket, buffer.data(), frame_size, 0);
                if (sent_size <= 0)
                {
                    LOG(LOG_LEVEL_INFO, "\nClient disconnected while sending frame data.");
                    break;  
                }
                if (sent_size < frame_size) 
                {
                    break;
                }
                char ack[10] = {0}; 
                int ack_read = recv(new_socket, ack, sizeof(ack), 0);
                if (ack_read > 0) 
                {
                    std::string ack_str(ack);
                    if (ack_str == "STOP") {
                        recording = false;
                        LOG(LOG_LEVEL_INFO, "\n\nRecording stopped by client: %s\n", client_hostname.c_str());
                        break;
                    }
                }
                else if (ack_read <= 0)
                {
                    break;
                }        
            }
        }
        else
        {
            break;
        }
    }
    close(new_socket);
    LOG(LOG_LEVEL_INFO, "\n\nClient disconnected : %s",client_hostname.c_str());
}
