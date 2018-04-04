#include <iostream>
#include <sstream>
#include <string>
#include <vector>


using namespace std;
void setup_conn_dialog();
//Returns generated label as a string in format: "qn_label"
string q_button_generator(int n)
{
	string q_label = "q";
	stringstream temp;
	string num;
	temp<<n;
	num = temp.str();
	q_label.append(num);
	q_label.append("_button");
	return q_label;
}

string create_send_data(vector<string> input)
{
	string to_send = "";
	int l = input.size();
	for(int i = 0;i<l;i++)
	{
		to_send.append(input.at(i));
		if(i<l-1)
		to_send.append(";");
	}
	return to_send;
}

string stripString(string inp, char c)
{
	int l = inp.size();
	//string out = "";
	while(inp.at(0)==c)
	{
		string out(l-1, '.');
		for(int i = 0; i<l-1; i++)
		{
			out.at(i) = inp.at(i+1);
		}
		inp = out;
		l = inp.size();
	}
	
	while(inp.at(l-1)==c)
	{
		string out(l-1, '.');
		for(int i = 0; i<l-1; i++)
		{
			out.at(i) = inp.at(i);
		}
		inp = out;
		l = out.size();
	}
	
	return inp;
}

//Function to split a string (input) using character (c) and return a vector array of individual members
vector<string> stringSplit(string input,char c)
{
	int no_of_c = 1;
	int len_of_input = input.length();
	for(int i = 0 ; i< len_of_input; i++)
	{
		if(input[i]==c) no_of_c++;
	}
	vector<string> splitString(no_of_c, "");
	int fill_count = 0;
	for(int i =0; i<len_of_input; i++)
	{
		if(input[i]!=c)
		{
			splitString[fill_count].append(1u,input[i]);
		}
		else if(input[i]==c)
		{
			fill_count++;
			continue;
		}
	}
	return splitString;
}

//Function to convert string numerals to integer
int to_int(string input)
{
	int l = input.length();
	int ans = 0;
	for(int i = 0; i < l; i++)
	{
		ans *= 10;
		ans += int(input[i])-48;
	}
	return ans;
}
