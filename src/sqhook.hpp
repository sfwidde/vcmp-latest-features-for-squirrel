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
// SQClassHook CLASS
// -----------------------------------------------------------------------------

class SQClassHook
{
private:
	// Whether this object succeeded at retrieving the class
	// from Squirrel's root table, i.e. the class exists
	bool m_valid;

public:
	SQClassHook(const SQChar* className);
	~SQClassHook();

	void AddMethod(SQFUNCTION function, const SQChar* methodName, const SQChar* parameterMask, SQInteger parameterCount = SQ_MATCHTYPEMASKSTRING) const;
private:
	void AddSqratProperty(SQFUNCTION function, const SQChar* propertyName, const SQChar* sqratTableName) const;
public:
	void AddSqratProperty(SQFUNCTION setterFunction, SQFUNCTION getterFunction, const SQChar* propertyName) const;
	bool IsObjectInstanceOfThis(SQInteger objectIndex) const;
};

// -----------------------------------------------------------------------------

void RegisterSquirrelFunction(SQFUNCTION function, const SQChar* functionName,
	const SQChar* parameterMask, SQInteger parameterCount = SQ_MATCHTYPEMASKSTRING);

#endif
