#pragma once

#define SERVER_IP "127.0.0.1"

enum NETWORK { PORT = 9000, BUF_SIZE = 1024 };

enum DIRECTION 
{ 
	IDLE = 0x0000, 
	UP = 0x0001, 
	DOWN = 0x0002, 
	RIGHT = 0x0004, 
	LEFT = 0x0008, 

	UP_RIGHT = 0x0005,
	UP_LEFT = 0x0009,
	DOWN_RIGHT = 0x0006,
	DOWN_LEFT = 0x000A
};

enum WORLD { WIDTH = 1, HEIGHT = 1 };

enum CS_PACKET
{
	CS_LOGIN,
	CS_MOVE
};

enum SC_PACKET
{
	SC_LOGIN_OK,
	SC_LOGIN_FAIL,
	SC_ADD_OBJECT,
	SC_POSITION,
	SC_REMOVE_OBJECT
};

struct CS_Packet_Login
{
	char size;
	char type;
};

struct CS_Packet_Move
{
	char	size;
	char	type;
	char	direction;
};

struct SC_Packet_Login_OK
{
	char size;
	char type;
	int id;
	float x;
	float y;
	short HP;
	unsigned char LEVEL;
	unsigned int EXP;
	unsigned char AttackPower;
};

struct SC_Packet_Login_Fail
{
	char size;
	char type;
};

struct SC_Packet_Add_Object
{
	char size;
	char type;
	int id;
	float x;
	float y;
	int HP;
	unsigned int EXP;
};

struct SC_Packet_Position
{
	char size;
	char type;
	int id;
	float x;
	float y;
};

struct SC_Packet_Remove_Object
{
	char size;
	char type;
	int id;
};