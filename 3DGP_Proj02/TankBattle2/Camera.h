#pragma once

struct VS_CB_CAMERA_INFO {
	XMFLOAT4X4						m_xmf4x4View;
	XMFLOAT4X4						m_xmf4x4Projection;
	XMFLOAT3						m_xmf3Position;
};

class CPlayer;

class CCamera {
public:
	CCamera() { }
	CCamera(CCamera* pCamera);
	~CCamera() { }

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

protected:
	XMFLOAT3 m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	float m_fPitch = 0.0f;
	float m_fRoll = 0.0f;
	float m_fYaw = 0.0f;

	XMFLOAT3 m_xmf3LookAtWorld = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3Offset = XMFLOAT3(0.0f, 0.0f, 0.0f);

	XMFLOAT4X4 m_xmf4x4View = Matrix4x4::Identity();
	XMFLOAT4X4 m_xmf4x4Projection = Matrix4x4::Identity();

	D3D12_VIEWPORT m_d3dViewport = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	D3D12_RECT m_d3dScissorRect = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };

	CPlayer* m_pPlayer = NULL;

	ID3D12Resource* m_pd3dcbCamera = NULL;
	VS_CB_CAMERA_INFO* m_pcbMappedCamera = NULL;

	BoundingFrustum m_xmFrustum = BoundingFrustum();

	// 턴 변경 카메라 워크를 위한 변수
	XMFLOAT3 m_startPosition = XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	float m_time = 0.0f;

public:
	void SetFOVAngle(float fFOVAngle);

	void GenerateViewMatrix();
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up);
	void RegenerateViewMatrix();

	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);

	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);

	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList);

	XMFLOAT4X4 GetViewMatrix() { return m_xmf4x4View; }
	XMFLOAT4X4 GetProjectionMatrix() { return m_xmf4x4Projection; }
	D3D12_VIEWPORT GetViewport() { return m_d3dViewport; }
	D3D12_RECT GetScissorRect() { return m_d3dScissorRect; }

	void SetLookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);
	void SetLookAt(XMFLOAT3& vPosition, XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up);

	void Move(XMFLOAT3& xmf3Shift);
	void Move(float x, float y, float z);
	void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
	void Update(CPlayer* pPlayer, XMFLOAT3& xmf3LookAt, float fTimeElapsed = 0.016f);

	void GenerateFrustum();
	bool IsInFrustum(BoundingOrientedBox& xmBoundingBox);
};
