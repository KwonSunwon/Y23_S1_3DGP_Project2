#include "stdafx.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Player.h"
#include "Shader.h"

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	//CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CScene::ReleaseObjects()
{
	if (CExplosiveObject::m_pExplosionMesh)
		CExplosiveObject::m_pExplosionMesh->Release();

	if (m_pd3dGraphicsRootSignature)
		m_pd3dGraphicsRootSignature->Release();

	if (!m_vObjects.empty()) {
		for (auto& object : m_vObjects)
			if (object)
				object->Release();
		m_vObjects.clear();
	}

	ReleaseShaderVariables();

	//if (m_pLights) delete[] m_pLights;
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[3];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 2; //GameObject
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::AddObject(CGameObject* object)
{
	m_vObjects.push_back(object);
}

void CScene::AddEffect(CGameObject* effect)
{
	m_lEffects.push_back(effect);
}

CPlayer* CScene::PlayerChange()
{
	CPlayer* temp = m_pPlayer;
	m_pPlayer = (CPlayer*)m_vObjects[0];
	m_pPlayer->SetCamera(temp->GetCamera());
	m_pPlayer->SetCameraOffset(temp->m_xmf3CameraOffset);
	m_vObjects[0] = temp;
	((CPlayer*)m_vObjects[0])->SetCamera(nullptr);
	m_pPlayer->m_pCamera->m_startPosition = temp->GetPosition();
	m_pPlayer->m_bActive = false;
	m_pPlayer->m_fMovementEnergy = 20.0f;
	m_pPlayer->m_nBulletLeft = 3;
	return m_pPlayer;
}

void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID) {
	case WM_KEYDOWN:
		switch (wParam) {
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		{
			/*CExplosiveObject* pExplosiveObject = (CExplosiveObject*)m_ppObjects[int(wParam - '1')];
			pExplosiveObject->m_bBlowingUp = true;
			break;*/
		}
		case 'A':
			/*for (int i = 0; i < m_nObjects; i++) {
				CExplosiveObject* pExplosiveObject = (CExplosiveObject*)m_ppObjects[i];
				pExplosiveObject->m_bBlowingUp = true;
			}*/
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void CScene::CheckObjectByObjectCollisions()
{
	//for (auto& object : m_vObjects)
	//	object->m_pObjectCollided = NULL;
	//for (auto& object : m_vObjects) {
	//	for (int j = (i + 1); j < m_nObjects; j++) {
	//		if (object->m_xmOOBB.Intersects(object->m_xmOOBB)) {
	//			object->m_pObjectCollided = object;
	//			object->m_pObjectCollided = object;
	//		}
	//	}
	//}
	//for (auto& object : m_vObjects) {
	//	if (object->m_pObjectCollided) {
	//		XMFLOAT3 xmf3MovingDirection = object->m_xmf3MovingDirection;
	//		float fMovingSpeed = object->m_fMovingSpeed;
	//		object->m_xmf3MovingDirection = object->m_pObjectCollided->m_xmf3MovingDirection;
	//		object->m_fMovingSpeed = object->m_pObjectCollided->m_fMovingSpeed;
	//		object->m_pObjectCollided->m_xmf3MovingDirection = xmf3MovingDirection;
	//		object->m_pObjectCollided->m_fMovingSpeed = fMovingSpeed;
	//		object->m_pObjectCollided->m_pObjectCollided = NULL;
	//		object->m_pObjectCollided = NULL;
	//	}
	//}
}

void CScene::CheckObjectByBulletCollisions()
{
	//CBulletObject** ppBullets = ((CTankPlayer*)m_pPlayer)->m_ppBullets;
	for (auto& object : m_vObjects) {
		for (auto& bullet : ((CTankPlayer*)m_pPlayer)->m_ppBullets) {
			if (bullet->GetPosition().y <= 0.0f) {
				bullet->Reset();
				continue;
			}
			if (bullet->m_bActive && object->m_xmOOBB.Intersects(bullet->m_xmOOBB)) {
				//CExplosiveObject* pExplosiveObject = (CExplosiveObject*)object;
				//pExplosiveObject->m_bBlowingUp = true;
				//bullet->Reset();

				BoundingOrientedBox collisionOOBB = object->m_xmOOBB;
				// oobb 의 회전각도 
				XMVECTOR quat = XMLoadFloat4(&collisionOOBB.Orientation);
				// oobb 의 회전각도를 이용하여 forward, right, up 벡터를 구함
				XMVECTOR forward = (XMVector3Rotate(XMVectorSet(0, 0, 1, 0), quat));
				XMVECTOR right = (XMVector3Rotate(XMVectorSet(1, 0, 0, 0), quat));
				XMVECTOR up = (XMVector3Rotate(XMVectorSet(0, 1, 0, 0), quat));

				// oobb 의 너비, 높이, 깊이
				float width = collisionOOBB.Extents.x;
				float height = collisionOOBB.Extents.y;
				float depth = collisionOOBB.Extents.z;

				// oobb 의 중심점
				XMVECTOR center = XMLoadFloat3(&collisionOOBB.Center);

				XMVECTOR plane[6];
				plane[0] = XMPlaneFromPointNormal(center - forward * depth, -forward);
				plane[1] = XMPlaneFromPointNormal(center + forward * depth, forward);
				plane[2] = XMPlaneFromPointNormal(center - right * width, -right);
				plane[3] = XMPlaneFromPointNormal(center + right * width, right);
				plane[4] = XMPlaneFromPointNormal(center - up * height, -up);
				plane[5] = XMPlaneFromPointNormal(center + up * height, up);

				BoundingOrientedBox bulletOOBB = bullet->m_xmOOBB;
				for (int i = 0; i < 6; ++i) {
					if (bulletOOBB.Intersects(plane[i]) == INTERSECTING) {
						XMVECTOR planeNormal = { plane[i].m128_f32[0], plane[i].m128_f32[1], plane[i].m128_f32[2] };
						XMVECTOR bulletDirection = XMLoadFloat3(&bullet->m_xmf3MovingDirection);
						XMVECTOR reflectionDirection = XMVector3Reflect(bulletDirection, planeNormal);
						XMStoreFloat3(&bullet->m_xmf3MovingDirection, reflectionDirection);
						break;
					}
				}

				// 벽에 총알이 충돌하면 반사각을 계산해 튕겨져 나오는건 구현완료되었는데
				// 애매한 위치에 총알이 충돌하거나 정상적으로 작동해야할 각도인데도 이상한 방향으로 반사각이 계산되는 경우가 있음
				// 또한 튕긴 후의 방향이 벽을 통과하는 방향이 되어 이상하게 날아가는 경우도 발생
				// 5.2 7:49
				// 총알의 속도를 절반으로 해서 테스트를 진행해 봤는데 기존의 높은 속도가 충돌 판정을 하는 데 있어
				// 제대로된 결과를 주지 못한게 아닌가 하는 생각이 들었음.
				// 그리고 속도를 낮췄음에도 불구하고 여전히 이상한 방향으로 튕겨져 나가는 경우가 있음(모서리에 맞추는 경우)
				// 면에 정확히 맞는 경우에 이상한 방향으로 튕겨져 나가는 경우는 줄어든거 같음
			}

		}
	}
}

void CScene::CheckObjectByPlayerCollisions()
{
	for (auto& object : m_vObjects) {
		if (object->m_xmOOBB.Intersects(m_pPlayer->m_xmOOBB))
			m_pPlayer->OnCollisionByWall();
	}
}

void CScene::CheckPlayerByBulletCollisions()
{
	CTankPlayer* pPlayer = (CTankPlayer*)m_vObjects[0];
	for (auto& bullet : ((CTankPlayer*)m_pPlayer)->m_ppBullets) {
		// span으로 자신의 모든 메쉬의 바운딩 박스 넘겨주는 함수 만들어서 주는게 좋을듯?
		// 아예 object가 bb 관리하도록 변경?
		//if (pPlayer->m_pMesh) {
		for (int j = 0; j < 3; ++j) {
			if (bullet->m_bActive && bullet->m_xmOOBB.Intersects(pPlayer->m_pMesh->GetBoundingBox())) {
				// 총알이 날아간 거리만큼 점수 획득
				m_pPlayer->m_nScore += (int)bullet->m_fMovingDistance * 0.5;
				bullet->Reset();

				// 타격 이펙트 발생(ExplosiveObject 이용)
				CExplosiveObject* pExplosiveObject = new CExplosiveObject();
				pExplosiveObject->SetPosition(bullet->GetPosition());
				pExplosiveObject->m_bBlowingUp = true;
				AddEffect(pExplosiveObject);

				return;
			}
		}


	}
}

void CScene::Animate(float fElapsedTime)
{
	//m_pFloorObject->Animate(fElapsedTime);
	//m_pFloorObject->ComputeWorldTransform(NULL);

	for (auto& object : m_vObjects) {
		object->Animate(fElapsedTime);
		object->ComputeWorldTransform(NULL);
		object->UpdateBoundingBox();
	}

	std::vector<CGameObject*> vRemoveList;
	for (auto effect : m_lEffects) {
		effect->Animate(fElapsedTime);
		if (!((CExplosiveObject*)effect)->m_bBlowingUp)
			vRemoveList.push_back(effect);
	}
	for (auto effect : vRemoveList) {
		m_lEffects.remove(effect);
		delete effect;
	}

	CheckObjectByObjectCollisions();
	CheckObjectByBulletCollisions();

	CheckObjectByPlayerCollisions();
	CheckPlayerByBulletCollisions();
}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

	for (auto& object : m_vObjects) {
		if (object) {
			object->Animate(m_fElapsedTime, NULL);
			object->UpdateTransform(NULL);
			object->Render(pd3dCommandList, pCamera);
		}
	}

	int playerIndex = (m_nActivePlayer + 1) % 2;
	m_pPlayers[playerIndex]->Animate(m_fElapsedTime, NULL);
	m_pPlayers[playerIndex]->UpdateTransform(NULL);
	m_pPlayers[playerIndex]->Render(pd3dCommandList, pCamera);
}
