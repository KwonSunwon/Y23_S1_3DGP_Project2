#pragma once

class CScene {
public:
	CScene() { }
	~CScene() { }

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	void ReleaseUploadBuffers();

private:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	CShader* m_pShader;

private:
	std::vector<CGameObject*> m_vObjects;
	std::list<CGameObject*>	m_lEffects;

	CFloorObject* m_pFloorObject = NULL;

	CPlayer* m_pPlayer = NULL;

	float m_fElapsedTime = 0.0f;

public:
	void AddObject(CGameObject* object);
	void AddEffect(CGameObject* effect);

	CPlayer* PlayerChange();

	void CheckObjectByObjectCollisions();
	void CheckObjectByBulletCollisions();
	void CheckObjectByPlayerCollisions();
	void CheckPlayerByBulletCollisions();

	virtual void Animate(float fElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

