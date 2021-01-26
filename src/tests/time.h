#ifndef TESTS_TIME_H_
#define TESTS_TIME_H_

#include <fstream>


using namespace std;

bool timeTest(fstream& com);
bool alarmTest(fstream& com);
bool alarmOffsetTest(fstream& com);
void testSelectColumntime(fstream& com);
void testSetOwnTimes(fstream& com);

#endif /* TESTS_TIME_H_ */
