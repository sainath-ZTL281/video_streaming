#include "client.hpp"

int main(int argc, char *argv[]) 
{
    if(argc < 3)
    {
        cerr << "\nUsage: ./build/client <server_ip> <port>\n" << endl;
        return -1;
    }
    const char *server_ip = argv[1];
    const int port = stoi(argv[2]);

    int sock = CreateSocket();
    if (sock < 1) return -1;
    if (ConnectToServer(sock, server_ip, port) < 0) return -1;
    process_command(sock);
    close(sock);
    return 0;
}
