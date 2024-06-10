#pragma once
#include <cprocessing.h>

extern const int window_width;
extern const int window_height;

// enum

enum AliveOrFinish
{
	Finish = 0,
	Alive = 1
}typedef AliveOrFinish;

enum ExplosionState
{
	Before = 0,
	After = 1
}typedef ExplosionState;

// struct

struct Vec2d {
	float x;
	float y;
}typedef Vec2d;

struct Particle {

	struct Vec2d Pos;

	//Velocity X, Y
	struct Vec2d Velocity;

	//Acceleration X,Y
	struct Vec2d Acceleration;

	AliveOrFinish alive;

	struct Vec2d Start;
	struct Vec2d Target;

	int RL; 

	float w;
	float h;

	float accumulatedTime;
	float maxTime;

	float accumulatedLifeSpanTime;
	float maxLifeSpanTime;

	CP_Color color;
}typedef Particle;

// Function

void Particle_InitializeRandom(struct Particle* obj, AliveOrFinish a);
void Particle_Initialize(struct Particle* obj, AliveOrFinish a,
	float  X, float Y, float W, float H,
	CP_Color color);
void Particle_Draw(const struct Particle* obj);
void Particle_Update(struct Particle* obj);

void Particle_Movement_Linear(struct Particle* obj);
void Particle_Movement_Gravity(struct Particle* obj);

void Particle_SetRandomGoal(struct Particle* obj);

void Particle_KillObject(struct Particle* obj);

void Particle_AddVelocity(struct Particle* obj, float x, float y);
void Particle_AddAcceleration(struct Particle* obj, float x, float y);

// Pulling

struct MemoryPool {
	int TotalSize;

	struct Particle* AllObjects;
};

void MemPool_AllocateMemory(struct MemoryPool* pMemPool, int size);
void MemPool_DeallocateMemory(struct MemoryPool* pMemPool);
void MemPool_InitializeMemory(struct MemoryPool* pMemPool);

void MemPool_DrawAllObjects(const struct MemoryPool* pMemPool);
void MemPool_UpdateAllObjects(const struct MemoryPool* pMemPool);

void MemPool_DeleteAll(struct MemoryPool* pMemPool);

struct Particle* MemPool_GetFirstDeadObj(struct MemoryPool* pMemPool);


// Cycle Function

void Firework_State_init(void);
void Firework_State_update(void);
void Firework_State_exit(void);

// Memory Pool
extern struct MemoryPool* gMemoryPool;