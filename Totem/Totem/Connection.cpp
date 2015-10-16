#include "Connection.h"

CConnection::CConnection(){};
CConnection::~CConnection(){};

void CConnection::Intialise(){
	string hostlist = GetHostList("fping -a -g -r 1 192.168.1.12 192.168.1.20 2> /dev/null -q");

	std::cout<<hostlist<< std::endl;
}


string CConnection::GetHostList(string cmd){
	string data;
	FILE * stream;
	const int max_buffer = 256;
	char buffer[max_buffer];
	cmd.append(" 2>&1");

	stream = popen(cmd.c_str(), "r");
	if (stream) {
		while (!feof(stream))
			if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
			pclose(stream);
	}
	return data;

}