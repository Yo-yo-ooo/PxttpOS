#include "main.h"
#include <libm/stdlib/cpuid.h>

GuiInstance* guiInstance;
Window* window;
CanvasComponent* canvas;

unsigned int eax = 0;
unsigned int ebx = 0;
unsigned int ecx = 0;
unsigned int edx = 0;


int main(int argc, const char** argv)
{
    initWindowManagerStuff();
    window = requestWindow();
    if (window == NULL)
        return 0;

    // Update Window
    _Free(window->Title);
    window->Title = StrCopy("cpuid Test");
    window->Dimensions.width = 500;
    window->Dimensions.height = 500;
    window->Resizeable = false;
    setWindow(window);

    // Init Gui Instance
    guiInstance = new GuiInstance(window);
    guiInstance->Init();
    guiInstance->screen->backgroundColor = Colors.white;

    // Create Canvas
    guiInstance->CreateComponentWithId(1400, ComponentType::CANVAS);
    canvas = (CanvasComponent*)guiInstance->GetComponentFromId(1400);
    canvas->position.x = 0;
    canvas->position.y = 0;
    canvas->size.IsXFixed = false;
    canvas->size.ScaledX = 1;
    canvas->size.IsYFixed = false;
    canvas->size.ScaledY = 1;
    canvas->bgColor = Colors.white;

    // Init Canvas
    canvas->Clear();
    canvas->CheckUpdates();

    MainLoop();

    return 0;
}


void MainLoop()
{
    
    while (!CheckForWindowClosed(window))
    {
        guiInstance->Update();
        HandleFrame();
        guiInstance->Render(false);

        // Wait 20ms per frame
        programWait(20);
    }
}

void HandleFrame()
{
    
	struct cpuinfo_x86 _cpuinfo_x86;

    cpuid(0, &eax, &ebx, &ecx, &edx);
	get_cpu_address_sizes(&_cpuinfo_x86);
	cpu_detect(&_cpuinfo_x86);
	get_model_name(&_cpuinfo_x86);

    
    const char* a = StrCombineAndFree(StrCopy("Vendor Id: "),(const char*)_cpuinfo_x86.x86_vendor_id);
    const char* b = StrCombineAndFree(StrCopy("Model Id: "),(const char*)_cpuinfo_x86.x86_model_id);
    const char* c = StrCombineAndFree(StrCopy("Cache Bits: "),to_string(_cpuinfo_x86.x86_cache_bits));
    const char* d = StrCombineAndFree(StrCopy("Physics Bits: "),to_string(_cpuinfo_x86.x86_phys_bits));
    const char* e = StrCombineAndFree(StrCopy("Virtual Bits: "),to_string(_cpuinfo_x86.x86_virt_bits));
    
    canvas->DrawText(0,0,Colors.black,1,a);
    canvas->DrawText(0,20,Colors.black,1,b);
    canvas->DrawText(0,40,Colors.black,1,c);
    canvas->DrawText(0,60,Colors.black,1,d);
    canvas->DrawText(0,80,Colors.black,1,e);
    //_Free(c);_Free(d);
    // All Scancodes are here https://wiki.osdev.org/PS/2_Keyboard#Scan_Code_Set_1
    if (envGetKeyState(Key_Enter))
    {
        serialPrintLn("> ENTER WAS PRESSED!");
    }
}