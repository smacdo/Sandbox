#pragma once

#include <SimpleMath.h>
#include <string>
#include <vector>
#include "IInitializable.h"

#include <wrl\wrappers\corewrappers.h>      // ComPtr
#include <wrl\client.h>
#include <memory>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
class Texture;

// TODO: TERRIBLE TERRIBLE
// TODO: Remove loading of texture in this class, pass in Texture object.
class Model : public IInitializable
{
public:
	Model();
    Model(const Model&) = delete;
	virtual ~Model();

    Model& operator =(const Model&) = delete;

	void Initialize(ID3D11Device* pDevice,
                    const std::wstring& modelFile,
                    const std::wstring& textureFile);
	void Render(ID3D11DeviceContext* pContext);

    const int IndexCount() const { return mIndexCount; }
    const int VertexCount() const { return mVertexCount; }
    ID3D11ShaderResourceView * GetTexture();

    DirectX::SimpleMath::Vector3 Position() const { return mPosition; }
    void SetPosition(const DirectX::SimpleMath::Vector3& position) { mPosition = position; }

    DirectX::SimpleMath::Vector4 Color() const { return mColor; }
    void SetColor(const DirectX::SimpleMath::Vector4& color) { mColor = color; }

    bool Enabled() const { return mEnabled; }
    void SetEnabled(bool isEnabled) { mEnabled = isEnabled; }

    DirectX::SimpleMath::Vector3 BoundingSphereCenter() const { return mPosition; }
    float BoundingSphereRadius() const { return mBoundingSphereRadius; }

protected:
    virtual void OnShutdown() override;

private:
	void InitializeBuffers(ID3D11Device *pDevice);
	void RenderBuffers(ID3D11DeviceContext *pContext);

    // Load model from a file on disk.
    void LoadModel(const std::wstring& filepath);

    // Load model using v1 file format.
    void LoadTxtModelv1(const std::wstring& filepath);

    // Load model using v2 file format.
    void LoadTxtModelv2(const std::wstring& filepath);

private:
    // in-memory software mesh format.
    struct s_mesh_vertex_t
    {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
    };

private:
    bool mEnabled;

    unsigned int mVertexCount;
    unsigned int mIndexCount;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;

    std::vector<s_mesh_vertex_t> mMeshVertices;     // TODO: pull this and indices out, put into special struct don't store it.
    std::vector<int> mMeshIndices;

	std::unique_ptr<Texture> mTexture;
    
    DirectX::SimpleMath::Vector3 mPosition;
    DirectX::SimpleMath::Vector4 mColor;
    float mBoundingSphereRadius;
};

