#include "server.hpp"

// std::string getIPAddress() 
// {
//     char host[256];
//     struct hostent *host_entry;
    
//     // Retrieve the hostname
//     if (gethostname(host, sizeof(host)) == -1) 
//     {
//         std::cerr << "Error getting hostname" << std::endl;
//         return "";
//     }

//     // Retrieve host information
//     host_entry = gethostbyname(host);
//     if (host_entry == nullptr) {
//         std::cerr << "Error getting host entry" << std::endl;
//         return "";
//     }

//     // Convert the address to IPv4 and return as string
//     return inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));
// }

void handle_client(int new_socket, VideoCapture& cap) 
{
    Mat frame;
    vector<uchar> buffer;
    int frame_size;
    char command[256];
    int valread = recv(new_socket, command, sizeof(command), 0);
    string filename = "server_video_" + to_string(time(nullptr)) + ".mp4";
    VideoWriter video(filename, VideoWriter::fourcc('m', 'p', '4', 'v'), 30, Size(frame.cols, frame.rows));

    if (valread > 0 && strcmp(command, "save_data") == 0) 
    {
        while (true) 
        {

            cout << "Recording video..." << endl;
            while (!stop_recording) 
            {
                cap >> frame;
                if (frame.empty()) 
                {
                    cerr << "Received empty frame. Stopping recording..." << endl;
                    break;
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

                std::cout << "Sent size : " << sent_size << endl;
                char ack[10] = {0}; 
                int ack_read = recv(new_socket, ack, sizeof(ack), 0);
                if (ack_read <= 0) 
                {
                    cout << "Client disconnected while waiting for acknowledgment." << endl;
                    break;  
                }
                cout << "Received acknowledgment: " << ack << endl;

                video.write(frame);
            }

            video.release();
            stop_recording = false; 
            break;
        
        }

    }
    close(new_socket);
    cout << "Client disconnected." << endl;
}
