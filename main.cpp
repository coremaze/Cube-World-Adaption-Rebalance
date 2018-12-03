#include <windows.h>
#include "cube.h"
#include <math.h>


unsigned int base;

int __thiscall __declspec(noinline) cube__AdaptationWidget__CalculateAdaptationCost(cube::AdaptationWidget *adaptionwidget){
    cube::GameController* gamecontroller = adaptionwidget->gamecontroller;
    cube::Creature* local_player = gamecontroller->local_player;

    AdaptationItem* weapon = adaptionwidget->GetWeapon();
    if (!weapon){
        return 0;
    }

    if (weapon->level >= local_player->level){
        return 0;
    }

    int effective_level = (weapon->level - 5) + weapon->rarity;
    float level_difference = local_player->level - effective_level;
    float stretch = 1500.0;
    float limit = 10000.0;
    float level_scale = (((float)local_player->level)+1.0) / (((float)local_player->level)+10.0);
    float result = level_scale * ( 1.0 - (stretch/(level_difference+stretch)) ) * limit;
    return (int)result;
}
unsigned int cube__AdaptationWidget__CalculateAdaptationCost_ptr = (unsigned int)&cube__AdaptationWidget__CalculateAdaptationCost;

unsigned int inlinefix_JMP_back;
unsigned int cube__AdaptationWidget__GetWeapon_ptr;
void __declspec(naked) inlinefix(){
    asm("mov ecx, ebx"); //AdaptationWidget
    asm("call [_cube__AdaptationWidget__CalculateAdaptationCost_ptr]");
    asm("mov edi, eax"); //cost expected in edi
    asm("jmp [_inlinefix_JMP_back]");
}

void WriteJMP(BYTE* location, BYTE* newFunction){
	DWORD dwOldProtection;
	VirtualProtect(location, 5, PAGE_EXECUTE_READWRITE, &dwOldProtection);
    location[0] = 0xE9; //jmp
    *((DWORD*)(location + 1)) = (DWORD)(( (unsigned INT32)newFunction - (unsigned INT32)location ) - 5);
	VirtualProtect(location, 5, dwOldProtection, &dwOldProtection);
}

extern "C" __declspec(dllexport) bool APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            base = (unsigned int)GetModuleHandle(NULL);
            cube::SetBase(base);

            WriteJMP((BYTE*)(base + 0xF4F0), (BYTE*)cube__AdaptationWidget__CalculateAdaptationCost);


            inlinefix_JMP_back = base + 0x10C1A;
            cube__AdaptationWidget__GetWeapon_ptr =  base + 0xF570;
            WriteJMP((BYTE*)(base+0x10B88), (BYTE*)inlinefix);
            break;

    }
    return true;
}
