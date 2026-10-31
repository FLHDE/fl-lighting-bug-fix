#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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

void Patch(DWORD vOffset, LPVOID mem, UINT len)
{
    DWORD _;

    VirtualProtect((PVOID) vOffset, len, PAGE_EXECUTE_READWRITE, &_);
    memcpy((PVOID) vOffset, mem, len);
}

void inline Patch_BYTE(DWORD vOffset, BYTE value)
{
    Patch(vOffset, &value, sizeof(BYTE));
}

template <typename Func>
DWORD SetRelPointer(DWORD location, Func hookFunc)
{
    DWORD originalPointer, _;

    // Set and calculate the relative offset for the hook function
    VirtualProtect((PVOID) location, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &_);
    originalPointer = location + (*(PDWORD) location) + 4;

    *(Func*) (location) = hookFunc;
    *(PDWORD) (location) -= location + 4;

    return originalPointer;
}

void Nop(DWORD vOffset, UINT len)
{
    DWORD _;

    VirtualProtect((PVOID) vOffset, len, PAGE_EXECUTE_READWRITE, &_);
    memset((PVOID) vOffset, 0x90, len);
}

template <typename Func>
void Hook(DWORD location, Func hookFunc, UINT instrLen, bool jmp = false)
{
    // Set the opcode for the call or jmp instruction
    Patch_BYTE(location, jmp ? 0xE9 : 0xE8); // 0xE9 = jmp, 0xE8 = call

    // Set the relative address
    SetRelPointer(location + 1, hookFunc);

    // Nop out excess bytes
    if (instrLen > 5)
        Nop((location + 5), instrLen - 5);
}

class IDirect3DDevice8
{
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

    virtual long __stdcall SetLight(DWORD Index, CONST D3DLIGHT8* pLight); // 0xB0

public:
    // Implementation is based on https://github.com/crosire/d3d8to9/blob/3e293ca7f834c3c46f0bd8c688255bd2087449df/source/d3d8to9_device.cpp#L719
    long __stdcall SetLight_Hook(DWORD Index, CONST D3DLIGHT8* pLight)
    {
        if (pLight == NULL)
            return D3DERR_INVALIDCALL;

        if (pLight->Type == D3DLIGHT_SPOT)
        {
            if (pLight->Theta <= pLight->Phi)
            {
                D3DLIGHT8 lightEdit = *pLight;
                lightEdit.Theta /= 1.75f;

                return SetLight(Index, &lightEdit);
            }
        }

        return SetLight(Index, pLight);
    }
};

void Init()
{
    if (DWORD rp8Handle = (DWORD) GetModuleHandle("rp8.dll"))
    {
        Hook(rp8Handle + SET_LIGHT_CALL_F_OF, &IDirect3DDevice8::SetLight_Hook, 6);
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
