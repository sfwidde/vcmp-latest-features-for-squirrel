/*
 * Latest Vice City: Multiplayer (VC:MP) 0.4 features for Squirrel
 * Author: sfwidde ([SS]Kelvin)
 * 2024-07-07
 */

#include "sqhook.hpp"
#include <VCMP.h>
#include <squirrel/SQModule.h>

extern PluginFuncs* vcmpFunctions;
extern HSQAPI sq;

// -----------------------------------------------------------------------------

/*
	Helper functions
 */

// Grab T class instance off the Squirrel stack
template<typename T>
static const T* GetInstance(HSQUIRRELVM v, SQInteger stackIndex)
{
	SQUserPointer p;
	sq->getinstanceup(v, stackIndex, &p, nullptr);
	return (T*)p;
}

// -----------------------------------------------------------------------------

/*
	Global functions
 */

/* (bool) -> void */
static SQInteger SetCrouchEnabled(HSQUIRRELVM v)
{
	SQBool enable;
	sq->getbool(v, 2, &enable);
	vcmpFunctions->SetServerOption(vcmpServerOptionDisableCrouch, !enable);
	return 0;
}

/* (void) -> bool */
static SQInteger GetCrouchEnabled(HSQUIRRELVM v)
{
	sq->pushbool(v, !vcmpFunctions->GetServerOption(vcmpServerOptionDisableCrouch));
	return 1;
}

/*
	CPlayer class read-write properties
 */

/* (bool) -> void */
static SQInteger CPlayer_SetBleeding(HSQUIRRELVM v)
{
	SQBool enable;
	sq->tobool(v, 2, &enable);
	vcmpFunctions->SetPlayerOption(GetInstance<CPlayer>(v, 1)->nPlayerId, vcmpPlayerOptionBleeding, !!enable);
	return 0;
}

/* (void) -> bool */
static SQInteger CPlayer_GetBleeding(HSQUIRRELVM v)
{
	sq->pushbool(v, !!vcmpFunctions->GetPlayerOption(GetInstance<CPlayer>(v, 1)->nPlayerId, vcmpPlayerOptionBleeding));
	return 1;
}

/* (int) -> void */
static SQInteger CPlayer_SetDrunkVisuals(HSQUIRRELVM v)
{
	SQInteger drunkLevel = 0;
	sq->getinteger(v, 2, &drunkLevel);
	vcmpFunctions->SetPlayerDrunkVisuals(GetInstance<CPlayer>(v, 1)->nPlayerId, (uint8_t)drunkLevel);
	return 0;
}

/* (void) -> int */
static SQInteger CPlayer_GetDrunkVisuals(HSQUIRRELVM v)
{
	sq->pushinteger(v, vcmpFunctions->GetPlayerDrunkVisuals(GetInstance<CPlayer>(v, 1)->nPlayerId));
	return 1;
}

/* (int) -> void */
static SQInteger CPlayer_SetDrunkHandling(HSQUIRRELVM v)
{
	SQInteger drunkLevel = 0;
	sq->getinteger(v, 2, &drunkLevel);
	vcmpFunctions->SetPlayerDrunkHandling(GetInstance<CPlayer>(v, 1)->nPlayerId, (uint32_t)drunkLevel);
	return 0;
}

/* (void) -> int */
static SQInteger CPlayer_GetDrunkHandling(HSQUIRRELVM v)
{
	sq->pushinteger(v, (SQInteger)vcmpFunctions->GetPlayerDrunkHandling(GetInstance<CPlayer>(v, 1)->nPlayerId));
	return 1;
}

/*
	CPlayer class methods
 */

/* (void) -> void */
static SQInteger CPlayer_Kill(HSQUIRRELVM v)
{
	vcmpFunctions->KillPlayer(GetInstance<CPlayer>(v, 1)->nPlayerId);
	return 0;
}

/* (Vector, Vector[, int]) -> void */
static SQInteger CPlayer_SetCameraPos(HSQUIRRELVM v)
{
	if (!IsSquirrelObjectInstanceOfClass(2, SQClassHook(v, SQID_VECTOR)) &&
		!IsSquirrelObjectInstanceOfClass(2, SQClassHook(v, SQID_ENTITYVECTOR)))
	{
		return sq->throwerror(v, _SC("camera position must be a " SQID_VECTOR " " 
			"or an " SQID_ENTITYVECTOR " class instance"));
	}
	if (!IsSquirrelObjectInstanceOfClass(3, SQClassHook(v, SQID_VECTOR)) &&
		!IsSquirrelObjectInstanceOfClass(3, SQClassHook(v, SQID_ENTITYVECTOR)))
	{
		return sq->throwerror(v, _SC("camera look must be a " SQID_VECTOR " " 
			"or an " SQID_ENTITYVECTOR " class instance"));
	}

	SQInteger n = sq->gettop(v);
	SQInteger ms;
	if (n > 3) {
		if (n > 4) { return sq->throwerror(v, _SC("wrong number of parameters")); }
		sq->getinteger(v, 4, &ms);
	} else { ms = 0; }

	const CPlayer* player = GetInstance<CPlayer>(v, 1);
	const Vector* cameraPos = GetInstance<Vector>(v, 2);
	const Vector* cameraLook = GetInstance<Vector>(v, 3);
	uint32_t interpTimeMS = (uint32_t)ms;
	// Set camera position (original CPlayer::SetCameraPos() behavior)
	vcmpFunctions->SetCameraPosition(
		player->nPlayerId,
		cameraPos->x,
		cameraPos->y,
		cameraPos->z,
		cameraLook->x,
		cameraLook->y,
		cameraLook->z
	);
	// Does camera need to be interpolated (new behavior)?
	if (interpTimeMS)
	{
		vcmpFunctions->InterpolateCameraLookAt(
			player->nPlayerId,
			cameraLook->x,
			cameraLook->y,
			cameraLook->z,
			interpTimeMS
		);
	}
	return 0;
}

/* (CPlayer, bool) -> void */
static SQInteger CPlayer_Set3DArrowForPlayer(HSQUIRRELVM v)
{
	if (!IsSquirrelObjectInstanceOfClass(2, SQClassHook(v, SQID_PLAYER)))
	{
		return sq->throwerror(v, _SC("player to show must be a " SQID_PLAYER " class instance"));
	}

	SQBool enable;
	sq->getbool(v, 3, &enable);
	vcmpFunctions->SetPlayer3DArrowForPlayer(GetInstance<CPlayer>(v, 1)->nPlayerId, GetInstance<CPlayer>(v, 2)->nPlayerId, !!enable);
	return 0;
}

/* (CPlayer) -> bool */
static SQInteger CPlayer_Get3DArrowForPlayer(HSQUIRRELVM v)
{
	if (!IsSquirrelObjectInstanceOfClass(2, SQClassHook(v, SQID_PLAYER)))
	{
		return sq->throwerror(v, _SC("player to show must be a " SQID_PLAYER " class instance"));
	}

	sq->pushbool(v, !!vcmpFunctions->GetPlayer3DArrowForPlayer(
		GetInstance<CPlayer>(v, 1)->nPlayerId, GetInstance<CPlayer>(v, 2)->nPlayerId
	));
	return 1;
}

/*
	CVehicle class read-write properties
 */

/* (bool) -> void */
static SQInteger CVehicle_SetEngineEnabled(HSQUIRRELVM v)
{
	SQBool enable;
	sq->tobool(v, 2, &enable);
	vcmpFunctions->SetVehicleOption(GetInstance<CVehicle>(v, 1)->nVehicleId, vcmpVehicleOptionEngineDisabled, !enable);
	return 0;
}

/* (void) -> bool */
static SQInteger CVehicle_GetEngineEnabled(HSQUIRRELVM v)
{
	sq->pushbool(v, !vcmpFunctions->GetVehicleOption(GetInstance<CVehicle>(v, 1)->nVehicleId, vcmpVehicleOptionEngineDisabled));
	return 1;
}

/* (bool) -> void */
static SQInteger CVehicle_SetBonnetOpen(HSQUIRRELVM v)
{
	SQBool enable;
	sq->tobool(v, 2, &enable);
	vcmpFunctions->SetVehicleOption(GetInstance<CVehicle>(v, 1)->nVehicleId, vcmpVehicleOptionBonnetOpen, !!enable);
	return 0;
}

/* (void) -> bool */
static SQInteger CVehicle_GetBonnetOpen(HSQUIRRELVM v)
{
	sq->pushbool(v, !!vcmpFunctions->GetVehicleOption(GetInstance<CVehicle>(v, 1)->nVehicleId, vcmpVehicleOptionBonnetOpen));
	return 1;
}

/* (bool) -> void */
static SQInteger CVehicle_SetBootOpen(HSQUIRRELVM v)
{
	SQBool enable;
	sq->tobool(v, 2, &enable);
	vcmpFunctions->SetVehicleOption(GetInstance<CVehicle>(v, 1)->nVehicleId, vcmpVehicleOptionBootOpen, !!enable);
	return 0;
}

/* (void) -> bool */
static SQInteger CVehicle_GetBootOpen(HSQUIRRELVM v)
{
	sq->pushbool(v, !!vcmpFunctions->GetVehicleOption(GetInstance<CVehicle>(v, 1)->nVehicleId, vcmpVehicleOptionBootOpen));
	return 1;
}

/*
	CVehicle class methods
 */

/* (CPlayer, bool) -> void */
static SQInteger CVehicle_Set3DArrowForPlayer(HSQUIRRELVM v)
{
	if (!IsSquirrelObjectInstanceOfClass(2, SQClassHook(v, SQID_PLAYER)))
	{
		return sq->throwerror(v, _SC("player to show must be a " SQID_PLAYER " class instance"));
	}

	SQBool enable;
	sq->getbool(v, 3, &enable);
	vcmpFunctions->SetVehicle3DArrowForPlayer(GetInstance<CVehicle>(v, 1)->nVehicleId, GetInstance<CPlayer>(v, 2)->nPlayerId, !!enable);
	return 0;
}

/* (CPlayer) -> bool */
static SQInteger CVehicle_Get3DArrowForPlayer(HSQUIRRELVM v)
{
	if (!IsSquirrelObjectInstanceOfClass(2, SQClassHook(v, SQID_PLAYER)))
	{
		return sq->throwerror(v, _SC("player to show must be a " SQID_PLAYER " class instance"));
	}

	sq->pushbool(v, !!vcmpFunctions->GetVehicle3DArrowForPlayer(
		GetInstance<CVehicle>(v, 1)->nVehicleId, GetInstance<CPlayer>(v, 2)->nPlayerId
	));
	return 1;
}

// -----------------------------------------------------------------------------

void RegisterNewSquirrelDefinitions(HSQUIRRELVM v)
{
	/* Global functions */
	RegisterSquirrelFunction(v, SetCrouchEnabled, _SC("SetCrouchEnabled"), _SC("tb"));
	RegisterSquirrelFunction(v, GetCrouchEnabled, _SC("GetCrouchEnabled"), _SC("t"));

	/* CPlayer class */
	SQClassHook CPlayerClass(v, SQID_PLAYER);
	// Read-write properties
	CPlayerClass.AddSqratProperty(CPlayer_SetBleeding,      CPlayer_GetBleeding,      _SC("Bleeding"));
	CPlayerClass.AddSqratProperty(CPlayer_SetBleeding,      CPlayer_GetBleeding,      _SC("IsBleeding"));
	CPlayerClass.AddSqratProperty(CPlayer_SetDrunkVisuals,  CPlayer_GetDrunkVisuals,  _SC("DrunkVisuals"));
	CPlayerClass.AddSqratProperty(CPlayer_SetDrunkHandling, CPlayer_GetDrunkHandling, _SC("DrunkHandling"));
	// Methods
	CPlayerClass.AddMethod(CPlayer_Kill,                _SC("Kill"),                _SC("x"));
	CPlayerClass.AddMethod(CPlayer_SetCameraPos,        _SC("SetCameraPos"),        _SC("xxxi"), -3);
	CPlayerClass.AddMethod(CPlayer_Set3DArrowForPlayer, _SC("Set3DArrowForPlayer"), _SC("xxb"));
	CPlayerClass.AddMethod(CPlayer_Get3DArrowForPlayer, _SC("Get3DArrowForPlayer"), _SC("xx"));

	/* CVehicle class */
	SQClassHook CVehicleClass(v, SQID_VEHICLE);
	// Read-write properties
	CVehicleClass.AddSqratProperty(CVehicle_SetEngineEnabled, CVehicle_GetEngineEnabled, _SC("EngineEnabled"));
	CVehicleClass.AddSqratProperty(CVehicle_SetEngineEnabled, CVehicle_GetEngineEnabled, _SC("IsEngineEnabled"));
	CVehicleClass.AddSqratProperty(CVehicle_SetBonnetOpen,    CVehicle_GetBonnetOpen,    _SC("BonnetOpen"));
	CVehicleClass.AddSqratProperty(CVehicle_SetBonnetOpen,    CVehicle_GetBonnetOpen,    _SC("IsBonnetOpen"));
	CVehicleClass.AddSqratProperty(CVehicle_SetBootOpen,      CVehicle_GetBootOpen,      _SC("BootOpen"));
	CVehicleClass.AddSqratProperty(CVehicle_SetBootOpen,      CVehicle_GetBootOpen,      _SC("IsBootOpen"));
	// Methods
	CVehicleClass.AddMethod(CVehicle_Set3DArrowForPlayer, _SC("Set3DArrowForPlayer"), _SC("xxb"));
	CVehicleClass.AddMethod(CVehicle_Get3DArrowForPlayer, _SC("Get3DArrowForPlayer"), _SC("xx"));
}

// -----------------------------------------------------------------------------
