#include "stdafx.h"
#include "Camera.h"
#include "Mesh.h"
#include "Player.h"

CCamera::CCamera(CCamera* pCamera)
{
	if (pCamera)
		*this = *pCamera;
}

void CCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
	//	XMMATRIX xmmtxProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
	//	XMStoreFloat4x4(&m_xmf4x4Projection, xmmtxProjection);
}

void CCamera::GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up)
{
	m_xmf3Position = xmf3Position;
	m_xmf3LookAtWorld = xmf3LookAt;
	m_xmf3Up = xmf3Up;

	GenerateViewMatrix();
}

void CCamera::GenerateViewMatrix()
{
	m_xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, m_xmf3LookAtWorld, m_xmf3Up);
}

void CCamera::RegenerateViewMatrix()
{
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);

	m_xmf4x4View._11 = m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x; m_xmf4x4View._13 = m_xmf3Look.x;
	m_xmf4x4View._21 = m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y; m_xmf4x4View._23 = m_xmf3Look.y;
	m_xmf4x4View._31 = m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z; m_xmf4x4View._33 = m_xmf3Look.z;
	m_xmf4x4View._41 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Right);
	m_xmf4x4View._42 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Up);
	m_xmf4x4View._43 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Look);

	GenerateFrustum();
}

void CCamera::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(VS_CB_CAMERA_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbCamera = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbCamera->Map(0, NULL, (void**)&m_pcbMappedCamera);
}

void CCamera::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4View;
	XMStoreFloat4x4(&xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	::memcpy(&m_pcbMappedCamera->m_xmf4x4View, &xmf4x4View, sizeof(XMFLOAT4X4));

	XMFLOAT4X4 xmf4x4Projection;
	XMStoreFloat4x4(&xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));
	::memcpy(&m_pcbMappedCamera->m_xmf4x4Projection, &xmf4x4Projection, sizeof(XMFLOAT4X4));

	::memcpy(&m_pcbMappedCamera->m_xmf3Position, &m_xmf3Position, sizeof(XMFLOAT3));

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbCamera->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dGpuVirtualAddress);
}

void CCamera::ReleaseShaderVariables()
{
	if (m_pd3dcbCamera)
	{
		m_pd3dcbCamera->Unmap(0, NULL);
		m_pd3dcbCamera->Release();
	}
}

void CCamera::SetLookAt(XMFLOAT3& xmf3Position, XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	m_xmf3Position = xmf3Position;
	m_xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3Up);
	m_xmf3Right = Vector3::Normalize(XMFLOAT3(m_xmf4x4View._11, m_xmf4x4View._21, m_xmf4x4View._31));
	m_xmf3Up = Vector3::Normalize(XMFLOAT3(m_xmf4x4View._12, m_xmf4x4View._22, m_xmf4x4View._32));
	m_xmf3Look = Vector3::Normalize(XMFLOAT3(m_xmf4x4View._13, m_xmf4x4View._23, m_xmf4x4View._33));
}

void CCamera::SetLookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3Up);
	m_xmf3Right = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	m_xmf3Up = Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	m_xmf3Look = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}

void CCamera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
}

void CCamera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_d3dScissorRect.left = xLeft;
	m_d3dScissorRect.top = yTop;
	m_d3dScissorRect.right = xRight;
	m_d3dScissorRect.bottom = yBottom;
}

void CCamera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}

void CCamera::GenerateFrustum()
{
	m_xmFrustum.CreateFromMatrix(m_xmFrustum, XMLoadFloat4x4(&m_xmf4x4Projection));
	XMMATRIX xmmtxInversView = XMMatrixInverse(NULL, XMLoadFloat4x4(&m_xmf4x4View));
	m_xmFrustum.Transform(m_xmFrustum, xmmtxInversView);
}

bool CCamera::IsInFrustum(BoundingOrientedBox& xmBoundingBox)
{
	return m_xmFrustum.Intersects(xmBoundingBox);
}

void CCamera::Move(XMFLOAT3& xmf3Shift)
{
	m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
}

void CCamera::Move(float x, float y, float z)
{
	Move(XMFLOAT3(x, y, z));
}

void CCamera::Rotate(float fPitch, float fYaw, float fRoll)
{
	if (fPitch != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(fPitch));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
	}
	if (fYaw != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fYaw));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}
	if (fRoll != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(fRoll));
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}
}

void CCamera::Update(CPlayer* pPlayer, XMFLOAT3& xmf3LookAt, float fTimeElapsed)
{
	// 플레이어가 활성화 된 경우 장상적인 카메라 업데이트 수행
	// 비활성화 상태인 경우 턴 전환 카메라 워크 수행
	if (!pPlayer->GetActive()) {
		FXMVECTOR p1 = XMLoadFloat3(&m_startPosition);
		FXMVECTOR p4 = XMLoadFloat3(&pPlayer->GetPosition());
		FXMVECTOR p2 = XMVector3Transform(p1, XMMatrixTranslation(0.0f, 10.0f, 0.0f));
		FXMVECTOR p3 = XMVector3Transform(p4, XMMatrixTranslation(0.0f, 10.0f, 0.0f));
		XMVECTOR position = XMVectorCatmullRom(p1, p2, p3, p4, m_time);
		m_time += fTimeElapsed * 0.5f;
		XMStoreFloat3(&m_xmf3Position, position);
		// 다음 차례인 플레이어를 보면서 이동
		SetLookAt(pPlayer->m_xmf3Position, pPlayer->m_xmf3Up);

		// 카메라가 다음 차례 플레이어의 위치에 도달하면 다시 플레이어 활성화
		if (m_time >= 1.0f) {
			m_time = 0.0f;
			pPlayer->SetActive(true);
		}
		return;
	}

	// 카메라가 플레이어의 터렛의 위치를 바라보도록 설정
	XMFLOAT3 xmf3TurretLook = ((CTankPlayer*)pPlayer)->m_pTurret->GetLook();
	XMFLOAT3 xmf3TurretUp = ((CTankPlayer*)pPlayer)->m_pTurret->GetUp();
	XMFLOAT3 xmf3TurretRight = ((CTankPlayer*)pPlayer)->m_pTurret->GetRight();

	XMFLOAT4X4 mtxRotate = Matrix4x4::Identity();
	/*mtxRotate._11 = pPlayer->m_xmf3Right.x; mtxRotate._21 = pPlayer->m_xmf3Up.x; mtxRotate._31 = pPlayer->m_xmf3Look.x;
	mtxRotate._12 = pPlayer->m_xmf3Right.y; mtxRotate._22 = pPlayer->m_xmf3Up.y; mtxRotate._32 = pPlayer->m_xmf3Look.y;
	mtxRotate._13 = pPlayer->m_xmf3Right.z; mtxRotate._23 = pPlayer->m_xmf3Up.z; mtxRotate._33 = pPlayer->m_xmf3Look.z;*/
	mtxRotate._11 = xmf3TurretRight.x; mtxRotate._21 = xmf3TurretUp.x; mtxRotate._31 = xmf3TurretLook.x;
	mtxRotate._12 = xmf3TurretRight.y; mtxRotate._22 = xmf3TurretUp.y; mtxRotate._32 = xmf3TurretLook.y;
	mtxRotate._13 = xmf3TurretRight.z; mtxRotate._23 = xmf3TurretUp.z; mtxRotate._33 = xmf3TurretLook.z;

	XMFLOAT3 xmf3Offset = Vector3::TransformCoord(pPlayer->m_xmf3CameraOffset, mtxRotate);
	XMFLOAT3 xmf3Position = Vector3::Add(pPlayer->m_xmf3Position, xmf3Offset);
	XMFLOAT3 xmf3Direction = Vector3::Subtract(xmf3Position, m_xmf3Position);
	float fLength = Vector3::Length(xmf3Direction);
	xmf3Direction = Vector3::Normalize(xmf3Direction);

	float fTimeLagScale = fTimeElapsed * (1.0f / 0.25f);
	float fDistance = fLength * fTimeLagScale;
	if (fDistance > fLength) fDistance = fLength;
	if (fLength < 0.01f) fDistance = fLength;
	if (fDistance > 0)
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Direction, fDistance);
		SetLookAt(pPlayer->m_xmf3Position, pPlayer->m_xmf3Up);
	}
}