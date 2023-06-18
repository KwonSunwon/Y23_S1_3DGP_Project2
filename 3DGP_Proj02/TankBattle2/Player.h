#pragma once

#include "GameObject.h"
#include "Camera.h"

class CPlayer : public CGameObject
{
public:
	CPlayer();
	virtual ~CPlayer();

public:
	XMFLOAT3					m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3					m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	XMFLOAT3				    m_xmf3OldPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);

	XMFLOAT3					m_xmf3CameraOffset = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	float						m_fFriction = 125.0f;

	float           			m_fPitch = 0.0f;
	float           			m_fYaw = 0.0f;
	float           			m_fRoll = 0.0f;

	CCamera*					m_pCamera = NULL;

	float m_fMovementEnergy = 20.0f;
	int m_nScore = 0;
	int m_nBulletLeft = 3;

public:
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	void LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);

	void Move(DWORD dwDirection, float fDistance);
	void Move(XMFLOAT3& xmf3Shift, bool bUpdateVelocity);
	void Move(float x, float y, float z);

	void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);

	void SetCameraOffset(XMFLOAT3& xmf3CameraOffset);

	void Update(float fTimeElapsed = 0.016f);

	virtual void OnUpdateTransform();
	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);

	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
	CCamera* GetCamera() { return(m_pCamera); }

	void OnCollisionByWall();
};

#define BULLET_COUNT 3

class CTankPlayer : public CPlayer
{
public:
	CTankPlayer();
	virtual ~CTankPlayer();

	CGameObject*				m_pTurret = NULL;
	CGameObject*				m_pGun = NULL;

	float						m_fBulletEffectiveRange = 150.0f;
	//CBulletObject*				m_ppBullets[BULLETS];
	array<CBulletObject*, BULLET_COUNT> m_ppBullets;
	
	// 포신의 각도가 일정 이상 변하지 않도록 수정
	float m_fGunAngle = 0.0f;

	void RotateTurret(float fAngle) { if (!m_bActive) return; m_pTurret->Rotate(0.0f, fAngle, 0.0f); }
	void RotateGun(float fAngle);

	void FireBullet(CGameObject* pLockedObject = NULL);

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);

	void SetColors(DWORD body, DWORD turret, DWORD gun);
};
