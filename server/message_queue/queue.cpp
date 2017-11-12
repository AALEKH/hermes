/*
   queue.cpp

   Implements Inmemory Queue functionality and file system interaction's
   ------------------------------------------

   MIT License

	Copyright (c) 2017 Aalekh Nigam

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
#include <stdio.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include "queue.h"


// Storing queue message to file system by appending or creating file

void MessageQueue::file_system_store(std::string filename, std::string message) {
	std::ofstream outfile;
	outfile.open(filename, std::ios_base::app);
	outfile << message << std::endl; // Appends new line to start for next message
}

// Insert's a new message for queue channel

void MessageQueue::insert_message_to_queue(std::string channel, std::string message) {
	std::string filename = channel + ".txt";
	inmemory_queue[channel].push(message);
	file_system_store(filename, message); // Don't store yet
}

std::string MessageQueue::get_Element(std::string channel) {
	std::string element = inmemory_queue[channel].front();
	inmemory_queue[channel].pop();
	return element;
}

int MessageQueue::select_operation(std::string operation) {
	if(operation[0] == 'P' || operation[0] == 'p') return 1;
	return 0;
}

// Dump existing queue sequence to file over file storage

void MessageQueue::dump_queue_to_file() {
	std::map<std::string, std::queue<std::string> > mymap;
	std::string filename;
	for (std::map<std::string, std::queue<std::string> >::iterator it=mymap.begin(); it!=mymap.end(); ++it){
		filename = it->first + ".txt";
		while (!it->second.empty()) {
		    file_system_store(filename, it->second.front());
		    it->second.pop();
		}
	}
}

// Delete file with old queue dump and replace it with new one

// void queue::delete_File(std::string f_name) {

// 	std::string file_name = f_name + ".txt";

// 	if( remove( file_name ) != 0 )
// 		std::cout << "Error deleting file" << std::endl;
// 	else
// 		std::cout << "File successfully deleted" << std::endl;
// }

/// Read to Vector from file

std::map<std::string, std::queue<std::string> > MessageQueue::inmemory_queue_daemon(std::string channel) {

	std::string S;
	std::string filename = channel + ".txt";
	std::ifstream fs(filename);

	while (fs >> S) {
    	inmemory_queue[channel].push(S);
	}
	return inmemory_queue;
}


/// Funtion to map file to line number write mapping, this will run as a daemon to remove deleted lines in the end

std::vector<std::string> MessageQueue::file_to_line_map(std::string filename) {

	std::vector<std::vector<std::string> >     data;

    std::ifstream  file(filename);
    std::string   line;
    std::string token;
    int counter = 0;
    std::vector<std::string> file_line; // 0'th index is file name, 1'st index is line to skip

    std::string::size_type sz;   // alias of size_t

    while(file >> line) {

        std::vector<std::string>   lineData;
        std::stringstream  lineStream(line);
        std::string value;
        while(lineStream >> value)
        {
            lineData.push_back(value);
        }
        data.push_back(lineData);

        std::istringstream ss(data[counter][0]);

	    while(std::getline(ss, token, ':')) {
	    	file_line.push_back(token);
		}
		counter++;
    }

    return file_line;
} 