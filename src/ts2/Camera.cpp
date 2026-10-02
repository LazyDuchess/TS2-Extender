#include "ts2/Camera.h"
#define PITCH_OFFSET 0x8C
#if TS2_LC
#define TF_OFFSET -0x388
#else
#define TF_OFFSET -0x37C
#endif

float cCameraTransform::GetYaw() {
	return (*(float*)(this));
}

float cCameraTransform::GetYawTarget() {
	return (*(float*)(this + 0x4));
}

float cCameraTransform::GetPitch() {
	return (*(float*)(this + PITCH_OFFSET));
}

float cCameraTransform::GetPitchTarget() {
	return (*(float*)(this + PITCH_OFFSET + 0x4));
}

void cCameraTransform::SetYaw(float yaw) {
	(*(float*)(this)) = yaw;
}

void cCameraTransform::SetYawTarget(float yaw) {
	(*(float*)(this + 0x4)) = yaw;
}

void cCameraTransform::SetPitch(float pitch) {
	(*(float*)(this + PITCH_OFFSET)) = pitch;
}

void cCameraTransform::SetPitchTarget(float pitch) {
	(*(float*)(this + PITCH_OFFSET + 0x4)) = pitch;
}

cCameraTransform* cCameraController::GetTransform() {
	return (cCameraTransform*)(this + TF_OFFSET);
}