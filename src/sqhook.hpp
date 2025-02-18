/*
 * Latest Vice City: Multiplayer (VC:MP) 0.4 features for Squirrel
 * Author: sfwidde ([SS]Kelvin)
 * 2024-12-04
 */

/*
	Utilities that will help us hook the official Squirrel module
 */

#ifndef SQHOOK_HPP
#define SQHOOK_HPP

#include <squirrel/squirrel.h>
#include <utils.hpp>

// -----------------------------------------------------------------------------

/*
	Refer to
	https://bitbucket.org/stormeus/0.4-squirrel/src/master/
	for the definitions below
 */

/* UtilStructs.cpp, CPlayer.cpp, CVehicle.cpp */
#define SQID_VECTOR       _SC("Vector")
#define SQID_ENTITYVECTOR _SC("EntityVector")
#define SQID_PLAYER       _SC("CPlayer")
#define SQID_VEHICLE      _SC("CVehicle")

// Any extra padding byte and we're fucked. This ensures we remain safe and sound :)
#pragma pack(push, 1)

/* UtilStructs.h */
struct Vector
{
	float x; // 4
	float y; // 4
	float z; // 4
};

/* UtilStructs.h */
//typedef Vector EntityVector; // Inherits from Vector

/* CPlayer.h */
struct CPlayer
{
	PAD(0, sizeof(Vector*)); // 4/8
	int nPlayerId;           // 4
};

/* CVehicle.h */
struct CVehicle
{
	PAD(0, sizeof(int)); // 4
	int nVehicleId;      // 4
};

#pragma pack(pop)

// -----------------------------------------------------------------------------

class SQClassHook
{
	// Coulda just added a new method but meh this made more sense to me
	friend bool IsSquirrelObjectInstanceOfClass(SQInteger, const SQClassHook&);

private:
	HSQUIRRELVM v;

	void AddSqratProperty(SQFUNCTION function, const SQChar* propertyName, const SQChar* sqratTableName) const;

public:
	SQClassHook(HSQUIRRELVM v, const SQChar* className);
	~SQClassHook();

	void AddMethod(SQFUNCTION function, const SQChar* methodName, const SQChar* parameterMask, SQInteger parameterCount = SQ_MATCHTYPEMASKSTRING) const;
	void AddSqratProperty(SQFUNCTION setterFunction, SQFUNCTION getterFunction, const SQChar* propertyName) const;
};

void RegisterSquirrelFunction(HSQUIRRELVM v, SQFUNCTION function, const SQChar* functionName,
	const SQChar* parameterMask, SQInteger parameterCount = SQ_MATCHTYPEMASKSTRING);
bool IsSquirrelObjectInstanceOfClass(SQInteger objectIndex, const SQClassHook& sqClass);

// -----------------------------------------------------------------------------

#endif
