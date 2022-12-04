#include "http_server.hh"

#include <vector>

#include <sys/stat.h>

#include <fstream>
#include <sstream>

vector<string> split(const string &s, char delim) {
  vector<string> elems;

  stringstream ss(s);
  string item;

  while (getline(ss, item, delim)) {
    if (!item.empty())
      elems.push_back(item);
  }

  return elems;
}

string load_html(string path)
{
	ifstream file_stream;
	file_stream.open(path);
	
	stringstream ss;
	ss<< file_stream.rdbuf();
	
  file_stream.close();
	return ss.str();

}

HTTP_Request::HTTP_Request(string request) {
  vector<string> lines = split(request, '\n');
  vector<string> first_line = split(lines[0], ' ');

  this->HTTP_version = "1.0"; // We'll be using 1.0 irrespective of the request

  /*
   TODO : extract the request method and URL from first_line here
  */
	this->method = first_line[0];

  if (this->method != "GET") {
    cerr << "Method '" << this->method << "' not supported" << endl;
    exit(1);
  }
  
	this->url = first_line[1];
  
}

HTTP_Response *handle_request(string req) {
  HTTP_Request *request = new HTTP_Request(req);

  HTTP_Response *response = new HTTP_Response();

  string url = string("html_files") + request->url;

  response->HTTP_version = "1.0";

  struct stat sb;
  if (stat(url.c_str(), &sb) == 0) // requested path exists
  {
    response->status_code = "200";
    response->status_text = "OK";
    response->content_type = "text/html";

    string body;

    if (S_ISDIR(sb.st_mode)) {
      /*
      In this case, requested path is a directory.
      TODO : find the index.html file in that directory (modify the url
      accordingly)
      */
      url = url+ "/index.html";
    }
    /*
    TODO : open the file and read its contents
    */
		response->body = load_html(url);			
    /*
    TODO : set the remaining fields of response appropriately
    */
    struct stat html_sb;
    stat(url.c_str(), &html_sb);
    response->content_length = to_string(html_sb.st_size);
  }

  else {
    response->status_code = "404";

    /*
    TODO : set the remaining fields of response appropriately
    */
    response->status_text = "Not Found";
    response->content_type = "text/html";
    response->body = "<html><body><h1>Not Found</h1></body></html>";
    response->content_length = to_string(response->body.size());
  }

  delete request;

  return response;
}

string HTTP_Response::get_string() {
  /*
  TODO : implement this function
  */
  string response_string;
  response_string = "HTTP/" + this->HTTP_version + " " + this->status_code + " " + this->status_text + "\r\n"\
  +"Content-Type: " + this->content_type + "\r\n"\
  +"Content-Length: " + this->content_length + "\r\n"\
  +"\r\n"\
  +this->body;
 	return response_string;
}
