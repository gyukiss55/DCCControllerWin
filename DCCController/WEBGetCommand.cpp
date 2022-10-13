/*
 * WEBGetCommand.cpp
*/

#include <string.h>
#include <stdio.h>
#include <string>

#include "WEBGetCommand.h"
#include "ClientSendRec.h"



const char* sendTextArray[] = {
	"GET ",
	nullptr,
	" HTTP/1.1\n",
	"Host: ",
	nullptr,
	"\n",
//	"Connection: keep-alive\n",
//	"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/105.0.0.0 Safari/537.36\n",
//	"Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8\n",
	"Referer: http://",
	nullptr,
	nullptr,
	"\n",
//	"Accept-Encoding: gzip, deflate\n",
//	"Accept-Language: hu-HU,hu;q=0.9,en-US;q=0.8,en;q=0.7\n\n\n\n",
	nullptr,
	nullptr };



void PrintUsage(char *progPath) {
	printf("usage for client: %s -c hostname port text\n", progPath);
	printf("usage for server: %s -s port\n", progPath);

}

void FillinSendString(std::string& sendString, const char * command, const char* ipAddress)
{
	int j = 0;
	sendString.clear();
	for (int i = 0; j < 5; ++i) {
		if (sendTextArray[i] == nullptr) {
			switch (j) {
			case 1:
			case 2:
				sendString += ipAddress;
				break;
			case 0:
			case 3:
				sendString += command;
				break;
			}
			++j;
			continue;
		}
		sendString += sendTextArray[i];
	}
	printf("send string length:%zd send str:####\n%s\n####\n", sendString.length(), sendString.c_str());
}

#define IP_ADDRESS_STR "192.168.2.69"

int WEBGetConsolIO()
{
	std::string sendString;
	FillinSendString(sendString, "/ ", IP_ADDRESS_STR);
	std::string recString;
	int err = ClientSendRec("192.168.2.69", sendString.c_str (), recString);;
	printf("ClientSendRec err:%d rec len:%zd rec str:#%s#\n", err, recString.length (), recString.c_str ());
	for (bool run = true; run; ) {
		char ch = getchar();
		switch (ch) {
		case 'Q':
		case 'q':
			run = false;
			break;
		case '0':
			FillinSendString(sendString, "/L ", IP_ADDRESS_STR);
			err = ClientSendRec("192.168.2.69", sendString.c_str(), recString);
			printf("ClientSendRec err:%d rec len:%zd rec str:#%s#\n", err, recString.length(), recString.c_str());
			break;
		case '1':
			FillinSendString(sendString, "/H ", IP_ADDRESS_STR);
			err = ClientSendRec("192.168.2.69", sendString.c_str(), recString);
			printf("ClientSendRec err:%d rec len:%zd rec str:#%s#\n", err, recString.length(), recString.c_str());
			break;
		case 'C':
		case 'c':
			FillinSendString(sendString, "/C=12,a=34,v=7892 ", IP_ADDRESS_STR);
			err = ClientSendRec("192.168.2.69", sendString.c_str(), recString);
			printf("ClientSendRec err:%d rec len:%zd rec str:#%s#\n", err, recString.length(), recString.c_str());
			break;
		case 'S':
		case 's':
			FillinSendString(sendString, "/S=12,a=34 ", IP_ADDRESS_STR);
			err = ClientSendRec("192.168.2.69", sendString.c_str(), recString);
			printf("ClientSendRec err:%d rec len:%zd rec str:#%s#\n", err, recString.length(), recString.c_str());
			break;
		}
	}
	return  err;
}
