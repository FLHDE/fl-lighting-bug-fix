#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define STDCALL __stdcall

#define CALL(from, to) \
    *(from) = 0xE8;\
    *(DWORD*) (from+1) = (PBYTE) (to) - (from) - 5

DWORD oldProtect;

#define VirtualProtectX(addr, size) \
    VirtualProtect(addr, size, PAGE_EXECUTE_READWRITE, &oldProtect)

#define SET_LIGHT_CALL_F_OF 0xCEF9

#define D3DERR_INVALIDCALL  (0x8876086C)

typedef enum _D3DLIGHTTYPE
{
    D3DLIGHT_SPOT           = 2,
    D3DLIGHT_FORCE_DWORD    = 0x7fffffff
} D3DLIGHTTYPE;

typedef struct _D3DLIGHT8
{
    D3DLIGHTTYPE Type;  // 0x00
    BYTE x04[0x5C];
    float Theta;        // 0x60
    float Phi;          // 0x64
} D3DLIGHT8;

typedef struct _IDirect3DDevice8 IDirect3DDevice8;

typedef long STDCALL SetD3d8LightFunc(IDirect3DDevice8* pDevice, DWORD Index, CONST D3DLIGHT8* pLight);

typedef struct _IDirect3DDevice8VfTable
{
    BYTE x00[0xB0];
    SetD3d8LightFunc* SetLight; // 0xB0
} IDirect3DDevice8VfTable;

typedef struct _IDirect3DDevice8
{
    IDirect3DDevice8VfTable* vftable;
} IDirect3DDevice8;

// Implementation is based on https://github.com/crosire/d3d8to9/blob/3e293ca7f834c3c46f0bd8c688255bd2087449df/source/d3d8to9_device.cpp#L719
long STDCALL SetD3d8Light_Hook(IDirect3DDevice8* pDevice, DWORD Index, CONST D3DLIGHT8* pLight)
{
    D3DLIGHT8 lightEdit;

    if (pLight == NULL)
        return D3DERR_INVALIDCALL;

    if (pLight->Type == D3DLIGHT_SPOT)
    {
        if (pLight->Theta <= pLight->Phi)
        {
            lightEdit = *pLight;
            lightEdit.Theta /= 1.75f;

            return pDevice->vftable->SetLight(pDevice, Index, &lightEdit);
        }
    }

    return pDevice->vftable->SetLight(pDevice, Index, pLight);
}

void Init()
{
    PBYTE rp8Handle = (PBYTE) GetModuleHandle("rp8.dll");

    if (rp8Handle)
    {
        VirtualProtectX(rp8Handle + SET_LIGHT_CALL_F_OF, 6);

        CALL(rp8Handle + SET_LIGHT_CALL_F_OF, SetD3d8Light_Hook);
        *(PBYTE) (rp8Handle + SET_LIGHT_CALL_F_OF + 5) = 0x90;
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hinstDLL);
        Init();
    }

    return TRUE;
}
