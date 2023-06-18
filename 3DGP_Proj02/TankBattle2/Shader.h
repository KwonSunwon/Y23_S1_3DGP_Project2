#pragma once

class CShader {
public:
	CShader() {};
	virtual ~CShader();

	D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	D3D12_RASTERIZER_DESC CreateRasterizerState();
	D3D12_BLEND_DESC CreateBlendState();
	D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	D3D12_SHADER_BYTECODE ReadCompiledShaderFromFile(WCHAR* pszFileName, ID3DBlob** ppd3dShaderBlob);

	void CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	
	void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, CMaterialColors* pMaterialColors);

	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState = 0);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);

protected:
	ID3DBlob* m_pd3dVertexShaderBlob = NULL;
	ID3DBlob* m_pd3dPixelShaderBlob = NULL;

	int m_nPipelineStates = 0;
	ID3D12PipelineState** m_ppd3dPipelineStates = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_d3dPipelineStateDesc;
};

