#pragma once

#define CAM_EVENT_ORBIT 0xbc17e41c
typedef int camEvent_t;

class cCameraEvent {
private:
	void* vtable;
public:
	int m_OrbitX;
	int m_OrbitY;
};

class cCameraTransform {
public:
	float GetYaw();
	float GetYawTarget();
	float GetPitch();
	float GetPitchTarget();

	void SetYaw(float yaw);
	void SetYawTarget(float yaw);
	void SetPitch(float pitch);
	void SetPitchTarget(float pitch);
};

class cCameraController {
public:
	cCameraTransform* GetTransform();
};