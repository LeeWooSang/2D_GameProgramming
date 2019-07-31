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

private:
	float m_FrameTime;
};