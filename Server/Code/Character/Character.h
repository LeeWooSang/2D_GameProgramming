#pragma once
#include "../Defines.h"
#include "../IOCP/IOCP.h"

enum CHARACTER_STATE { LIVE, DIE, SLEEPING };

class Character
{
public:
	Character();
	virtual ~Character();

	virtual void Move(char, float&, float&);

public:
	OverEx& GetOverEx() { return m_OverEx; }

	float GetX() const { return m_X; }
	void SetX(float x) { m_X = x; }

	float GetY() const { return m_Y; }
	void SetY(float y) { m_Y = y; }

	mutex& GetViewListMtx() { return m_ViewListMtx; }
	unordered_set<int>& GetViewList() { return m_ViewList; }

	short GetHp()	const { return m_Hp; }
	void SetHp(short hp) { m_Hp = hp; }

	unsigned char GetAttackPower()	const { return m_AttackPower; }
	void SetAttackPower(unsigned char power) { m_AttackPower = power; }

	unsigned char GetLevel()	const { return m_Level; }
	void SetLevel(unsigned char level) { m_Level = level; }

	unsigned int GetExp()	const { return m_Exp; }
	void SetExp(unsigned int exe) { m_Exp = exe; }

	virtual short GetMaxHP();

protected:
	OverEx	m_OverEx;
	float m_X;
	float m_Y;

	mutex m_ViewListMtx;
	unordered_set<int> m_ViewList;

	short m_Hp;
	unsigned char m_AttackPower;

	unsigned char	m_Level;
	unsigned int		m_Exp;
};

class Player : public Character
{
public:
	Player();
	virtual ~Player();

	void ClearPlayerInfo();

	const SOCKET& GetSocket() const { return m_Socket; }
	void SetSocket(SOCKET& sock) { m_Socket = sock; }

	char* GetPacketBuf() { return m_PacketBuf; }
	
	int GetPrevSize()	const { return m_PrevSize; }
	void SetPrevSize(int size) { m_PrevSize = size; }

	void SetSendBytes(int bytes) { m_SendBytes = bytes; }

	bool GetIsConnect() const { return m_IsConnect; }
	void SetIsConnect(bool value) { m_IsConnect = value; }

private:
	SOCKET	m_Socket;
	char			m_PacketBuf[BUF_SIZE];
	int				m_PrevSize;
	int				m_SendBytes;
	bool			m_IsConnect;
};

