#include "server.hpp"
#include <atomic>
#include <iostream>
#include <thread>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>


using namespace std;

namespace chaos
{
    namespace server
    {
        void start(const int port, atomic_bool& start_flag, atomic_bool& stop_flag)
        {
            struct sockaddr_in servaddr, cli;

            // socket create and verification 
            int sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd == -1)
            {
                cerr << "Socket creation failed: " << strerror(errno) << endl;
                return;
            }

            cout << "Socket successfully created" << endl;
            bzero(&servaddr, sizeof servaddr);

            // assign IP, PORT 
            servaddr.sin_family = AF_INET;
            servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
            servaddr.sin_port = htons(port);

            // Binding newly created socket to given IP and verification 
            if (bind(sockfd, reinterpret_cast<struct sockaddr*>(&servaddr), sizeof servaddr) != 0)
            {
                cerr << "Socket bind failed: " << strerror(errno) << endl;
                return;
            }

            cout << "Socket successfully bound.." << endl;

            // Now server is ready to listen and verification 
            if (listen(sockfd, 5) != 0)
            {
                cerr << "Listen failed: " << strerror(errno) << endl;
                return;
            }

            cout << "Server listening..." << endl;

            socklen_t len = sizeof cli;

            // Accept the data packet from client and verification 
            int connfd = accept(sockfd, reinterpret_cast<struct sockaddr*>(&cli), &len);
            if (connfd < 0)
            {
                cerr << "Server accept failed: " << strerror(errno) << endl;
                return;
            }

            cout << "Server accepted the client." << endl;

            char buff[254];
            int read_bytes;

            // Function for chatting between client and server 
            while (!stop_flag)
            {
                bzero(buff, 256);

                // read the message from client and copy it in buffer 
                read_bytes = recv(connfd, buff, 255, 0);

                if (read_bytes > 0)
                {
                    buff[256] = '\0';
                    cout << "Message received: " << buff << endl;

                    if (strcmp(buff, "start") == 0)
                    {
                        start_flag = true;
                    }
                    else if (strcmp(buff, "stop") == 0)
                    {
                        stop_flag = true;
                    }
                }
            }

            cout << "Closing server..." << endl;

            // After chatting close the socket 
            close(connfd);
            close(sockfd);
        }

        unique_ptr<thread> start_background(const int port, atomic_bool& start_flag, atomic_bool& stop_flag)
        {
            return unique_ptr<thread>(new thread(&start, port, ref(start_flag), ref(stop_flag)));
        }
    }
}
