/*
   queue.h

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

#ifndef __queue_h__
#define __queue_h__

#include <map>
#include <queue>
#include <vector>
#include <string>

class MessageQueue {
	public:

    	MessageQueue();
    	virtual ~MessageQueue();
    	void file_system_store(std::string, std::string);
    	void insert_message_to_queue(std::string, std::string);
    	void dump_queue_to_file();
    	std::string get_Element(std::string);
    	int select_operation(std::string);
    	// void delete_File(std::string); /// Work on remove all file method
    	std::map<std::string, std::queue<std::string> > inmemory_queue_daemon(std::string);
    	std::vector<std::string> file_to_line_map(std::string);
    private:
    	// Master map storing queue for each channel (used here as key)
		std::map<std::string, std::queue<std::string> > inmemory_queue;

};

#endif