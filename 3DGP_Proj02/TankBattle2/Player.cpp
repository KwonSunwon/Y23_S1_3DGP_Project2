#include "stdafx.h"
#include "Player.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPlayer::CPlayer()
{
}

CPlayer::~CPlayer()
{
	if (m_pCamera) delete m_pCamera;
}

void CPlayer::SetPosition(float x, float y, float z)
{
	m_xmf3Position = XMFLOAT3(x, y, z);

	CGameObject::SetPosition(x, y, z);
}

void CPlayer::SetCameraOffset(XMFLOAT3& xmf3CameraOffset)
{
	m_xmf3CameraOffset = xmf3CameraOffset;
	m_pCamera->SetLookAt(Vector3::Add(m_xmf3Position, m_xmf3CameraOffset), m_xmf3Position, m_xmf3Up);
	m_pCamera->GenerateViewMatrix();
}

void CPlayer::Move(DWORD dwDirection, float fDistance)
{
	if (m_fMovementEnergy <= 0.0f || !m_bActive) return;
	if (dwDirection) {
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		// 터렛이 바라보는 방향을 기준으로 플레이어가 이동하도록 변경
		XMFLOAT3 xmf3Look = ((CTankPlayer*)this)->m_pTurret->GetLook();
		XMFLOAT3 xmf3Right = ((CTankPlayer*)this)->m_pTurret->GetRight();
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, xmf3Look, -fDistance);
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, xmf3Right, -fDistance);
		
		//if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		//if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		Move(xmf3Shift, true);

		m_fMovementEnergy -= fDistance * 0.5f;
	}
}

void CPlayer::Move(XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity) {
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else {
		m_xmf3Position = Vector3::Add(xmf3Shift, m_xmf3Position);
		m_pCamera->Move(xmf3Shift);
	}
}

void CPlayer::Move(float x, float y, float z)
{
	Move(XMFLOAT3(x, y, z), false);
}

void CPlayer::Rotate(float fPitch, float fYaw, float fRoll)
{
	m_pCamera->Rotate(fPitch, fYaw, fRoll);
	if (fPitch != 0.0f) {
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(fPitch));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
	}
	if (fYaw != 0.0f) {
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fYaw));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}
	if (fRoll != 0.0f) {
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(fRoll));
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Up, m_xmf3Look));
	m_xmf3Up = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Look, m_xmf3Right));
}

void CPlayer::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3Up);
	m_xmf3Right = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	m_xmf3Up = Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	m_xmf3Look = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}

void CPlayer::Update(float fTimeElapsed)
{
	m_xmf3OldPosition = m_xmf3Position;
	Move(m_xmf3Velocity, false);

	m_pCamera->Update(this, m_xmf3Position, fTimeElapsed);
	m_pCamera->GenerateViewMatrix();

	XMFLOAT3 xmf3Deceleration = Vector3::Normalize(Vector3::ScalarProduct(m_xmf3Velocity, -1.0f));
	float fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = m_fFriction * fTimeElapsed;
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Deceleration, fDeceleration);
}

void CPlayer::Animate(float fElapsedTime)
{
	OnUpdateTransform();

	CGameObject::Animate(fElapsedTime);
}

void CPlayer::OnUpdateTransform()
{
	m_xmf4x4Transform._11 = m_xmf3Right.x; m_xmf4x4Transform._12 = m_xmf3Right.y; m_xmf4x4Transform._13 = m_xmf3Right.z;
	m_xmf4x4Transform._21 = m_xmf3Up.x; m_xmf4x4Transform._22 = m_xmf3Up.y; m_xmf4x4Transform._23 = m_xmf3Up.z;
	m_xmf4x4Transform._31 = m_xmf3Look.x; m_xmf4x4Transform._32 = m_xmf3Look.y; m_xmf4x4Transform._33 = m_xmf3Look.z;
	m_xmf4x4Transform._41 = m_xmf3Position.x; m_xmf4x4Transform._42 = m_xmf3Position.y; m_xmf4x4Transform._43 = m_xmf3Position.z;
}

void CPlayer::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CGameObject::Render(hDCFrameBuffer, pCamera);
}

void CPlayer::OnCollisionByWall()
{
	m_xmf3Position = m_xmf3OldPosition;
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
CTankPlayer::CTankPlayer()
{
	CCubeMesh* pBodyMesh = new CCubeMesh(6.0f, 2.0f, 6.0f);
	SetMesh(pBodyMesh);
	SetColor(RGB(0, 0, 255));

	m_pTurret = new CGameObject();
	CCubeMesh* pTurretMesh = new CCubeMesh(4.0f, 1.5f, 4.0f);
	m_pTurret->SetMesh(pTurretMesh);
	m_pTurret->SetColor(RGB(255, 0, 0));
	m_pTurret->SetPosition(0.0f, 1.75f, 0.0f);
	SetChild(m_pTurret);

	m_pGun = new CGameObject();
	CCubeMesh* pGunMesh = new CCubeMesh(1.0f, 1.0f, 10.0f);
	m_pGun->SetMesh(pGunMesh);
	m_pGun->SetColor(RGB(255, 0, 255));
	m_pGun->SetPosition(0.0f, 1.25f, 4.0f);
	m_pTurret->SetChild(m_pGun);

	CCubeMesh* pBulletMesh = new CCubeMesh(1.0f, 1.0f, 4.0f);
	for (int i = 0; i < BULLET_COUNT; i++) {
		m_ppBullets[i] = new CBulletObject(m_fBulletEffectiveRange);
		m_ppBullets[i]->SetMesh(pBulletMesh);
		m_ppBullets[i]->SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 1.0f));
		m_ppBullets[i]->SetRotationSpeed(360.0f);
		// 처음 속도 120.0
		m_ppBullets[i]->SetMovingSpeed(60.0f);
		m_ppBullets[i]->SetActive(false);
	}
}

CTankPlayer::~CTankPlayer()
{
	for (int i = 0; i < BULLET_COUNT; i++) if (m_ppBullets[i]) delete m_ppBullets[i];
}

void CTankPlayer::Animate(float fElapsedTime)
{
	CPlayer::Animate(fElapsedTime);
	ComputeWorldTransform(NULL);
	/*수정*/
	UpdateBoundingBox();

	for (int i = 0; i < BULLET_COUNT; i++) {
		if (m_ppBullets[i]->m_bActive) {
			m_ppBullets[i]->Animate(fElapsedTime);
			m_ppBullets[i]->ComputeWorldTransform(NULL);
		}
	}
}

void CTankPlayer::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CPlayer::Render(hDCFrameBuffer, pCamera);

	for (int i = 0; i < BULLET_COUNT; i++) if (m_ppBullets[i]->m_bActive) m_ppBullets[i]->Render(hDCFrameBuffer, pCamera);
}

void CTankPlayer::FireBullet(CGameObject* pLockedObject)
{
	if (!m_nBulletLeft || !m_bActive) return;

	CBulletObject* pBulletObject = NULL;
	for (int i = 0; i < BULLET_COUNT; i++) {
		if (!m_ppBullets[i]->m_bActive) {
			pBulletObject = m_ppBullets[i];
			break;
		}
	}

	if (pBulletObject) {
		XMFLOAT3 xmf3Position = m_pGun->GetPosition();
		XMFLOAT3 xmf3Direction = m_pGun->GetLook();
		XMFLOAT3 xmf3FirePosition = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Direction, 6.0f, false));

		pBulletObject->m_xmf4x4Transform = m_pGun->m_xmf4x4World;

		pBulletObject->SetFirePosition(xmf3FirePosition);
		pBulletObject->SetMovingDirection(xmf3Direction);
		pBulletObject->SetColor(RGB(255, 0, 0));
		pBulletObject->SetActive(true);

		m_nBulletLeft--;

		if (pLockedObject) {
			pBulletObject->m_pLockedObject = pLockedObject;
			pBulletObject->SetColor(RGB(0, 0, 255));
		}
	}
}

void CTankPlayer::RotateGun(float fAngle)
{
	m_fGunAngle += fAngle;
	if (abs(m_fGunAngle) > 1.0f) {
		m_fGunAngle = m_fGunAngle > 0 ? 1.0f : -1.0f;
	}

	// DEBUG
	//OutputDebugString((LPCWSTR)std::to_wstring(m_fGunAngle).c_str());
	//OutputDebugString(L"\n");
	
	m_pGun->m_xmf4x4Transform = Matrix4x4::RotationYawPitchRoll(m_fGunAngle , 0.0f, 0.0f);
	m_pGun->SetPosition(0.0f, 1.25f, 4.0f);
}

void CTankPlayer::SetColors(DWORD body, DWORD turret, DWORD gun)
{
	this->m_dwColor = body;
	m_pTurret->m_dwColor = turret;
	m_pGun->m_dwColor = gun;
}