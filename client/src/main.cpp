#include "client.hpp"

int main(int argc, char *argv[]) 
{
    if(argc < 3)
    {
        cerr << "\nUsage: ./client <server_ip> <port>\n" << endl;
        return -1;
    }
    const char *server_ip = argv[1];
    const int port = stoi(argv[2]);

    int sock = CreateSocket();
    if (sock < 1) return -1;
    if (ConnectToServer(sock, server_ip, port) < 0) return -1;

    string  client_hostname = GetTheHostName();
    send(sock, client_hostname.c_str(), client_hostname.length(), 0);
    //get the server hostname
    char hostname[1024] = {0};
    memset(hostname, 0, sizeof(hostname));
    int bytes_read  = read(sock, hostname, sizeof(hostname)-1);
    string server_hostname;
    
    if (bytes_read > 0)
    {
        hostname[bytes_read] = '\0'; 
        server_hostname = hostname;
        LOG(LOG_LEVEL_INFO, "\n\nConnected to server : %s\n", server_hostname.c_str()); 
    }
    else
    {
        LOG(LOG_LEVEL_ERROR, "\n\nFailed to get the  server's hostname\n");
    }
    process_command(sock);
    close(sock);
    return 0;
}
