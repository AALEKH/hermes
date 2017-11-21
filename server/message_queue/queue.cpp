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
#include <map>
#include <queue>
#include <string>
#include <vector>
#include <string>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include <boost/filesystem.hpp>

#include "queue.h"

using namespace boost::filesystem;

// Storing queue message to file system by appending or creating file

MessageQueue::MessageQueue() : inmemory_queue() {}

MessageQueue::~MessageQueue() {}

std::vector<std::string> MessageQueue::file_names() {

	std::vector<std::string> filename_vec;
    std::string intermediate;
    path p("../../infile");
    for (auto i = directory_iterator(p); i != directory_iterator(); i++)
    {
        if (!is_directory(i->path())) //we eliminate directories in a list
        {
            intermediate = i->path().filename().string();
            intermediate = intermediate.substr(0, intermediate.size()-4);
        	filename_vec.push_back(intermediate);
        }
        else
            continue;
    }
    return filename_vec;

}

int MessageQueue::rename_files (std::string c_name) {

  int result;
  std::string rename_element_f = "../../infile/" + c_name + ".temporary.txt";
  std::string rename_element_t = "../../infile/" + c_name + ".txt";
  result= rename( rename_element_f.c_str() , rename_element_t.c_str() );
  if ( result == 0 )
    puts ( "File successfully renamed" );
  else
    perror( "Error renaming file" );
  return 0;
}

void MessageQueue::delete_files (std::string filename) {

	std::string delete_element = "../../infile/" + filename+ ".txt";
	if( remove(delete_element.c_str()) != 0 )
    	perror( "Error deleting file" );
  	else
    	puts( "File successfully deleted" );
}

void MessageQueue::file_system_store(std::string filename, std::string message) {
	std::ofstream outfile;
	outfile.open(filename, std::ios_base::app);
	outfile << message << std::endl; // Appends new line to start for next message
}

// Insert's a new message for queue channel

void MessageQueue::insert_message_to_queue(std::string channel, std::string message) {
	std::string filename = "../../infile/" + channel + ".txt";
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
	// std::map<std::string, std::queue<std::string> > mymap;
	std::string filename;
	for (std::map<std::string, std::queue<std::string> >::iterator it=inmemory_queue.begin(); it!=inmemory_queue.end(); ++it){
		filename = "../../infile/" + it->first + ".temporary.txt";
		while (!it->second.empty()) {
		    file_system_store(filename, it->second.front());
		    it->second.pop();
		}
	}
}

/// Read to Vector from file

std::map<std::string, std::queue<std::string> > MessageQueue::inmemory_queue_daemon(std::string channel) {

	std::string S;
	std::string filename = "../../infile/" + channel + ".txt";
	std::ifstream fs(filename);

	while (fs >> S) {
    	inmemory_queue[channel].push(S);
	}
	return inmemory_queue;
}

// Fetched each element from a directory and load all the elements to queue
void MessageQueue::load_map() {
	std::vector<std::string> files = file_names();
	for(int i = 0; i < files.size(); i++) {
		inmemory_queue_daemon(files[i]);
	}
}

void MessageQueue::dump_map() { // This function is to dump all data
	std::string filename;
	dump_queue_to_file();
	for (std::map<std::string, std::queue<std::string> >::iterator it=inmemory_queue.begin(); it!=inmemory_queue.end(); ++it){
		delete_files(it->first);
		rename_files(it->first);
	}
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