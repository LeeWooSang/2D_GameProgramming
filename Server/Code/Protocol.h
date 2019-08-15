#pragma once

#define SERVER_IP "127.0.0.1"

enum NETWORK { PORT = 9000, BUF_SIZE = 1024 };
enum DIRECTION { UP, DOWN, LEFT, RIGHT };

enum WORLD { WIDTH = 300, HEIGHT = 300 };

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