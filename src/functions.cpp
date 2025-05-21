/*
 * Latest Vice City: Multiplayer (VC:MP) 0.4 features for Squirrel
 * Author: sfwidde ([SS]Kelvin)
 * 2024-07-07
 */

#include "sqhook.hpp"
#include <VCMP.h>
#include <squirrel/SQModule.h>

extern PluginFuncs* vcmpFunctions;
extern HSQAPI       sq;
extern HSQUIRRELVM  v;

// Grab T class instance off the Squirrel stack
template<typename T>
static const T* Get(SQInteger stackIndex)
{
	SQUserPointer p;
	sq->getinstanceup(v, stackIndex, &p, nullptr);
	return (T*)p;
}

// -----------------------------------------------------------------------------
// GLOBAL FUNCTIONS
// -----------------------------------------------------------------------------

/* (bool) -> void */
static SQInteger SetCrouchEnabled(HSQUIRRELVM)
{
	SQBool enable;
	sq->getbool(v, 2, &enable);
	vcmpFunctions->SetServerOption(vcmpServerOptionDisableCrouch, !enable);
	return 0;
}

/* (void) -> bool */
static SQInteger GetCrouchEnabled(HSQUIRRELVM)
{
	sq->pushbool(v, !vcmpFunctions->GetServerOption(vcmpServerOptionDisableCrouch));
	return 1;
}

// -----------------------------------------------------------------------------
// CPlayer CLASS
// -----------------------------------------------------------------------------

/* (bool) -> void */
static SQInteger CPlayer_SetBleeding(HSQUIRRELVM)
{
	SQBool enable;
	sq->tobool(v, 2, &enable);
	vcmpFunctions->SetPlayerOption(Get<CPlayer>(1)->nPlayerId, vcmpPlayerOptionBleeding, !!enable);
	return 0;
}

/* (void) -> bool */
static SQInteger CPlayer_GetBleeding(HSQUIRRELVM)
{
	sq->pushbool(v, !!vcmpFunctions->GetPlayerOption(Get<CPlayer>(1)->nPlayerId, vcmpPlayerOptionBleeding));
	return 1;
}

/* (int) -> void */
static SQInteger CPlayer_SetDrunkVisuals(HSQUIRRELVM)
{
	SQInteger drunkLevel = 0;
	sq->getinteger(v, 2, &drunkLevel);
	vcmpFunctions->SetPlayerDrunkVisuals(Get<CPlayer>(1)->nPlayerId, (uint8_t)drunkLevel);
	return 0;
}

/* (void) -> int */
static SQInteger CPlayer_GetDrunkVisuals(HSQUIRRELVM)
{
	sq->pushinteger(v, vcmpFunctions->GetPlayerDrunkVisuals(Get<CPlayer>(1)->nPlayerId));
	return 1;
}

/* (int) -> void */
static SQInteger CPlayer_SetDrunkHandling(HSQUIRRELVM)
{
	SQInteger drunkLevel = 0;
	sq->getinteger(v, 2, &drunkLevel);
	vcmpFunctions->SetPlayerDrunkHandling(Get<CPlayer>(1)->nPlayerId, (uint32_t)drunkLevel);
	return 0;
}

/* (void) -> int */
static SQInteger CPlayer_GetDrunkHandling(HSQUIRRELVM)
{
	sq->pushinteger(v, (SQInteger)vcmpFunctions->GetPlayerDrunkHandling(Get<CPlayer>(1)->nPlayerId));
	return 1;
}

/* (void) -> void */
static SQInteger CPlayer_Kill(HSQUIRRELVM)
{
	vcmpFunctions->KillPlayer(Get<CPlayer>(1)->nPlayerId);
	return 0;
}

/* (Vector, Vector[, int]) -> void */
static SQInteger CPlayer_SetCameraPos(HSQUIRRELVM)
{
	if (!SQClassHook(SQID_VECTOR).IsObjectInstanceOfThis(2) &&
		!SQClassHook(SQID_ENTITYVECTOR).IsObjectInstanceOfThis(2))
	{
		return sq->throwerror(v, _SC("camera position must be a " SQID_VECTOR " " 
			"or an " SQID_ENTITYVECTOR " class instance"));
	}
	if (!SQClassHook(SQID_VECTOR).IsObjectInstanceOfThis(3) &&
		!SQClassHook(SQID_ENTITYVECTOR).IsObjectInstanceOfThis(3))
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

	const CPlayer* player = Get<CPlayer>(1);
	const Vector* cameraPos = Get<Vector>(2);
	const Vector* cameraLook = Get<Vector>(3);
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
	// Does camera need to be interpolated (extended behavior)?
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
static SQInteger CPlayer_Set3DArrowForPlayer(HSQUIRRELVM)
{
	if (!SQClassHook(SQID_PLAYER).IsObjectInstanceOfThis(2))
	{
		return sq->throwerror(v, _SC("player to show must be a " SQID_PLAYER " class instance"));
	}

	SQBool enable;
	sq->getbool(v, 3, &enable);
	vcmpFunctions->SetPlayer3DArrowForPlayer(Get<CPlayer>(1)->nPlayerId, Get<CPlayer>(2)->nPlayerId, !!enable);
	return 0;
}

/* (CPlayer) -> bool */
static SQInteger CPlayer_Get3DArrowForPlayer(HSQUIRRELVM)
{
	if (!SQClassHook(SQID_PLAYER).IsObjectInstanceOfThis(2))
	{
		return sq->throwerror(v, _SC("player to show must be a " SQID_PLAYER " class instance"));
	}

	sq->pushbool(v, !!vcmpFunctions->GetPlayer3DArrowForPlayer(Get<CPlayer>(1)->nPlayerId, Get<CPlayer>(2)->nPlayerId));
	return 1;
}

// -----------------------------------------------------------------------------
// CVehicle CLASS
// -----------------------------------------------------------------------------

/* (bool) -> void */
static SQInteger CVehicle_SetEngineEnabled(HSQUIRRELVM)
{
	SQBool enable;
	sq->tobool(v, 2, &enable);
	vcmpFunctions->SetVehicleOption(Get<CVehicle>(1)->nVehicleId, vcmpVehicleOptionEngineDisabled, !enable);
	return 0;
}

/* (void) -> bool */
static SQInteger CVehicle_GetEngineEnabled(HSQUIRRELVM)
{
	sq->pushbool(v, !vcmpFunctions->GetVehicleOption(Get<CVehicle>(1)->nVehicleId, vcmpVehicleOptionEngineDisabled));
	return 1;
}

/* (bool) -> void */
static SQInteger CVehicle_SetBonnetOpen(HSQUIRRELVM)
{
	SQBool enable;
	sq->tobool(v, 2, &enable);
	vcmpFunctions->SetVehicleOption(Get<CVehicle>(1)->nVehicleId, vcmpVehicleOptionBonnetOpen, !!enable);
	return 0;
}

/* (void) -> bool */
static SQInteger CVehicle_GetBonnetOpen(HSQUIRRELVM)
{
	sq->pushbool(v, !!vcmpFunctions->GetVehicleOption(Get<CVehicle>(1)->nVehicleId, vcmpVehicleOptionBonnetOpen));
	return 1;
}

/* (bool) -> void */
static SQInteger CVehicle_SetBootOpen(HSQUIRRELVM)
{
	SQBool enable;
	sq->tobool(v, 2, &enable);
	vcmpFunctions->SetVehicleOption(Get<CVehicle>(1)->nVehicleId, vcmpVehicleOptionBootOpen, !!enable);
	return 0;
}

/* (void) -> bool */
static SQInteger CVehicle_GetBootOpen(HSQUIRRELVM)
{
	sq->pushbool(v, !!vcmpFunctions->GetVehicleOption(Get<CVehicle>(1)->nVehicleId, vcmpVehicleOptionBootOpen));
	return 1;
}

/* (CPlayer, bool) -> void */
static SQInteger CVehicle_Set3DArrowForPlayer(HSQUIRRELVM)
{
	if (!SQClassHook(SQID_PLAYER).IsObjectInstanceOfThis(2))
	{
		return sq->throwerror(v, _SC("player to show must be a " SQID_PLAYER " class instance"));
	}

	SQBool enable;
	sq->getbool(v, 3, &enable);
	vcmpFunctions->SetVehicle3DArrowForPlayer(Get<CVehicle>(1)->nVehicleId, Get<CPlayer>(2)->nPlayerId, !!enable);
	return 0;
}

/* (CPlayer) -> bool */
static SQInteger CVehicle_Get3DArrowForPlayer(HSQUIRRELVM)
{
	if (!SQClassHook(SQID_PLAYER).IsObjectInstanceOfThis(2))
	{
		return sq->throwerror(v, _SC("player to show must be a " SQID_PLAYER " class instance"));
	}

	sq->pushbool(v, !!vcmpFunctions->GetVehicle3DArrowForPlayer(Get<CVehicle>(1)->nVehicleId, Get<CPlayer>(2)->nPlayerId));
	return 1;
}

// -----------------------------------------------------------------------------

void RegisterNewSquirrelDefinitions()
{
	/* Global functions */
	RegisterSquirrelFunction(SetCrouchEnabled, _SC("SetCrouchEnabled"), _SC("tb"));
	RegisterSquirrelFunction(GetCrouchEnabled, _SC("GetCrouchEnabled"), _SC("t"));

	/* CPlayer class */
	SQClassHook CPlayer(SQID_PLAYER);
	// Read-write properties
	CPlayer.AddSqratProperty(CPlayer_SetBleeding,      CPlayer_GetBleeding,      _SC("Bleeding"));
	CPlayer.AddSqratProperty(CPlayer_SetBleeding,      CPlayer_GetBleeding,      _SC("IsBleeding"));
	CPlayer.AddSqratProperty(CPlayer_SetDrunkVisuals,  CPlayer_GetDrunkVisuals,  _SC("DrunkVisuals"));
	CPlayer.AddSqratProperty(CPlayer_SetDrunkHandling, CPlayer_GetDrunkHandling, _SC("DrunkHandling"));
	// Methods
	CPlayer.AddMethod(CPlayer_Kill,                _SC("Kill"),                _SC("x"));
	CPlayer.AddMethod(CPlayer_SetCameraPos,        _SC("SetCameraPos"),        _SC("xxxi"), -3);
	CPlayer.AddMethod(CPlayer_Set3DArrowForPlayer, _SC("Set3DArrowForPlayer"), _SC("xxb"));
	CPlayer.AddMethod(CPlayer_Get3DArrowForPlayer, _SC("Get3DArrowForPlayer"), _SC("xx"));

	/* CVehicle class */
	SQClassHook CVehicle(SQID_VEHICLE);
	// Read-write properties
	CVehicle.AddSqratProperty(CVehicle_SetEngineEnabled, CVehicle_GetEngineEnabled, _SC("EngineEnabled"));
	CVehicle.AddSqratProperty(CVehicle_SetEngineEnabled, CVehicle_GetEngineEnabled, _SC("IsEngineEnabled"));
	CVehicle.AddSqratProperty(CVehicle_SetBonnetOpen,    CVehicle_GetBonnetOpen,    _SC("BonnetOpen"));
	CVehicle.AddSqratProperty(CVehicle_SetBonnetOpen,    CVehicle_GetBonnetOpen,    _SC("IsBonnetOpen"));
	CVehicle.AddSqratProperty(CVehicle_SetBootOpen,      CVehicle_GetBootOpen,      _SC("BootOpen"));
	CVehicle.AddSqratProperty(CVehicle_SetBootOpen,      CVehicle_GetBootOpen,      _SC("IsBootOpen"));
	// Methods
	CVehicle.AddMethod(CVehicle_Set3DArrowForPlayer, _SC("Set3DArrowForPlayer"), _SC("xxb"));
	CVehicle.AddMethod(CVehicle_Get3DArrowForPlayer, _SC("Get3DArrowForPlayer"), _SC("xx"));
}
