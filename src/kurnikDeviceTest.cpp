/*
 ============================================================================
 Name        : kurnikDeviceTest.cpp
 Author      : Marcin Szczyrbowski
 Version     :
 Copyright   : agiss
 Description : Test for kurnik device
 ============================================================================
 */


#include "kurnikDeviceTest.h"

// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

#include <string>
#include <fstream>
#include <iostream>
#include <regex>
#include "tests/time.h"

using namespace std;

bool comPreparation();
void tests(fstream& com, int testId);


regex reg("[^A-Za-z0-9 :///.-/,><+-/!]");

ofstream log;

int main(int argc, char* argv[]) {

	int testId = 0;

	if (argc > 0) {
		sscanf(argv[1], "%d", &testId);
		cout << "skip test before " << testId << endl;
	}


    string line;

    if (!comPreparation()) return 1;
    fstream com;
    com.open("/dev/ttyUSB0", ios::in | ios::out | ios::app);

	if(!com.is_open()) {
		cout << "unable to open" << endl;
	}

	log.open("./com.log", ios::trunc | ios::out);

	log << " ----------------   start test ------------------------" << endl;

	// wait for device
    assert(000, "start",  "start", "Device started"          , com, "Device Not started");
    assert(001, "device", "ready", "Device stop before menu.", com, "Device initialized");

    // testowanie
    tests(com, testId);

    com.close();
    log.close();

    system("date \"+setTime %H:%M:%S %u %U %:::z\" >> /dev/ttyUSB0");
    printf("beep beep \n\b\n");
    return 0;
}

void tests(fstream& com, int testId) {
	if (testId <=100 ) timeTest(com);
	if (testId <=200 ) alarmTest(com);
	if (testId <=300 ) alarmOffsetTest(com);
	if (testId <=400 ) testSelectColumntime(com);
}


/**
 * @brief
 *
 * @param tag			test tag
 * @param expect		what expect in line
 * @param faultMessage
 * @param data
 * @param passMessage
 * @param maxLine
 */
void assert(int id, string tag, string expect, string faultMessage, fstream& data, string passMessage, int maxLine) {
	int i = 0;
	string line;
	char buff[100];
	if (passMessage.length() == 0) passMessage = "OK";

	while (i++ < maxLine) {
		getline(data, line);
		line = regex_replace(line, reg, "");						// remove COM pollution
		if (line.find(tag) != string::npos) {						// find tag / mark for test
			if (line.find(expect) != string::npos) {
				sprintf(buff, "%03d  %-45s", id, passMessage.c_str());
				cout << GRN << buff << NC << line << endl;
				log << "OK\t" << id << ": " << i << ": " << line << endl;
				return;
			} else {
				sprintf(buff, "%03d  %-40s expect %s but %s", id, faultMessage.c_str(), expect.c_str(), line.c_str());
				cout << RED << buff << endl;
				log << "EE!\t" << id << ": " << i << ": " << line << endl;
				return;
			}
		}
		log << "\t" << id << ": " << i << ": " << line << endl;
	}
	cout << RED << id << ": " << faultMessage << " tag: " << tag << " not find" << endl;
}

void ignore(string content, fstream& data, int times, int wait) {
	int i = 0;
	string line;

	while (i++ < wait) {
		getline(data, line);
		line = regex_replace(line, reg, "");						// remove COM pollution
		if (line.find(content) != string::npos) {					// find tag / mark for test
			times--;
			log << "ignored line:" << "\t" << line << endl;
			if (times == 0) return;
		}
		log << "\t" << ": " << i << ": " << line << endl;
	}
}

/**
 * @private
 * @brief prepare com port to job
 *
 * @return true if ok else false
 */
bool comPreparation() {
	 struct termios tty;

	tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
	tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
	tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
	tty.c_cflag |= CS8; // 8 bits per byte (most common)
	tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
	tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO; // Disable echo
	tty.c_lflag &= ~ECHOE; // Disable erasure
	tty.c_lflag &= ~ECHONL; // Disable new-line echo
	tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

	tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
	tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
	// tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
	// tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

	tty.c_cc[VTIME] = 100;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
	tty.c_cc[VMIN] = 0;

	cfsetispeed(&tty, B9600);
	cfsetospeed(&tty, B9600);
	int serial_port = open("/dev/ttyUSB0", O_RDWR);
	if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
		printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
		return false;
	}

	close(serial_port);
	return true;
}

void comSend(string command) {
	command = "echo \"" + command + "\" >> /dev/ttyUSB0";
	log << command << endl;
	system(command.c_str());
}






