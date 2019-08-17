#pragma once
#include "../GameObject.h"

class Player : public GameObject
{
public:
	Player();
	virtual ~Player();

	virtual bool Initialize() override;
	virtual void Update(float) override;
	virtual void Render() override;
	virtual void Release() override;

	void ProcessAnimation(char, float);

private:
	enum ANIMATION
	{
		IDLE = 0x0000,
		BREAK = 0x0001,
		DIE = 0x0002,
		RAISINGBLOW = 0x0004
	};

	enum ANIMATION_FRAME
	{
		IDLE_FRAME = 6,
		BREAK_FRAME = 29,
		DIE_FRAME = 58,
		RAISINGBLOW_FRAME = 13
	};

	char m_PrevAnimation;
	string m_AnimationName;
	float m_Frame;

	unordered_map<ANIMATION, ANIMATION_FRAME> m_AnimationMap;
};