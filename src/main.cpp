#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d8.h>

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

class IDirect3DDevice8_Hook
{
public:
    // Implementation is based on https://github.com/crosire/d3d8to9/blob/3e293ca7f834c3c46f0bd8c688255bd2087449df/source/d3d8to9_device.cpp#L719
    long __stdcall SetLight_Hook(DWORD Index, CONST D3DLIGHT8* pLight)
    {
        if (pLight == NULL)
            return D3DERR_INVALIDCALL;

        D3DLIGHT8 lightEdit = *pLight;

        if (lightEdit.Type == D3DLIGHT_SPOT)
        {
            if (lightEdit.Theta <= lightEdit.Phi)
            {
                lightEdit.Theta /= 1.75f;
            }
        }

        IDirect3DDevice8* device = (IDirect3DDevice8*) this;
        return device->SetLight(Index, &lightEdit);
    }
};

void Init()
{
    #define SET_LIGHT_CALL_F_OF 0xCEF9

    if (DWORD rp8Handle = (DWORD) GetModuleHandleA("rp8.dll"))
    {
        Hook(rp8Handle + SET_LIGHT_CALL_F_OF, &IDirect3DDevice8_Hook::SetLight_Hook, 6);
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hinstDLL);
    UNREFERENCED_PARAMETER(lpReserved);

    if (fdwReason == DLL_PROCESS_ATTACH)
        Init();

    return TRUE;
}
