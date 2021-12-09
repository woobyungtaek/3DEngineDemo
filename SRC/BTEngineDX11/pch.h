#pragma once

#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment( lib, "Effects11d_x64.lib" )
#else
#pragma comment( lib, "Effects11_x64.lib" )
#endif
#elif _WIN32
#ifdef _DEBUG
#pragma comment( lib, "Effects11d.lib" )
#else
#pragma comment( lib, "Effects11.lib" )
#endif
#endif

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment( lib, "DirectXTKd_x64.lib" )
#else
#pragma comment( lib, "DirectXTK_x64.lib" )
#endif
#elif _WIN32
#ifdef _DEBUG
#pragma comment( lib, "DirectXTKd.lib" )
#else
#pragma comment( lib, "DirectXTK.lib" )
#endif
#endif

#include <windows.h>
#include <vector>
#include <unordered_map>
#include <string>

#include <d3d11.h>
#include <d3dcompiler.h>

#include "DXTKFont.h"
#include <SimpleMath.h>

//  DX11
#include "DX11Define.h"
#include "d3dx11Effect.h"
#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"

#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "struct_and_const.h"

// Parser
#include "ParsingDataDefine.h"
#include "Animation.h"
#include "BTASEParser.h"

// Input
#include "BTInput.h"

#include "Camera.h" // EngineObject인데 일단 순서상..

// Component
#include "Object.h"
#include "BTBehaviour.h"
#include "Transform.h"
#include "GameObject.h"

#include "MaterialProperties.h"
#include "BTMaterial.h"

#include "MeshData.h"
#include "ModelData.h"
#include "ModelDataSet.h"
#include "ResourceManager.h"

#include "Renderer.h"
#include "MeshRenderer.h"
#include "HelperRenderer.h"
#include "SpriteRenderer.h"

// EngineObject

#include "BTDefine.h"
#include "EngineScene.h"

// Core
#include "D3D11Render.h"
#include "BTGameEngine.h"

// 외부
#include "BTEngine.h"

using namespace DirectX;