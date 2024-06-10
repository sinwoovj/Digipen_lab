#pragma once
#include "firework.h"
#include "stdlib.h"
#include <stdio.h>

#define MAX_PARTICLES1 15
#define MAX_PARTICLES2 500
#define PARTICLE_INTERVAL 0.05
#define INIT_VELOCITY_X 50
#define INIT_VELOCITY_Y 200
#define TEXTSIZE 15
#define PARTICLE_W 5
#define PARTICLE_H 10

struct MemoryPool* gMemoryPool = NULL;
int particle_cnt = -1;
struct Particle* object[MAX_PARTICLES1] = { 0 };
Vec2d loc[MAX_PARTICLES1] = { 0 };
float interval_check = 0;

ExplosionState explosion;
Vec2d explosion_loc;
// Functions

void Particle_InitializeRandom(Particle* obj, AliveOrFinish a)
{
	if (obj == NULL)
		return;

	float x = CP_Random_RangeFloat((float)window_width / 3, ((float)window_width / 3) * 2);
	float y = (float)window_height;

	float w = PARTICLE_W;
	float h = PARTICLE_H;

	CP_Color color = CP_Color_Create(CP_Random_RangeInt(0, 255),
		CP_Random_RangeInt(0, 255),
		CP_Random_RangeInt(0, 255),
		255);

	Particle_Initialize(obj, a, x, y, w, h, color);
}

void Particle_Initialize(struct Particle* obj, AliveOrFinish a,
						 float  X, float Y, float W, float H,
						 CP_Color color){
	if (obj == NULL)
		return;
	obj->accumulatedLifeSpanTime = 0;
	obj->maxLifeSpanTime = CP_Random_RangeFloat(3.0f, 4.0f);

	obj->alive = a;
	obj->color = color;

	obj->Pos.x = X;
	obj->Pos.y = Y;

	obj->w = W;
	obj->h = H;
	obj->RL = CP_Random_RangeInt(0, 2);
	obj->Velocity.x = (obj->RL ? -1 : 1) * (float)INIT_VELOCITY_X;
	obj->Velocity.y = -1 *(float)INIT_VELOCITY_Y;

	obj->Acceleration.x = (float)((obj->RL ? -1 : 1) * 10);
	obj->Acceleration.y = -100;
	if (a == Alive) {
		loc[particle_cnt] = obj->Pos;
		particle_cnt++;
	}

	Particle_SetRandomGoal(obj);
}
Particle* Particle_Generate(struct Particle* obj, AliveOrFinish a, Vec2d loc_) {
	Particle* output = MemPool_GetFirstDeadObj(gMemoryPool);;
	Particle_Initialize(output, a, loc_.x, loc_.y, obj->w, obj->h, obj->color);
	output->Target.x = obj->Target.x;
	output->Target.y = obj->Target.y;
	output->Start = loc_;
	output->accumulatedLifeSpanTime = 0;
	output->accumulatedTime = 0;

	output->Velocity.x = (obj->RL ? -1 : 1) * (float)INIT_VELOCITY_X;
	output->Velocity.y = -1 * (float)INIT_VELOCITY_Y;
	output->Acceleration.x = (float)((obj->RL ? -1 : 1) * 10);
	output->Acceleration.y = -100;
	output->maxLifeSpanTime = obj->maxLifeSpanTime;
	return output;
}

void Particle_Draw(const Particle* obj)
{
	if (obj == NULL)
		return;

	CP_Settings_Fill(obj->color);
	CP_Settings_NoStroke();
	CP_Graphics_DrawRect(obj->Pos.x, obj->Pos.y, obj->w, obj->h);
}

void Particle_Movement_Linear(Particle* obj)
{
	//Want them to move to another point
	obj->Pos.x = (obj->accumulatedTime / obj->maxTime) * (obj->Target.x - obj->Start.x) + obj->Start.x;
	obj->Pos.y = (obj->accumulatedTime / obj->maxTime) * (obj->Target.y - obj->Start.y) + obj->Start.y;

	obj->accumulatedTime += CP_System_GetDt();

	//If they arrive to the other point
	if (obj->accumulatedTime >= obj->maxTime)
	{
		//Set new goal
		Particle_SetRandomGoal(obj);
	}
}

void ShowInfo(Particle* obj) {
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Settings_TextSize(TEXTSIZE);
	char buffer[200];
	sprintf_s(buffer, 200, "FPS : %f\nVelocity (Y) : %f", CP_System_GetFrameRate(), obj->Velocity.y);
	CP_Font_DrawText(&buffer[0], 10, 10);
}

void Particle_Update(Particle* obj)
{
	if (obj == NULL)
		return;
	Particle_Movement_Gravity(obj);

	obj->accumulatedLifeSpanTime += CP_System_GetDt();

	if (obj->accumulatedLifeSpanTime > obj->maxLifeSpanTime)
	{
		Particle_KillObject(obj);
	}
}

void Particle_Movement_Gravity(Particle* obj)
{
	float t = CP_System_GetDt();

	//Update my velocity, depending on the acceleration
	obj->Velocity.x = obj->Velocity.x + obj->Acceleration.x * t;

	if (obj->Pos.y > 500)
	{
		if (obj->Acceleration.y > 0)
		{
			obj->Velocity.y = obj->Velocity.y - obj->Acceleration.y * t;
		}
	}
	else
	{
		obj->Velocity.y = obj->Velocity.y - obj->Acceleration.y * t;
	}

	//Update my position, depending on the velocity
	obj->Pos.x = obj->Pos.x + obj->Velocity.x * t;
	obj->Pos.y = obj->Pos.y + obj->Velocity.y * t;

	if (obj->Acceleration.x > 0.05f)
		obj->Acceleration.x -= t;
	else if (obj->Acceleration.x < -0.05f)
		obj->Acceleration.x += t;
	else
		obj->Acceleration.x = 0;

	if (obj->Velocity.x > 0.05f)
		obj->Velocity.x -= t;
	else if (obj->Velocity.x < -0.05f)
		obj->Velocity.x += t;
	else
		obj->Velocity.x = 0;
}

void Particle_SetRandomGoal(Particle* obj)
{
	if (obj == NULL)
		return;

	//Create 2 random floats inside the window
	//Set the target to them
	obj->Target.x = CP_Random_RangeFloat((float)window_width  / 3, ((float)window_width / 3)*2);
	obj->Target.y = CP_Random_RangeFloat((float)window_height / 2,((float)window_height / 3)*2);

	obj->Start.x = obj->Pos.x;
	obj->Start.y = obj->Pos.y;

	//Set the timer to 0
	obj->accumulatedTime = 0;

	//Sets the max time to random
	obj->maxTime = CP_Random_RangeFloat(2.f, 5.f);
}

void Particle_KillObject(Particle* obj)
{
	if (obj == NULL)
		return;

	obj->alive = Finish;
}

void Particle_AddVelocity(Particle* obj, float x, float y)
{
	obj->Velocity.x += x;
	obj->Velocity.y += y;
}

void Particle_AddAcceleration(Particle* obj, float x, float y)
{
	obj->Acceleration.x += x;
	obj->Acceleration.y += y;
}

// Pulling

void MemPool_AllocateMemory(struct MemoryPool* pMemPool, int size)
{
	if (pMemPool == NULL)
		return;

	MemPool_DeallocateMemory(pMemPool);

	pMemPool->TotalSize = size;
	pMemPool->AllObjects = malloc(size * sizeof(struct Particle));
}

void MemPool_DeallocateMemory(struct MemoryPool* pMemPool)
{
	if (pMemPool == NULL)
		return;

	pMemPool->TotalSize = 0;

	if (pMemPool->AllObjects != NULL)
		free(pMemPool->AllObjects);

	pMemPool->AllObjects = NULL;
}

void MemPool_InitializeMemory(struct MemoryPool* pMemPool)
{
	if (pMemPool == NULL)
		return;

	int i = 0;
	for (i = 0; i < pMemPool->TotalSize; i++)
	{
		Particle_InitializeRandom(&pMemPool->AllObjects[i], Finish);
	}
}

void MemPool_DrawAllObjects(const struct MemoryPool* pMemPool)
{
	if (pMemPool == NULL)
		return;

	int i = 0;
	for (i = 0; i < pMemPool->TotalSize; i++)
	{
		if (pMemPool->AllObjects[i].alive == Alive)
			Particle_Draw(&pMemPool->AllObjects[i]);
	}
}

void MemPool_UpdateAllObjects(const struct MemoryPool* pMemPool)
{
	if (pMemPool == NULL)
		return;

	int i = 0;
	for (i = 0; i < pMemPool->TotalSize; i++)
	{
		if (pMemPool->AllObjects[i].alive == Alive)
		{
			Particle_Update(&pMemPool->AllObjects[i]);
		}
	}
}

void MemPool_DeleteAll(struct MemoryPool* pMemPool)
{
	if (pMemPool == NULL)
		return;
	int i = 0;
	for (i = 0; i < pMemPool->TotalSize; i++)
		Particle_KillObject(&pMemPool->AllObjects[i]);

	CP_Color color_white = CP_Color_Create(255, 255, 255, 255);
	CP_Graphics_ClearBackground(color_white);
}

struct Particle* MemPool_GetFirstDeadObj(struct MemoryPool* pMemPool)
{
	if (pMemPool == NULL)
		return NULL;

	
	int i = 0;
	for (i = 0; i < pMemPool->TotalSize; i++)
	{
		if (pMemPool->AllObjects[i].alive == Finish)
		{
			return &pMemPool->AllObjects[i];
		}
	}

	return NULL;
}

void Clear_Background() {
	CP_Color color_black = CP_Color_Create(0, 0, 0, 255);
	CP_Graphics_ClearBackground(color_black);
}

// Cycle Functions
void Firework_State_init(void)
{
	if (gMemoryPool != NULL)
	{
		MemPool_DeallocateMemory(gMemoryPool);
		free(gMemoryPool);
	}

	gMemoryPool = malloc(sizeof(struct MemoryPool));
	if (gMemoryPool != NULL) {
		gMemoryPool->AllObjects = NULL;
	}

	MemPool_AllocateMemory(gMemoryPool, MAX_PARTICLES2);
	MemPool_InitializeMemory(gMemoryPool);

	object[0] = MemPool_GetFirstDeadObj(gMemoryPool);
	explosion_loc.x = 0;
	explosion_loc.y = 0;
	explosion = Before;
	particle_cnt = 0;
	//Create the player
	Particle_InitializeRandom(object[0], Alive);
	Clear_Background();
}

void Firework_State_update(void)
{
	if (CP_Input_KeyTriggered(KEY_P)) {
		exit(0);
	}
	interval_check += CP_System_GetDt();
	ShowInfo(object[0]);
	if (object[0]->alive == Finish) {
		explosion_loc = object[0]->Pos;
		explosion = After;
	}
	if (CP_Input_KeyTriggered(KEY_R))
	{
		CP_Engine_SetNextGameStateForced(Firework_State_init, Firework_State_update, Firework_State_exit);
	}
	if (CP_Input_KeyTriggered(KEY_DELETE))
	{
		MemPool_DeleteAll(gMemoryPool);
	}

	if (explosion) {
		// logic..
	}
	
	if (interval_check > PARTICLE_INTERVAL) {
		if (particle_cnt < MAX_PARTICLES1)
		{
			loc[particle_cnt] = object[0]->Pos;
			object[particle_cnt] = Particle_Generate(object[0], Alive, loc[particle_cnt - 1]);
		}
		else {
			for (int i = 2; i < MAX_PARTICLES1; i++) {
				object[i - 1] = object[i];
			}
		}
		interval_check = 0;
	}
	
	MemPool_UpdateAllObjects(gMemoryPool);

	MemPool_DrawAllObjects(gMemoryPool);

	Clear_Background();
}

void Firework_State_exit(void)
{
	MemPool_DeallocateMemory(gMemoryPool);
	free(gMemoryPool);
	gMemoryPool = NULL;
}