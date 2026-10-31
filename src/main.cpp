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
#define D3DLIGHT_SPOT       (2)

struct D3DLIGHT8
{
    DWORD Type;     // 0x00
    BYTE x04[0x5C];
    float Theta;    // 0x60
    float Phi;      // 0x64
};

#define FILL_VFTABLE(tensPlace) \
    virtual void Vftable_x ##tensPlace## 0(); \
    virtual void Vftable_x ##tensPlace## 4(); \
    virtual void Vftable_x ##tensPlace## 8(); \
    virtual void Vftable_x ##tensPlace## C();

class IDirect3DDevice8
{
public:
    FILL_VFTABLE(0)
    FILL_VFTABLE(1)
    FILL_VFTABLE(2)
    FILL_VFTABLE(3)
    FILL_VFTABLE(4)
    FILL_VFTABLE(5)
    FILL_VFTABLE(6)
    FILL_VFTABLE(7)
    FILL_VFTABLE(8)
    FILL_VFTABLE(9)
    FILL_VFTABLE(A)

    virtual long STDCALL SetLight(DWORD Index, CONST D3DLIGHT8* pLight); // 0xB0
};

// Implementation is based on https://github.com/crosire/d3d8to9/blob/3e293ca7f834c3c46f0bd8c688255bd2087449df/source/d3d8to9_device.cpp#L719
long STDCALL SetD3d8Light_Hook(IDirect3DDevice8* device, DWORD Index, CONST D3DLIGHT8* pLight)
{
    if (pLight == NULL)
        return D3DERR_INVALIDCALL;

    if (pLight->Type == D3DLIGHT_SPOT)
    {
        if (pLight->Theta <= pLight->Phi)
        {
            D3DLIGHT8 lightEdit = *pLight;
            lightEdit.Theta /= 1.75f;

            return device->SetLight(Index, &lightEdit);
        }
    }

    return device->SetLight(Index, pLight);
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
