#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Path.h>
#include <Poco/Net/NetException.h>
#include <iostream>
#include <string>
//#include "utilities.cpp"

using namespace Poco::Net;
using namespace Poco;
using namespace std;

string server_universal;
//This function should be called atleast once to set the ip of the server
void set_server(string server="127.0.0.1")
{
	server_universal = server;
}


//This function send the get request. Begin with : "/"
string exchange_data(string path="")
{
    HTTPClientSession session(server_universal, 8080);
    string to_send = "/";
    to_send.append("(");
    to_send.append(path);
    to_send.append(")");
    string resp = "";
    //try
    //{
		// send request
		HTTPRequest req(HTTPRequest::HTTP_GET, to_send, HTTPMessage::HTTP_1_1);
		session.sendRequest(req);
		// get response
		HTTPResponse res;

		// print response
		istream &is = session.receiveResponse(res);
		
		while(is)
		{
			string temp;
			is>>temp;
			temp.append(" ");
			resp.append(temp);
		}										
		//cout<<"\n\n\tResponse : "<<resp;
		resp = stripString(resp, ' ');
	//}
    //catch(Poco::Net::ConnectionRefusedException &ex)
    //{
		//gtk_window_set_transient_for(GTK_WINDOW(conn_error_dialog), GTK_WINDOW(login_window));
		//setup_conn_dialog();
	//}
    return resp;
}



/*int main(int argc, char **argv)
{
	set_server();
	string response;
	response = exchange_data("/abcd");
}*/
