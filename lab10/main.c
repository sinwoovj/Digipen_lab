#include "cprocessing.h"
#include "firework.h"

const int window_width = 1000;
const int window_height = 750;

int main(void)
{
	CP_System_SetWindowSize(window_width, window_height);
	CP_Engine_SetNextGameState(Firework_State_init, Firework_State_update, Firework_State_exit);
	CP_Engine_Run();
	return 0;
}
