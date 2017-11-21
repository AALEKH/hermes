/*
   main.cpp

   Multithreaded work queue based example server in C++.
  
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

#include <map>
#include <mutex>
#include <ctime>
#include <vector>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>
#include <stdint.h>
//////

#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "thread.h"
#include "wqueue.h"
#include "tcpacceptor.h"

#include "json.hpp"

using namespace nlohmann;
using namespace std;
// using names
std::mutex mutex_x;

struct StorageObject
{
    const string& key;
    const string& value;
    char *time;
};


void process(const string& key, const string& value){
    std::time_t result = std::time(nullptr);
    char *time_object_creation = std::asctime(std::gmtime(&result));
    StorageObject st_object = {
        key,
        value,
        time_object_creation
    };
    
}

std::string process_Queue_Server(MessageQueue* que, std::string value, char input[4096]) {
    
    // For automatic management of locks
    std::lock_guard<std::mutex> guard(mutex_x);

    std::string return_message, message, channel, operation, word;

    auto j3 = json::parse(value);
    channel = j3["channel"].get<std::string>();
    operation = j3["operation"].get<std::string>();

    if(que->select_operation(operation)) {
        message = j3["message"][0].dump();
        que->insert_message_to_queue(channel, message);
        return_message = "Successfully Inserted";
    } else {
        return_message = que->get_Element(channel);
        que->dump_map();
    }
    memset(&input[0], 0, sizeof(input)); // Removing elements of array 'input'
    return return_message; 
}

class SynchronizedFile {
    public:
        SynchronizedFile (const string& path) : _path(path) {
            // Open file for writing...
            myfile.open (path, std::ofstream::out | std::ofstream::app);
        }

        void write (const string& dataToWrite, const string& situationId) {
            // Write to the file in a synchronized manner (described below)...
            // Ensure that only one thread can execute at a time
            std::lock_guard<std::mutex> lock(_writerMutex);
            std::time_t result = std::time(nullptr);
            char *foo = std::asctime(std::gmtime(&result));
            foo[strlen(foo) - 1] = 0;
            myfile << foo << " |~| " << result << " |~| " << situationId << " |~| "<<" seconds since the Epoch\n";
            myfile.close();
        }

    private:
        string _path;
        ofstream myfile;
        std::mutex _writerMutex;
};

class Writer {
    public:
        Writer (std::shared_ptr<SynchronizedFile> sf) : _sf(sf) {}

        void someFunctionThatWritesToFile (const string& dataToInsert, const string& situationId) {
            // Do some work...
            _sf->write(dataToInsert, situationId);
        }
    private:
        std::shared_ptr<SynchronizedFile> _sf;
        // std::mutex _writerMutex;
};

class WorkItem
{
    TCPStream* m_stream;
 
  public:
    WorkItem(TCPStream* stream) : m_stream(stream) {}
    ~WorkItem() { delete m_stream; }
 
    TCPStream* getStream() { return m_stream; }
};

class ConnectionHandler : public Thread
{
    wqueue<WorkItem*>& m_queue;
 
  public:
    ConnectionHandler(wqueue<WorkItem*>& queue) : m_queue(queue) {}

    void* run() {

        json object;
        json::string_t value;
        std::string return_message, message, channel, operation, word;
        const char *s1;
        json j;
        MessageQueue* que = new MessageQueue();
        for (int i = 0;; i++) {
            printf("thread %lu, loop %d - waiting for item...\n", 
                   (long unsigned int)self(), i);
            WorkItem* item = m_queue.remove();
            printf("thread %lu, loop %d - got one item\n", 
                   (long unsigned int)self(), i);
            TCPStream* stream = item->getStream();


            char input[4096];
            int len;
            while ((len = stream->receive(input, sizeof(input)-1)) > 0 ) {
////////////////////////////////////

                // Optimise it, currently it read's file for each iteration which isn't good at all
                que->load_map();
                input[len] = NULL;
                value = std::string(input);

                // Where stuff happens
                word = process_Queue_Server(que, value, input) ;
                strcpy(input, word.c_str());
                stream->send(input, sizeof(input));
                // mu.unlock();
                printf("thread %lu, echoed '%s' back to the Sender\n", 
                       (long unsigned int)self(), input);
///////////////////////////////////////////
            }
            delete item; 
        }
        // que->dump_map();
        delete que;

        // Should never get here
        return NULL;
    }
};

int main(int argc, char** argv)
{
    // Process command line arguments
    if ( argc < 3 || argc > 4 ) {
        printf("usage: %s <workers> <port> <ip>\n", argv[0]);
        exit(-1);
    }
    int workers = atoi(argv[1]);
    int port = atoi(argv[2]);
    string ip;
    if (argc == 4) { 
        ip = argv[3];
    }
 
    // Create the queue and consumer (worker) threads
    wqueue<WorkItem*>  queue;
    for (int i = 0; i < workers; i++) {
        ConnectionHandler* handler = new ConnectionHandler(queue);
        if (!handler) {
            printf("Could not create ConnectionHandler %d\n", i);
            exit(1);
        } 
        handler->start();
    }
 
    // Create an acceptor then start listening for connections
    WorkItem* item;
    TCPAcceptor* connectionAcceptor;
    if (ip.length() > 0) {
        connectionAcceptor = new TCPAcceptor(port, (char*)ip.c_str());
    }
    else {
        connectionAcceptor = new TCPAcceptor(port);        
    }                                        
    if (!connectionAcceptor || connectionAcceptor->start() != 0) {
        printf("Could not create an connection acceptor\n");
        exit(1);
    }

    // Add a work item to the queue for each connection
    while (1) {
        TCPStream* connection = connectionAcceptor->accept(); 
        if (!connection) {
            printf("Could not accept a connection\n");
            continue;
        }
        item = new WorkItem(connection);
        if (!item) {
            printf("Could not create work item a connection\n");
            continue;
        }
        queue.add(item);
    }
 
    // Should never get here
    exit(0);
}
