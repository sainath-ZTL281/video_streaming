#include "server.hpp"

std::mutex cap_mutex; 
std::atomic<bool> stop_recording(false);

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
        cout << "Recording video..." << endl;
        while (!stop_recording) 
        {
            {
                std::lock_guard<std::mutex> lock(cap_mutex); 
                cap >> frame;
            }
            
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
            // int sent_size = send(new_socket, buffer.data(), frame_size, 0);
            // if (sent_size <= 0)
            // {
            //     cout << "Client disconnected while sending frame data." << endl;
            //     break;  
            // }
            // std::cout << "Sent size : " << sent_size << endl;

            int total_sent = 0;
            while (total_sent < frame_size) 
            {
                int sent_size = send(new_socket, buffer.data() + total_sent, frame_size - total_sent, 0);
                if (sent_size <= 0) 
                {
                    cout << "Client disconnected while sending frame data." << endl;
                    break;
                }
                total_sent += sent_size;
            }

            if (total_sent < frame_size) 
            {
                break;
            }
            char ack[10] = {0}; 
            int ack_read = recv(new_socket, ack, sizeof(ack), 0);
            if (ack_read <= 0) 
            {
                cout << "Client disconnected while waiting for acknowledgment." << endl;
                break;  
            }
            //cout << "Received acknowledgment: " << ack << endl;
            video.write(frame);
        }
        video.release();
    }
    close(new_socket);
    cout << "Client disconnected." << endl;
}
