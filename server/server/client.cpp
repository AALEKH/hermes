/*
   client.cpp

   Test client for the tcpsockets classes. 

   ------------------------------------------

   Copyright (c) 2013 Vic Hargrave

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "tcpconnector.h"

using namespace std;

int main(int argc, char** argv)
{
    if (argc != 3) {
        printf("usage: %s <port> <ip>\n", argv[0]);
        exit(1);
    }

    int len;
    string message;
    char line[5120];
    TCPConnector* connector = new TCPConnector();
    TCPStream* stream = connector->connect(argv[2], atoi(argv[1]));
    if (stream) {
        // message = "{\"operation\": \"get\", \"bucket\": \"rogue1\", \"key\": \"force2\", \"value\": \"with me\"}";

        string hello = "{\"operation\": \"get\", \"bucket\": \"rogue1\", \"key\": \"force2\", \"value\": \"with me\"}";
  
        string complete_message = "{\"channel\": \"test\", \"message\": ["+ hello + "]}";
        stream->send(complete_message.c_str(), complete_message.size());
        printf("sent - %s\n", complete_message.c_str());
        len = stream->receive(line, sizeof(line));
        line[len] = NULL;
        printf("received - %s\n", line);
        // delete stream;

        delete stream;
    }

    exit(0);
}
