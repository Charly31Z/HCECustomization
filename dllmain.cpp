// dllmain.cpp : Define el punto de entrada de la aplicación DLL.
#include "pch.h"

#include <iostream>
#include <fstream>

#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "detours.h"
#pragma comment(lib, "detours.lib")

#include "Texture.h"
#include "io.h"
#include "mem.h"

#include "Tables.h"

const char* windowName = "Halo";
BYTE* base;
DWORD fxBase;
DWORD* input;

HMODULE DllHandle;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef HRESULT(__stdcall* endScene)(IDirect3DDevice9* pDevice);
endScene pEndScene;

HWND window;

int width, height;

typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
WNDPROC oWndProc;

bool showMenu = false;

bool menu;

HHOOK _hook;

int sectionSelected;
int hatSelected;

Texture nothingTxt;
Texture mariachiTxt;
Texture navidadTxt;

Tables* tables;

LRESULT CALLBACK HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (showMenu)
    {
        // the action is valid: HC_ACTION.
        if (wParam == WM_MOUSEMOVE)
        {
            ShowCursor(true);
            return true;
        }
    }

    // call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
    return CallNextHookEx(_hook, nCode, wParam, lParam);
}

DWORD WINAPI SetHook(LPVOID param)
{
    MSG msg;
    HINSTANCE appInstance = GetModuleHandle(NULL);

    if (!(_hook = SetWindowsHookEx(WH_MOUSE_LL, HookCallback, appInstance, 0)))
    {
        MessageBox(NULL, L"Failed to install hook!", L"Error", MB_ICONERROR);
    }

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

void ReleaseHook()
{
    UnhookWindowsHookEx(_hook);
}

void save()
{
    switch (sectionSelected)
    {
    case 0:
        std::ofstream os("custom.prefs", std::ios::binary);

        if (os.is_open())
            IO::write(os, hatSelected);

        os.close();
        break;
    }
}

void load()
{
    std::ifstream in("custom.prefs", std::ios::binary);

    if (in.is_open())
        IO::read(in, hatSelected);

    in.close();
}

HRESULT __stdcall hookedEndScene(IDirect3DDevice9* pDevice)
{
    static bool init = true;

    if (init)
    {
        init = false;

        load();

        nothingTxt.createTexture(pDevice, "nothing.png");
        mariachiTxt.createTexture(pDevice, "mariachi.png");
        navidadTxt.createTexture(pDevice, "navidad.png");

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        ImGui::StyleColorsDark();

        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX9_Init(pDevice);

        //io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
        //io.MouseDrawCursor = true;

        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5, io.DisplaySize.y * 0.5), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 1.2, io.DisplaySize.y / 1.2));
    }

    tables = new Tables(base);

    if (showMenu)
    {
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Customization", &menu, ImGuiWindowFlags_NoCollapse);

        ImVec2 size = ImGui::GetWindowSize();

        ImGui::BeginChild("General", ImVec2(size.x / 3.25, size.y / 1.15), true);
        if (ImGui::TreeNode("Spartan"))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));

            for (int i = 0; i < 2; i++)
            {
                ImGuiTreeNodeFlags node_flags;

                if (sectionSelected == i)
                    node_flags = ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_Leaf;
                else
                    node_flags = ImGuiTreeNodeFlags_Leaf;

                bool node_open;

                switch (i)
                {
                case 0:
                    node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Hats");
                    break;
                case 1:
                    node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Coming Soon...(Maybe?)");
                    break;
                }

                if (ImGui::IsItemClicked()) {
                    sectionSelected = i;
                }

                if (node_open)
                {
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
            ImGui::PopStyleVar();
        }
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::BeginChild("Information", ImVec2(size.x / 1.15, size.y / 1.15), true);
        switch (sectionSelected)
        {
        case 0:
            for (int h = 0; h < 3; h++)
            {
                ImGui::SameLine();

                if (hatSelected == h)
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.259f, 0.588f, 0.98f, 1.0f));
                else
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.3f, 0.4f, 1.0f });

                switch (h)
                {
                case 0:
                    if (ImGui::ImageButton((void*)nothingTxt.getTxt(), ImVec2(100, 100), ImVec2(0, 0), ImVec2(1, 1)))
                    {
                        hatSelected = 0;
                    }
                    break;
                case 1:
                    if (ImGui::ImageButton((void*)mariachiTxt.getTxt(), ImVec2(100, 100), ImVec2(0, 0), ImVec2(1, 1)))
                    {
                        hatSelected = 1;
                    }
                    break;
                case 2:
                    if (ImGui::ImageButton((void*)navidadTxt.getTxt(), ImVec2(100, 100), ImVec2(0, 0), ImVec2(1, 1)))
                    {
                        hatSelected = 2;
                    }
                    break;
                }

                ImGui::PopStyleColor();
            }
            
            ImGui::SetCursorPosY(size.y / 1.3);
            if (ImGui::Button("Save"))
            {
                save();
            }
            ImGui::SameLine();
            if (ImGui::Button("Reload Player Table"))
            {
                Player me = tables->players->get_player(0);
                Object obj = tables->objects->get_object(me.player_obj_id);
            }
            break;
        case 1:
            ImGui::Text("Maybe i will be added new options for\nthe customization or i dont know\n\nCredits: Charly31Z");
            break;
        }
        ImGui::EndChild();

        ImGui::End();

        ImGui::EndFrame();
        ImGui::Render();

        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }

    /*RECT window;
    width = 0;
    height = 0;
    if (GetWindowRect(hWnd, &window))
    {
        width = window.right - window.left;
        height = window.bottom - window.top;
    }
    if (!showMenu)
    {
        int padding = 2;
        int rectx1 = 100, rectx2 = 300, recty1 = 50, recty2 = 100;
        D3DRECT rectangle = { rectx1 * width, recty1 * height, rectx2 * width, recty2 * height };
        //pDevice->Clear(1, &rectangle, D3DCLEAR_TARGET, D3DCOLOR_ARGB(200, 0, 0, 0), 1.0f, 0);
        //pDevice->Clear(0, &rectangle, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
        if (!font)
            D3DXCreateFont(pDevice, 26, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial", &font);
        RECT textRectangle;
        SetRect(&textRectangle, rectx1 + padding, recty1 + padding, rectx2 - padding, recty2 - padding);
        font->DrawTextA(NULL, "Armaduras cargadas exitosamente...\nPresiona F1 para personalizar", -1, &textRectangle, DT_NOCLIP | DT_LEFT, D3DCOLOR_ARGB(200, 255, 255, 255));
    }
    else
    {
        int perPixel = 2;
        int padding = 1;
        int x = 0, y = -1, w = 3, h = 3;
        if (height <= 900)
            perPixel = 4;
        else if (height >= 900)
            perPixel = 2;
        D3DRECT rectangle = { width / perPixel, height / perPixel, w * width / perPixel, h * height / perPixel };
        pDevice->Clear(1, &rectangle, D3DCLEAR_TARGET, D3DCOLOR_ARGB(150, 46, 59, 70), 0.0f, 0);
        if (!font)
            D3DXCreateFont(pDevice, 26/perPixel, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial", &font);
        RECT textRectangle;
        SetRect(&textRectangle, x + padding * width/ perPixel, y + padding * height/ perPixel, w * width/ perPixel, h * height/ perPixel);
        font->DrawTextA(NULL, "Menu pa armaduras", -1, &textRectangle, DT_NOCLIP | DT_CENTER, D3DCOLOR_ARGB(200, 255, 255, 255));
    }*/
    return pEndScene(pDevice);
}

void turnoff()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void disableInput(bool value)
{
    if (value)
    {
        uintptr_t* funcDisable = (uintptr_t*)(base + 0x934A0);

        __asm call funcDisable
    }
    else
    {
        uintptr_t* funcEnable = (uintptr_t*)(base + 0x933E0);

        __asm call funcEnable
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    if (showMenu)
    {
        disableInput(true);

        //ShowCursor(TRUE);

        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
        io.MouseDrawCursor = true; //Draw Mouse Cursor

        return true;
    } 
    else
    {
        disableInput(false);

        //ShowCursor(FALSE);

        //SetForegroundWindow(NULL);
        return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
    }
}

DWORD __stdcall EjectThread(LPVOID param)
{
    Sleep(100);
    FreeLibraryAndExitThread(DllHandle, 0);
    return 0;
}

void hook()
{
    window = FindWindowA(NULL, windowName);

    // + 0x24C528
    base = (BYTE*)GetModuleHandleA("haloce.exe");

    fxBase = (DWORD)(base);

    input = (DWORD*)(fxBase + 0x24C528);

    oWndProc = (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)WndProc);

    IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);

    if (!pD3D)
        return;

    //Sleep(3000);

    D3DPRESENT_PARAMETERS d3dparams = { 0 };
    d3dparams.hDeviceWindow = window;
    d3dparams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dparams.Windowed = true;

    IDirect3DDevice9* pDevice = nullptr;

    HRESULT result = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dparams.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dparams, &pDevice);
    if (FAILED(result) || !pDevice)
    {
        pD3D->Release();
        return;
    }

    void** vTable = *reinterpret_cast<void***>(pDevice);

    if (pDevice)
        pDevice->Release(), pDevice = nullptr;

    pEndScene = (endScene)DetourFunction((PBYTE)vTable[42], (PBYTE)hookedEndScene);
}

bool hasInitialized()
{
    if (FindWindowA(NULL, windowName))
    {
        return true;
    }

    return false;
}

DWORD WINAPI Menue(LPVOID param)
{
    while (true)
    {
        if (hasInitialized())
            break;
    }

    hook();
    while (true)
    {
        Sleep(50);
        if (GetAsyncKeyState(VK_F2))
        {
            showMenu = !showMenu;
        }
    }

    CreateThread(0, 0, EjectThread, 0, 0, 0);

    //pDevice->Release();
    //pD3D->Release();

    return false;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DllHandle = hModule;
        CreateThread(0, 0, Menue, hModule, 0, 0); // Creates our thread
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

