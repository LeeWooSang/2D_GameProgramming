#include "Camera.h"
#include "../GameObject/GameObject.h"
#include "../Network/Network.h"

INIT_INSTACNE(Camera)

Camera::Camera()
	: m_Target(nullptr)
{
	m_ViewPort = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	m_ProjectionMatrix = Matrix4x4::Identity();
}

Camera::~Camera()
{
	cout << "카메라 - 소멸자" << endl;
}

bool Camera::Initialize()
{
	GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);

	return true;
}

void Camera::Update(float elapsedTime)
{
	if (m_Target)
	{
		XMFLOAT2 pos = m_Target->GetWorldPosition();
		m_WorldPosition.x -= pos.x * elapsedTime;
		m_WorldPosition.y -= pos.x * elapsedTime;

		//float size = 0.5f;

		//float leftArea = (-FRAME_BUFFER_WIDTH * 0.5) * size;
		//float topArea = (FRAME_BUFFER_HEIGHT * 0.5f) * size;
		//float rightArea = (FRAME_BUFFER_WIDTH * 0.5f) * size;
		//float bottomArea = (-FRAME_BUFFER_HEIGHT * 0.5f) * size;

		////float leftArea = (-WORLD::WIDTH * 0.5f) * size;
		////float topArea = (WORLD::HEIGHT * 0.5f) * size;
		////float rightArea = (WORLD::WIDTH * 0.5f) * size;
		////float bottomArea = (-WORLD::HEIGHT * 0.5f) * size;

		//if (pos.x <= leftArea)
		//	pos.x = leftArea;

		//else if (pos.x >= rightArea)
		//	pos.x = rightArea;

		//if (pos.y >= topArea)
		//	pos.y = topArea;

		//else if (pos.y <= bottomArea)
		//	pos.y = bottomArea;

		//m_WorldPosition = pos;
	}
}

void Camera::GenerateProjectionMatrix(float nearPlaneDistance, float farPlaneDistance, float aspectRatio, float fOVAngle)
{
	m_ProjectionMatrix = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fOVAngle), aspectRatio, nearPlaneDistance, farPlaneDistance);
}

void Camera::SetTarget(GameObject* target)
{
	if (m_Target)
		m_Target = nullptr;

	m_Target = target;
	m_WorldPosition = m_Target->GetWorldPosition();
}
