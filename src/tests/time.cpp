#include "time.h"
#include "../kurnikDeviceTest.h"

bool timeTest(fstream& com) {
	/*
	 * time & alarm format:
	 * TE: time 18:43:10 4 03 1
	 * TE: alarm 16:08
	 * woy 20 = 02,34,	03,12,	18,28,	19,07,	255,255,	255,255,
	 * woy 21 = 02,23,	03,03,	18,38,	19,17,	255,255,	255,255,
	 */

	// test czy jest możliwe ustawienie casu
	system("date \"+setTime %H:%M:%S %u %U %:::z\" >> /dev/ttyUSB0");
	assert(101, "TE: setTime", "TE: setTime", "Fault during setTime",com,"SetTime OK", 30);

	// próba ustawienia niewłaściwego czasu
	comSend("setTime 01:10:20 1 0 +1"); //Parameter 5 out of range!
	assert(102, "Parameter", "Parameter 5 out of range!", "Time not set correctly", com, "Time correct");

	comSend("setTime 24:10:20 1 20 +1"); //Parameter 1 out of range!
	assert(103, "Parameter", "Parameter 1 out of range!", "Time not set correctly", com, "Time correct");

	//ustawienie czasu i kontrola alarmu
	// alarm otwarcie drzwi
	comSend("setTime 01:10:20 1 20 0");
	assert(104, "TE: time", "01:10:20 1 20 +0", "Time not set correctly", com, "Time correct");
	assert(105, "TE: alarm", "02:34", "Alarm open set faulty", com, "Alarm open morning");

	// alarm zamknięcie ten sam tydzień
	comSend("setTime 02:35:20 5 20 +0");
	assert(106, "TE: time", "02:35:20 5 20 +0", "Time not set correctly", com, "Time correct");
	assert(107, "TE: alarm", "19:07", "Alarm open set faulty", com, "Alarm close night this week");
	// alarm zamknięcie nastepny tydzień
	comSend("setTime 19:08:01 7 20 +0");
	assert(108, "TE: alarm", "02:23", "Alarm open set faulty", com, "Alarm close next week");

	// Sprawdzenie zachowania alarmu dla czasu letniego
	// alarm otwarcie drzwi
	comSend("setTime 01:10:20 1 20 +1");
	assert(109, "TE: time", "01:10:20 1 20 +1", "Time not set correctly", com, "Time correct (letni)");
	assert(110, "TE: alarm", "03:34", "Alarm open set faulty", com, "Alarm open morning (letni)");

	// alarm zamknięcie ten sam tydzień
	comSend("setTime 03:35:20 5 20 +1");
	assert(111, "TE: time", "03:35:20 5 20 +1", "Time not set correctly", com, "Time correct");
	assert(112, "TE: alarm", "20:07", "Alarm open set faulty", com, "Alarm close night this week");
	// alarm zamknięcie nastepny tydzień
	comSend("setTime 20:08:01 7 20 +1");
	assert(113, "TE: alarm", "03:23", "Alarm open set faulty", com, "Alarm close next week");

	// alarm otwarcie ale naztawienie czasy między czasem zimowym a letnim - sprawdzenie czy dobrze wyniera czas
	comSend("setTime 02:35:20 5 20 +1");
	assert(114, "TE: alarm", "03:34", "Alarm open set faulty", com, "Alarm close night this week");
	// alarm zamknięcie nastepny tydzień
	comSend("setTime 19:08:01 7 20 +1");
	assert(115, "TE: alarm", "20:07", "Alarm open set faulty", com, "Alarm close next week");


	return true;

}

bool alarmTest(fstream& com) {
	//test alarmu
	// otwieranie zima
	comSend("setTime 02:33:55 1 20 0");
	ignore("TE: alarm", com);
	assert(201, "TE: alarm", "19:07", "Alarm open set faulty", com, "Alarm open night winter this week OK");
	ignore("TE: alarm finish", com);

	// zamykanie zima ten sam tydzień
	comSend("setTime 19:06:55 1 20 0");
	ignore("TE: alarm", com);
	assert(202, "TE: alarm", "02:34", "Alarm open set faulty", com, "Alarm close night winter this week OK");
	ignore("TE: alarm finish", com);

	// zamykanie zima następny tydzień
	comSend("setTime 19:06:55 7 20 0");
	ignore("TE: alarm", com);
	assert(203, "TE: alarm", "02:23", "Alarm open set faulty", com, "Alarm close night winter next week OK");
	ignore("TE: alarm finish", com);

	// otwieranie lato
	comSend("setTime 03:33:55 1 20 1");
	ignore("TE: alarm", com);
	assert(204, "TE: alarm", "20:07", "Alarm open set faulty", com, "Alarm open night this week lato OK");
	ignore("TE: alarm finish", com);

	// zamykanie lato ten sam tydzień
	comSend("setTime 20:06:55 1 20 1");
	ignore("TE: alarm", com);
	assert(205, "TE: alarm", "03:34", "Alarm open set faulty", com, "Alarm close night this week lato OK");
	ignore("TE: alarm finish", com);

	// zamykanie lato ten następny tydzień
	comSend("setTime 20:06:55 7 20 1");
	ignore("TE: alarm", com);
	assert(206, "TE: alarm", "03:23", "Alarm open set faulty", com, "Alarm close night next week lato OK");
	ignore("TE: alarm finish", com);

	return true;
}

bool alarmOffsetTest(fstream& com) {
	// testy z ustawionym offsetem otwieranie czas zimowy
	comSend("setTime 02:33:55 1 20 0");
	ignore("TE: alarm", com);
	comSend("offset 80 30");
	assert(301, "TE: alarm", "03:54", "Alarm + offset open faulty", com, "Alarm + offset otw, zima OK");
	comSend("setTime 03:53:55 1 20 0");
	ignore("TE: alarm", com);
	assert(302, "TE: alarm", "19:37", "Alarm + offset trigger fault", com, "Alarm + offset trigger OK");
	ignore("TE: alarm finish", com);


	// testy z ustawionym offsetem otwieranie czas letni
	comSend("setTime 03:33:55 1 20 1");
	ignore("TE: alarm", com);
	comSend("offset 80 30");
	assert(303, "TE: alarm", "04:54", "Alarm + offset open faulty", com, "Alarm + offset otw, lato");
	comSend("setTime 04:53:55 1 20 1");
	ignore("TE: alarm", com);
	assert(304, "TE: alarm", "20:37", "Alarm + offset trigger fault", com, "Alarm + offset trigger OK");
	ignore("TE: alarm finish", com);

	// testy z ustawionym offsetem zamukanie
	comSend("setTime 17:05:55 1 40 0");
	ignore("TE: alarm", com);
	comSend("offset 70 40");
	assert(305, "TE: alarm", "17:46", "Alarm + offset open faulty", com, "Alarm + offset zam. zima OK");
	comSend("setTime 17:45:55 1 40 0");
	ignore("TE: alarm", com);
	assert(306, "TE: alarm", "05:31", "Alarm + offset trigger fault", com, "Alarm + offset trigger OK");
	ignore("TE: alarm finish", com);

	// testy z ustawionym offsetem zamykanie
	comSend("setTime 18:05:55 1 40 1");
	ignore("TE: alarm", com);
	comSend("offset 70 40");
	assert(307, "TE: alarm", "18:46", "Alarm + offset open faulty", com, "Alarm + offset zam. lato OK");
	comSend("setTime 18:45:55 1 40 1");
	ignore("TE: alarm", com);
	assert(308, "TE: alarm", "06:31", "Alarm + offset trigger fault", com, "Alarm + offset trigger OK");
	ignore("TE: alarm finish", com);

	// testy z ustawionym offsetem zamukanie następny tydzień
	comSend("setTime 15:40:55 1 53 0");
	ignore("TE: alarm", com);
	comSend("offset 10 80");
	assert(309, "TE: alarm", "17:01", "Alarm + offset open faulty", com, "Alarm + offset zam. zima OK");
	comSend("setTime 17:00:55 1 53 0");
	ignore("TE: alarm", com);
	assert(310, "TE: alarm", "06:22", "Alarm + offset trigger fault", com, "Alarm + offset trigger OK");
	ignore("TE: alarm finish", com);

	// testy z ustawionym offsetem zamukanie następny tydzień
	comSend("setTime 15:40:55 7 53 1");
	ignore("TE: alarm", com);
	comSend("offset 10 80");
	assert(311, "TE: alarm", "18:01", "Alarm + offset open faulty", com, "Alarm + offset lato. zima OK");
	comSend("setTime 18:00:55 7 53 1");
	ignore("TE: alarm", com);
	assert(312, "TE: alarm", "07:22", "Alarm + offset trigger fault", com, "Alarm + offset trigger OK");
	ignore("TE: alarm finish", com);

	// add invertion offset

	comSend("setTime 15:40:55 7 53 1");
	ignore("TE: alarm", com);
	comSend("offset -10 -20");
	assert(313, "TE: alarm", "16:21", "Alarm + offset open faulty", com, "Alarm + offset lato. zima OK");
	comSend("setTime 16:20:55 7 53 1");
	ignore("TE: alarm", com);
	assert(314, "TE: alarm", "07:02", "Alarm + offset trigger fault", com, "Alarm + offset trigger OK");
	ignore("TE: alarm finish", com);

	comSend("setTime 15:40:55 7 53 1");
	ignore("TE: alarm", com);
	comSend("offset -20 -50");
	assert(315, "TE: alarm", "15:51", "Alarm + offset open faulty", com, "Alarm + offset lato. zima OK");
	comSend("setTime 15:50:55 7 53 1");
	ignore("TE: alarm", com);
	assert(316, "TE: alarm", "06:52", "Alarm + offset trigger fault", com, "Alarm + offset trigger OK");
	ignore("TE: alarm finish", com);

	return true;
}

void testSelectColumntime(fstream& com) {
	comSend("offset 0 0");
	ignore("TE: setOffset", com);
	comSend("openConfig 0 3");
	ignore("TE: openConfig", com);
	comSend("setTime 01:00:00 1 30 0");
	assert(400, "TE: alarm", "02:34", "alarm for col 0 fault", com, "Alarm for col 0 ok");
	comSend("openConfig 1 3");
	assert(401, "TE: alarm", "03:14", "alarm for col 1 fault", com, "Alarm for col 1 ok");
	comSend("setTime 14:00:00 1 30 0");
	ignore("TE: alarm", com);
	comSend("openConfig 1 2");
	assert(402, "TE: alarm", "18:47", "alarm for col 2 fault", com, "Alarm for col 2 ok");
	comSend("openConfig 1 3");
	assert(403, "TE: alarm", "19:26", "alarm for col 3 fault", com, "Alarm for col 3 ok");
	// test realnego wywołania alarmu
	comSend("openConfig 1 2");
	ignore("TE: openConfig", com);
	comSend("setTime 18:46:55 1 30 0");
	ignore("TE: setTime", com);
	assert(404, "TE: alarm", "03:14", "alarm test with column set fault", com, "Alarm test column set ok");
}
/*
	 * time & alarm format:
	 * TE: time 18:43:10 4 03 1
	 * TE: alarm 16:08
	 * woy 20 = 02,34,	03,12,	18,28,	19,07,	255,255,	255,255,
	 * woy 21 = 02,23,	03,03,	18,38,	19,17,	255,255,	255,255,
	 */
