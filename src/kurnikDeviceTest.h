#ifndef KURNIKDEVICETEST_H_
#define KURNIKDEVICETEST_H_

#define NC "\e[0m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define CYN "\e[0;36m"
#define REDB "\e[41m"

#define COM_PORT	"/dev/ttyUSB0"

#include <fstream>
#include <iostream>
#include <string>

using namespace std;



void assert(int id, string tag, string expect, string faultMessage, fstream& data, string passMessage = "", int maxLine = 40);
void ignore(string content, fstream& data, int times=1, int wait=40);
void comSend(string command);


#endif /* KURNIKDEVICETEST_H_ */
