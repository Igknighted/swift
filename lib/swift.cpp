/* swift - a web CGI made for C++
 * Copyright (C) 2010 Samuel Peterson <sam.igknighted@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <cstring>
#include <fstream>
#include <sstream>


using namespace std;

typedef map<string, char*> submap_string;

class swift
{
	public:
		swift(string);
		string content_type;
		string output_string;
		char *endh;
		
		char* request(string);
		char* rawrequest(string);
		int requeststore(string, string);
		int requestlen(string);
		
		char* contentin;
		
		map<string, char*> rdata_disposition;
		map<string, char*> rdata_filename;
		map<string, int> rdata_length;
		map<string, int> rdata_start;
		map<string, int> rdata_end;
		map<string, string> rdata_type; // either GET or POST
		
		map<string, string> template_blocks;
		map<string, string> template_assignments;
		
		void header(string, string);
		
		void setcookie(string, string);
		void unsetcookie(string);
		
		void output(string);
		
		void gettemplate(string);
		string getbetween(string,string,string);
		string replacebetween(string,string,string,string);
		string insertbefore(string, string, string);
		string insertbeforelast(string, string, string);
		string sanitize(string);
		void assign(string, string);
		void parse(string);
		void display();

		string itoa(int);
		
		string str_replace(string, string, string);
		string str_replace_single(string, string, string);
		
	private:
		// Used for storing get URL data, form data, and multipart form data.
		// request_data_name -> (char_len / data / content_disposition / filename / Content-Type)


		void process_data();
		int where_starts(char *&, char *&, int, int);
		int where_ends(char *&, char *&, int, int);
		void query_string_parser(char *, int, string);
		void cookie_string_parser(char *, int, string);
		
		map<string, string> cookiesetter;
		map<string, string> headersetter;
		
		bool first_parse;
};
 
swift::swift(string content_typein)
{
	content_type = content_typein;
	endh = "\r\n";
	output_string = "";
	first_parse = true;
	
	process_data();
}

char * swift::request(string variable)
{
	if(rdata_type[variable].compare("COOKIE") == 0)
	{
		char * contentstream = getenv("HTTP_COOKIE");
		char * out = new char[rdata_length[variable] + 1];
		for(int i = rdata_start[variable]; i < rdata_end[variable]; i++)
		{
			out[(i - rdata_start[variable])] = contentstream[i];
		}
		out[rdata_length[variable]] = '\0'; // put null term on this thing...
		return out;
	}else if(rdata_type[variable].compare("POST") == 0){
		char * out = new char[rdata_length[variable] + 1];
		for(int i = rdata_start[variable]; i < rdata_end[variable]; i++)
		{
			out[(i - rdata_start[variable])] = contentin[i];
		}
		out[rdata_length[variable]] = '\0'; // put null term on this thing...
		return out;
	}else{
		char * contentstream = getenv("QUERY_STRING");
		char * out = new char[rdata_length[variable] + 1];
		for(int i = rdata_start[variable]; i < rdata_end[variable]; i++)
		{
			out[(i - rdata_start[variable])] = contentstream[i];
		}
		out[rdata_length[variable]] = '\0'; // put null term on this thing...
		return out;
	}
	
	return "Undefined";
}

char * swift::rawrequest(string variable)
{
	if(rdata_type[variable].compare("COOKIE") == 0)
	{
		char * contentstream = getenv("HTTP_COOKIE");
		char * out = new char[rdata_length[variable]];
		for(int i = rdata_start[variable]; i < rdata_end[variable]; i++)
		{
			out[(i - rdata_start[variable])] = contentstream[i];
		}
		return out;
	}else if(rdata_type[variable].compare("POST") == 0){
		char * out = new char[rdata_length[variable]];
		for(int i = rdata_start[variable]; i < rdata_end[variable]; i++)
		{
			out[(i - rdata_start[variable])] = contentin[i];
		}
		return out;
	}else{
		char * contentstream = getenv("QUERY_STRING");
		char * out = new char[rdata_length[variable]];
		for(int i = rdata_start[variable]; i < rdata_end[variable]; i++)
		{
			out[(i - rdata_start[variable])] = contentstream[i];
		}
		return out;
	}
	
	return "Undefined";
}

int swift::requeststore(string variable, string filename)
{
	ofstream storefile; 
	storefile.open(filename.c_str());
	if(rdata_type[variable].compare("COOKIE") == 0)
	{
		char * contentstream = getenv("HTTP_COOKIE");
		for(int i = rdata_start[variable]; i < rdata_end[variable]; i++)
		{
			storefile << contentstream[i];
		}
		storefile.close();
		return 0;
	}else if(rdata_type[variable].compare("POST") == 0){
		for(int i = rdata_start[variable]; i < rdata_end[variable]; i++)
		{
			storefile << contentin[i];
		}
		storefile.close();
		return 0;
	}else{
		char * contentstream = getenv("QUERY_STRING");
		for(int i = rdata_start[variable]; i < rdata_end[variable]; i++)
		{
			storefile << contentstream[i];
		}
		storefile.close();
		return 0;
	}
	
	storefile.close();
	return 1;
}

int swift::requestlen(string variable)
{
	return rdata_length[variable];
}


void swift::process_data()
{
	string request_method = getenv("REQUEST_METHOD");
	
	// post data content
	char *content_type, *data_type, *boundary, *boundary_split;
	int content_len = 0;
	
	int iterator = 0;

	content_type = getenv("CONTENT_TYPE");
	

	
	// get the boundary length
	boundary_split = "boundary=";
	int boundary_len = 0;

	if(content_type != NULL)
	{
		if(where_ends(content_type, boundary_split, 0, strlen(content_type)) > 0)
			boundary_len = strlen(content_type) - where_ends(content_type, boundary_split, 0, strlen(content_type));
	}
	
	if (getenv("CONTENT_LENGTH") != NULL && request_method.compare("POST") == 0)
	{
		content_len = atoi(getenv("CONTENT_LENGTH"));
		
		char buff;
		contentin = new char[content_len];

		// Read in the content
		iterator = 0;
		while(fread(&buff,1,1,stdin) && iterator < content_len)
		{
			contentin[iterator] = buff;
			iterator++;
		}
		
		int c;
		c = 0;
		while(content_type[c] != ';' && c <= 255){
			c++;
		}
		data_type = new char[c];
		for(int i = 0; i < c; i++){
			data_type[i] = content_type[i];
		}
		
		
		// GET THE BOUNDARY!!!!
		boundary = new char[boundary_len];
		for (int i = where_ends(content_type, boundary_split, 0, content_len); i < strlen(content_type); i++)
		{
			boundary[(i - where_ends(content_type, boundary_split, 0, content_len))] = content_type[i];
		}
	}
	
	// add the cookie jar to the requests
	if (getenv("HTTP_COOKIE") != NULL)
	{
		char * cookie_string = getenv("HTTP_COOKIE");
		int cookie_string_len = strlen(getenv("HTTP_COOKIE"));
		// need to parse cookies
		cookie_string_parser(cookie_string, cookie_string_len, "COOKIE");
	}
	
	// query string data
	char * query_string = getenv("QUERY_STRING");
	int query_string_len = strlen(getenv("QUERY_STRING"));
	if(query_string_len > 0)
	{
		query_string_parser(query_string, query_string_len, "GET");
	}
	
	// not multipart form data
	if (getenv("CONTENT_LENGTH") != NULL && request_method.compare("POST") == 0 && boundary_len <= 0)
	{
		query_string_parser(contentin, content_len, "POST");
	}
	
	// multipart form data! =D
	if (getenv("CONTENT_LENGTH") != NULL && request_method.compare("POST") == 0 && boundary_len > 0) 
	{

		// Get the field names
		char *split;
		string fieldname = "";
		string filename = "";
		string disposition = "";
		string content_type = "";
		iterator = 0;

		// Get content disposition to start with
		split = "Content-Disposition: ";
		iterator = where_ends(contentin, split, iterator, content_len);

		while(iterator < content_len && iterator > -1)
		{
			int start_iterator = iterator;
			
			while(iterator < content_len && iterator > -1 && contentin[iterator] != ';')
			{
				disposition += contentin[iterator];
				iterator++;
			}
			
			// Get the fieldname
			split = " name=\"";
			iterator = where_ends(contentin, split, iterator, content_len);
			while(iterator < content_len && iterator > -1 && contentin[iterator] != '"')
			{
				fieldname += contentin[iterator];
				iterator++;
			}
			// Where does our data start at?
			split = "\r\n\r\n";
			int data_start = where_ends(contentin, split, iterator, content_len);
			
			
			// Test for our filename
			split = " filename=\"";
			int fcheck = where_ends(contentin, split, iterator, content_len);
			
			if(fcheck < data_start && contentin[fcheck] != '"' && contentin[fcheck+1] != ';')
			{
				iterator = fcheck;
				while(contentin[iterator] != '"' && iterator < data_start && iterator < content_len && iterator > -1)
				{
					 filename += contentin[iterator];
					 iterator++;
				}
			}else{
				filename = "Undefined";
			}
			
			// Test for content type
			split = "Content-Type: ";
			int dtcheck = where_ends(contentin, split, iterator, content_len);
			
			if(dtcheck < data_start && contentin[dtcheck] != '\r' && contentin[dtcheck+1] != '\n')
			{
				iterator = dtcheck;
				while(contentin[iterator] != '\r' && iterator < data_start && iterator < content_len && iterator > -1)
				{
					 content_type += contentin[iterator];
					 iterator++;
				}
			}else{
				content_type = "Undefined";
			}
			
			// We need to move to where the data starts
			iterator = data_start;
			
			// Where does the data end?
			split = new char[strlen(boundary) + 4];
			strcat(split, "\r\n--");
			strcat (split,boundary);
			int data_end = where_starts(contentin, split, iterator, content_len);
			
			// count data size
			int data_len = (data_end - data_start);
			
			
			
			// Organize this data into a usable format now
			rdata_disposition[fieldname] = new char[strlen(disposition.c_str())];
			strcpy(rdata_disposition[fieldname],disposition.c_str());
			
			rdata_filename[fieldname] = new char[strlen(filename.c_str())];
			strcpy(rdata_filename[fieldname],filename.c_str());
			
			rdata_length[fieldname] = data_len;

			rdata_start[fieldname] = data_start;
			rdata_end[fieldname] = data_end;
			
			rdata_type[fieldname] = "POST";
			
	
			// Cleanup for next iteration of this while loop
			fieldname = "";
			disposition = "";
			filename = "";
			content_type = "";
			
			// Get content disposition to continue with
			split = "Content-Disposition: ";
			iterator = where_ends(contentin, split, iterator, content_len);
		}

	}
	
	

}

void swift::cookie_string_parser(char * streamin, int content_len, string data_type)
{
	bool isfieldname = true;
	string fieldname = "";
	string valuebuffer;
	int iterator = 0;
	while(iterator < content_len)
	{
		int data_start = 0;
		int data_end = 0;
		int data_len = 0;

		if(streamin[iterator] == ' ')
		{
			iterator++;
		}
		
		while(iterator < content_len && streamin[iterator] != '=')
		{
			fieldname += streamin[iterator];
			
			iterator++;
		}
		iterator++;
		data_start = iterator;

		while(iterator < content_len && streamin[iterator] != ';')
		{
			valuebuffer += streamin[iterator];
			iterator++;
		}

		
		
		data_end = iterator;
		data_len = (data_end - data_start);


		rdata_disposition[fieldname] = "Undefined";
		rdata_filename[fieldname] = "Undefined";			
		rdata_length[fieldname] = data_len;
		rdata_start[fieldname] = data_start;
		rdata_end[fieldname] = data_end;
		rdata_type[fieldname] = data_type;

		fieldname = "";
		valuebuffer = "";

		iterator++;
	}
}

void swift::query_string_parser(char * streamin, int content_len, string data_type)
{
	bool isfieldname = true;
	string fieldname = "";
	string valuebuffer;
	int iterator = 0;
	while(iterator < content_len)
	{
		int data_start = 0;
		int data_end = 0;
		int data_len = 0;

		while(iterator < content_len && streamin[iterator] != '=')
		{
			fieldname += streamin[iterator];
			iterator++;
		}
		iterator++;
		data_start = iterator;

		while(iterator < content_len && streamin[iterator] != '&')
		{
			valuebuffer += streamin[iterator];
			iterator++;
		}

		data_end = iterator;
		data_len = (data_end - data_start);

		rdata_disposition[fieldname] = "Undefined";
		rdata_filename[fieldname] = "Undefined";			
		rdata_length[fieldname] = data_len;
		rdata_start[fieldname] = data_start;
		rdata_end[fieldname] = data_end;
		rdata_type[fieldname] = data_type;

		fieldname = "";
		valuebuffer = "";

		iterator++;
	}
}

/* returns the place that searchfor starts
 * 
 * Example: where_ends("hello world", "lo", 0, 11);
 * return: 0
 */
int swift::where_starts(char *&stringin, char *&searchfor, int start_pos, int contentlen)
{
	if(start_pos < 0)
		return -1;

	int string_iterator = start_pos;
	int searchfor_iterator = 0;

	if(string_iterator >= contentlen || start_pos < 0)
	{
		return -1;
	}

	while (string_iterator < contentlen && searchfor_iterator < strlen(searchfor))
	{
		if (stringin[string_iterator] == searchfor[searchfor_iterator])
		{
			searchfor_iterator++;
		}else{
			string_iterator = string_iterator - searchfor_iterator;
			searchfor_iterator = 0;
		}
		string_iterator++;
	}

	if (string_iterator >= contentlen || (string_iterator - strlen(searchfor)) < 0)
	{
		return -1;
	}
	return (string_iterator - strlen(searchfor));
}

/* returns the place after the searchfor
 * 
 * Example: where_ends("hello world", "lo", 0, 11);
 * return: 5
 */
int swift::where_ends(char *&stringin, char *&searchfor, int start_pos, int contentlen)
{

	int string_iterator = start_pos;
	int searchfor_iterator = 0;
	

	if(string_iterator >= contentlen || start_pos < 0)
	{
		return -1;
	}
	
	while (string_iterator < contentlen && searchfor_iterator < strlen(searchfor))
	{
		if (stringin[string_iterator] == searchfor[searchfor_iterator])
		{
			searchfor_iterator++;
		}else{
			string_iterator = string_iterator - searchfor_iterator;
			searchfor_iterator = 0;
		}
		string_iterator++;
	}

	if (string_iterator >= contentlen)
	{
		return -1;
	}

	return string_iterator;
}


void swift::setcookie(string key, string val)
{
	cookiesetter[key] = val;
}

void swift::unsetcookie(string key)
{
	cookiesetter[key] = "";
}

void swift::header(string key, string val)
{
	headersetter[key] = val;
}










void swift::output(string html)
{
	output_string += html;
}







void swift::gettemplate(string filename){
	char buffer;
	ifstream infile;
	infile.open(filename.c_str());
	
	while(infile.get(buffer)) {
		output_string += buffer;
	}
	
	infile.close();
	
	string begin_op = "<!-- BEGIN: ";
	string end_op = "<!-- END: ";
	string close_op = " -->";
	
	
	string blockname = "start", begin_block_comment, end_block_comment, block_identifier, block_holder_identifier;
	
	// getting all the base blocks into memory
	while(blockname != "")
	{
		blockname = getbetween(output_string, begin_op, close_op);
		if (blockname != "")
		{
			block_identifier = blockname;
			block_holder_identifier = "[__"+blockname+"__]";
			begin_block_comment = begin_op + blockname + close_op;
			end_block_comment = end_op + blockname + close_op;

			// check to ensure for end block, if none exist remove it
			if(output_string.find(end_block_comment) == string::npos){
				int start, end;
				start = output_string.find(begin_block_comment);
				end = start + begin_block_comment.length();
				output_string.erase(start,end);
			}else{
				// store it
				template_blocks[block_identifier] = getbetween(output_string, begin_block_comment, end_block_comment);

				// remove it from output
				output_string = replacebetween(output_string, begin_block_comment, end_block_comment, block_holder_identifier);
			}
		}
	}

	// find the sub blocks now until there are no more... this is probably redundant, but we'll work this out later.
	bool blockfind = true;
	blockname = "startonthesubblocks";
	while(blockfind)
	{
		blockfind = false;
		//template_blocks[blockname]
		for(map<string, string>::iterator iterator = template_blocks.begin(); iterator != template_blocks.end(); iterator++) {
			string block_parent = iterator->first;
			
			
			blockname = getbetween(template_blocks[block_parent], begin_op, close_op);
			if (blockname != "")
			{
				block_identifier = block_parent + "." + blockname;
				block_holder_identifier = "[__"+block_parent+"."+blockname+"__]";
				begin_block_comment = begin_op + blockname + close_op;
				end_block_comment = end_op + blockname + close_op;

				// check to ensure for end block, if none exist remove it
				if(template_blocks[block_parent].find(end_block_comment) == string::npos){
					int start, end;
					start = template_blocks[block_parent].find(begin_block_comment);
					end = start + begin_block_comment.length();
					template_blocks[block_parent].erase(start,end);
				}else{
					// store it
					template_blocks[block_identifier] = getbetween(template_blocks[block_parent], begin_block_comment, end_block_comment);

					// remove it from output
					template_blocks[block_parent] = replacebetween(template_blocks[block_parent], begin_block_comment, end_block_comment, block_holder_identifier);
					blockfind = true;
				}
			}
			
			
		}
	}
	
	
}


string swift::getbetween(string strin, string str1, string str2){
	string out = "";
	int start, end;
	start = strin.find(str1) + str1.length();
	end = strin.find(str2);
	
	if(start == string::npos || end == string::npos)
		return "";
	
	for(int i = start; i < end; i++)
	{
		out += strin[i];
	}
	
	return out;
}



string swift::str_replace_single(string strin, string strremove, string stradd)
{
	string out = "";
	int start, end;
	start = strin.find(strremove);
	end = start + strremove.length();
	
	if(start == string::npos)
		return "";
	
	for(int i = 0; i < start; i++)
		out += strin[i];

	out += stradd;

	for(int i = end; i < strin.length(); i++)
		out += strin[i];

	return out;
}

string swift::str_replace(string strin, string strremove, string stradd)
{
	string out = strin;
	
	while(out.find(strremove) != string::npos)
	{
		out = str_replace_single(out, strremove, stradd);
	}
	return out;
}

string swift::replacebetween(string strin, string str1, string str2, string placeholder)
{
	string out = "";
	int start, end;
	start = strin.find(str1);
	end = strin.find(str2) + str2.length();
	
	if(end == string::npos || end < 1)
		return strin;
	
	for(int i = 0; i < start; i++)
		out += strin[i];
	
	if(placeholder.length() > 0)
		out += placeholder;
	
	for(int i = end; i < strin.length(); i++)
		out += strin[i];
	
	return out;
}

string swift::insertbeforelast(string strin, string stradd, string str1)
{
	string out = "";
	int start;
	start = strin.find_last_of(str1);

	if(start == string::npos)
		return "";

	for(int i = 0; i < start; i++)
		out += strin[i];

	out += stradd;

	for(int i = start; i < strin.length(); i++)
		out += strin[i];

	return out;
}

string swift::insertbefore(string strin, string stradd, string str1)
{
	string out = "";
	int start;
	start = strin.find(str1);
	
	if(start == string::npos)
		return "";
	
	for(int i = 0; i < start; i++)
		out += strin[i];
	
	out += stradd;
	
	for(int i = start; i < strin.length(); i++)
		out += strin[i];
	
	return out;
}

string swift::sanitize(string strin){
	string out = strin;
	string tmp = "";
	
	bool erased_something = true;
	while(erased_something)
	{
		erased_something = false;
		if(out.find("[__") != string::npos && out.find("__]") != string::npos)
		{
			tmp = "";
			int end = out.find("__]") + 3;
			int start = out.find("[__");
			for(int i = 0; i < start; i++)
				tmp += out[i];
			for(int i = end; i < out.length(); i++)
				tmp += out[i];
			out = tmp;
			erased_something = true;
		}
	}
	
	out = str_replace(out, "{$", "{\\$");
	
	return out;
}

void swift::assign(string key, string val){
	template_assignments[key] = sanitize(val);
	template_assignments[key] = val;
}

void swift::parse(string blockname){
	string outgoing = template_blocks[blockname];
	
	for(map<string, string>::iterator iterator = template_assignments.begin(); iterator != template_assignments.end(); iterator++) {
		string key = iterator->first;
		string val = iterator->second;
		string true_key = "{$" + key + "}";
		
		// need to make an str_replace() command... 
		outgoing = str_replace(outgoing, true_key, val);
		//outgoing.replace(outgoing.find(true_key), true_key.length(), val);
	}
	
	string block_holder = "[__" + blockname + "__]";
	
	if(first_parse) {
		output_string = insertbefore(output_string, outgoing, block_holder);
	}else{
		output_string = insertbeforelast(output_string, outgoing, block_holder);
	}
}


string swift::itoa(int i)
{
	std::string s;
	std::stringstream out;
	out << i;
	s = out.str();
	
	return s;
}

void swift::display()
{
	cout << "Content-type: " << content_type << "\r\n";
	
	// send cookies
	typedef map<string, string>::iterator hashmap_iterator;
	for(hashmap_iterator iterator = cookiesetter.begin(); iterator != cookiesetter.end(); iterator++) {
		cout << "Set-Cookie: " << iterator->first << "=" << iterator->second <<  "\r\n";
	}
	
	for(hashmap_iterator iterator = headersetter.begin(); iterator != headersetter.end(); iterator++) {
		if (iterator->second.length() > 0)
			cout << iterator->second <<  "\r\n";
	}
	
	
	
	cout << "Status: 200 OK\r\n";
	cout << "\r\n";
	
	

	output_string = sanitize(output_string);
	
	// clean up the output
	bool cleaned_some = false;
	while(cleaned_some)
	{
		cleaned_some = false;
		if(output_string.find("\n") == 0)
		{
			output_string.erase(0,1);
			cleaned_some = true;
		}
		if(output_string.find("\r\n") == 0)
		{
			output_string.erase(0,2);
			cleaned_some = true;
		}
	}
	
	
	output_string = str_replace(output_string, "{\\$", "{$");
	cout << output_string;

	
}

