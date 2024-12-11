/*
 * Latest Vice City: Multiplayer (VC:MP) 0.4 features for Squirrel
 * Author: sfwidde ([SS]Kelvin)
 * 2024-12-04
 */

/*
 * Utilities that will help us hook the official Squirrel module
 */

#ifndef SQHOOK_H
#define SQHOOK_H

#include <squirrel/squirrel.h>
#include <utils.h>
#include <stdbool.h>

/* What these entities are named in the official module */
#define SQID_VECTOR       _SC("Vector")
#define SQID_ENTITYVECTOR _SC("EntityVector")
#define SQID_PLAYER       _SC("CPlayer")
#define SQID_VEHICLE      _SC("CVehicle")

// https://bitbucket.org/stormeus/0.4-squirrel/src/master/UtilStructs.h
typedef struct
{
	float x; // 4B
	float y; // 4B
	float z; // 4B
} VectorHook;

// EntityVector (hook) should go here but we can skip it for now (inherits from Vector)

#pragma pack(push, 1)
// https://bitbucket.org/stormeus/0.4-squirrel/src/master/CPlayer.h
typedef struct
{
	PAD(0, sizeof(VectorHook*)); // 4/8B
	int nPlayerId;               // 4B
} CPlayerHook;

// https://bitbucket.org/stormeus/0.4-squirrel/src/master/CVehicle.h
typedef struct
{
	PAD(0, sizeof(int)); // 4B
	int nVehicleId;      // 4B
} CVehicleHook;
#pragma pack(pop)

// -----------------------------------------------------------------------------

void SQLF_RegisterSquirrelFunction(
	HSQUIRRELVM   v,
	SQFUNCTION    function,
	const SQChar* functionName,
	SQInteger     parameterCount,
	const SQChar* parameterMask
);
bool SQLF_RegisterSquirrelClassMethod(
	HSQUIRRELVM   v,
	SQFUNCTION    function,
	const SQChar* className,
	const SQChar* methodName,
	SQInteger     parameterCount,
	const SQChar* parameterMask
);
bool SQLF_IsSquirrelObjectInstanceOfClass(
	HSQUIRRELVM   v,
	SQInteger     objectIndex,
	const SQChar* className
);
void SQLF_RegisterSqratClassProperty(
	HSQUIRRELVM   v,
	SQFUNCTION    setterFunction,
	SQFUNCTION    getterFunction,
	const SQChar* className,
	const SQChar* propertyName
);

#endif
