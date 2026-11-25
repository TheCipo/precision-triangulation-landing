/*
 * Tello.cpp
 *
 *  Created on: Mar 26, 2019
 *      Author: akki
 *  Updated: Active UDP response wait & buffer flush
 */

#include "Tello.h"

static const char* TELLOIPADDRESS = "192.168.10.1";
static const int COMMAND_PORT = 8889;
static const int STATUS_PORT = 8890;
#define MAXBUFFSIZE 1472

using namespace std;

Tello::Tello()
{
	isInitialised = false;
	udpclient = nullptr;
	lastResponse = "";
}

Tello::~Tello()
{
	if (udpclient != nullptr) {
		delete udpclient;
	}
}

void Tello::init()
{
	delay(2000);
	udpclient = new WiFiUDP();
	udpclient->begin(COMMAND_PORT);
	isInitialised = true;

	string response = "";
	while (response != "ok") {
		response = sendCommand("command");
		delay(500);
	}
}

string Tello::sendCommand(string command)
{
	if (!isInitialised) {
		lastResponse = "error";
		return lastResponse;
	}

	// Svuota eventuali pacchetti residui nel buffer UDP prima di inviare un nuovo comando
	while (udpclient->parsePacket() > 0) {
		udpclient->flush();
	}

	udpclient->beginPacket(TELLOIPADDRESS, COMMAND_PORT);
	udpclient->write((const unsigned char*)command.c_str(), command.length());
	udpclient->endPacket();

	char buffer[MAXBUFFSIZE] = {0};
	unsigned long startTime = millis();
	// Timeout di 10s per comandi lunghi (es. takeoff/land), 7s per movimenti standard
	unsigned long timeout = (command == "takeoff" || command == "land") ? 10000 : 7000;

	while (millis() - startTime < timeout) {
		int packetSize = udpclient->parsePacket();
		if (packetSize > 0) {
			int n = udpclient->read(buffer, MAXBUFFSIZE - 1);
			if (n > 0) {
				buffer[n] = '\0';
				string res(buffer);
				// Rimuove eventuali caratteri di fine riga (\r o \n)
				while (!res.empty() && (res.back() == '\r' || res.back() == '\n')) {
					res.pop_back();
				}
				printf("sendCommand: %s -> response=%s\n", command.c_str(), res.c_str());
				lastResponse = res;
				return res;
			}
		}
		delay(10); // Evita di bloccare i task Wi-Fi in background dell'ESP32
	}

	printf("sendCommand: %s -> TIMEOUT\n", command.c_str());
	lastResponse = "timeout";
	return lastResponse;
}

bool Tello::takeoff()
{
	return sendCommand("takeoff") == "ok";
}

bool Tello::land()
{
	return sendCommand("land") == "ok";
}

bool Tello::startVideoStream()
{
	return sendCommand("streamon") == "ok";
}

bool Tello::stopVideoStream()
{
	return sendCommand("streamoff") == "ok";
}

bool Tello::turnOff()
{
	return sendCommand("emergency") == "ok";
}

bool Tello::up(int x)
{
	char buff[10];
	string command = "up " + string(itoa(x, buff, 10));
	return sendCommand(command) == "ok";
}

bool Tello::down(int x)
{
	char buff[10];
	string command = "down " + string(itoa(x, buff, 10));
	return sendCommand(command) == "ok";
}

bool Tello::left(int x)
{
	char buff[10];
	string command = "left " + string(itoa(x, buff, 10));
	return sendCommand(command) == "ok";
}

bool Tello::right(int x)
{
	char buff[10];
	string command = "right " + string(itoa(x, buff, 10));
	return sendCommand(command) == "ok";
}

bool Tello::forward(int x)
{
	char buff[10];
	string command = "forward " + string(itoa(x, buff, 10));
	return sendCommand(command) == "ok";
}

bool Tello::back(int x)
{
	char buff[10];
	string command = "back " + string(itoa(x, buff, 10));
	return sendCommand(command) == "ok";
}

bool Tello::rotate_clockwise(int deg)
{
	char buff[10];
	string command = "cw " + string(itoa(deg, buff, 10));
	return sendCommand(command) == "ok";
}

bool Tello::rotate_anticlockwise(int deg)
{
	char buff[10];
	string command = "ccw " + string(itoa(deg, buff, 10));
	return sendCommand(command) == "ok";
}

bool Tello::flip_front()
{
	return sendCommand("flip f") == "ok";
}

bool Tello::flip_right()
{
	return sendCommand("flip r") == "ok";
}

bool Tello::flip_left()
{
	return sendCommand("flip l") == "ok";
}

bool Tello::flip_back()
{
	return sendCommand("flip b") == "ok";
}

bool Tello::setSpeed(int x)
{
	if ((x < 10) || (x > 100)) {
		return false;
	}
	char buff[10];
	string command = "speed " + string(itoa(x, buff, 10));
	return sendCommand(command) == "ok";
}

bool Tello::curve(int x1, int x2, int y1, int y2, int z1, int z2, int speed)
{
	char buff[10];
	string _x1 = string(itoa(x1, buff, 10));
	string _y1 = string(itoa(x2, buff, 10));
	string _z1 = string(itoa(y1, buff, 10));
	string _x2 = string(itoa(y2, buff, 10));
	string _y2 = string(itoa(z1, buff, 10));
	string _z2 = string(itoa(z2, buff, 10));
	string _speed = string(itoa(speed, buff, 10));
	string command = "curve " + _x1 + " " + _y1 + " " + _z1 + " " + _x2 + " " + _y2 + " " + _z2 + " " + _speed;
	return sendCommand(command) == "ok";
}

bool Tello::go(int x, int y, int z, int speed)
{
	char buff[10];
	string _x = string(itoa(x, buff, 10));
	string _y = string(itoa(y, buff, 10));
	string _z = string(itoa(z, buff, 10));
	string _speed = string(itoa(speed, buff, 10));
	string command = "go " + _x + " " + _y + " " + _z + " " + _speed;
	return sendCommand(command) == "ok";
}

bool Tello::flyto(int x, int y, int z, int speed, int mid)
{
	char buff[10];
	string _x = string(itoa(x, buff, 10));
	string _y = string(itoa(y, buff, 10));
	string _z = string(itoa(z, buff, 10));
	string _speed = string(itoa(speed, buff, 10));
	string _mid = string(itoa(mid, buff, 10));
	string command = "go " + _x + " " + _y + " " + _z + " " + _speed + " m" + _mid;
	return sendCommand(command) == "ok";
}

bool Tello::jump(int x, int y, int z, int speed, int yaw, int mid1, int mid2)
{
	char buff[10];
	string _x = string(itoa(x, buff, 10));
	string _y = string(itoa(y, buff, 10));
	string _z = string(itoa(z, buff, 10));
	string _speed = string(itoa(speed, buff, 10));
	string _yaw = string(itoa(yaw, buff, 10));
	string _mid1 = string(itoa(mid1, buff, 10));
	string _mid2 = string(itoa(mid2, buff, 10));
	string command = "go " + _x + " " + _y + " " + _z + " " + _speed + " " + _yaw + " m" + _mid1 + " m" + _mid2;
	return sendCommand(command) == "ok";
}

bool Tello::sendRCcontrol(int a, int b, int c, int d)
{
	if (!(check_range(a, -100, 100)) || !(check_range(b, -100, 100)) || !(check_range(c, -100, 100)) || !(check_range(d, -100, 100))) {
		return false;
	}
	char buff[10];
	string _a = string(itoa(a, buff, 10));
	string _b = string(itoa(b, buff, 10));
	string _c = string(itoa(c, buff, 10));
	string _d = string(itoa(d, buff, 10));
	string command = "rc " + _a + " " + _b + " " + _c + " " + _d;
	return sendCommand(command) == "ok";
}

bool Tello::changeWifi(string ssid, string password)
{
	string command = "wifi " + ssid + " " + password;
	return sendCommand(command) == "ok";
}

int Tello::getSpeed()
{
	string response = sendCommand("speed?");
	return atoi(response.c_str());
}

int Tello::getBattery()
{
	string response = sendCommand("battery?");
	return atoi(response.c_str());
}

int Tello::getTime()
{
	string response = sendCommand("time?");
	return atoi(response.c_str());
}

int Tello::getHeight()
{
	string response = sendCommand("height?");
	return atoi(response.c_str());
}

int Tello::getTemperature()
{
	string response = sendCommand("temp?");
	return atoi(response.c_str());
}

int Tello::getAltitude()
{
	string response = sendCommand("attitude?");
	return atoi(response.c_str());
}

int Tello::getBaro()
{
	string response = sendCommand("baro?");
	return atoi(response.c_str());
}

int Tello::getAcceleration()
{
	string response = sendCommand("acceleration?");
	return atoi(response.c_str());
}

int Tello::getTof()
{
	string response = sendCommand("tof?");
	return atoi(response.c_str());
}

int Tello::getWifiSnr()
{
	string response = sendCommand("wifi?");
	return atoi(response.c_str());
}

bool Tello::check_range(int x, int min, int max)
{
	return (min <= x && x <= max);
}

bool Tello::mdirection(int x)
{
	char buff[10];
	string command = "mdirection " + string(itoa(x, buff, 10));
	return sendCommand(command) == "ok";
}

bool Tello::mon()
{
	return sendCommand("mon") == "ok";
}

bool Tello::moff()
{
	return sendCommand("moff") == "ok";
}
