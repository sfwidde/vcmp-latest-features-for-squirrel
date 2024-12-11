/*
 * Latest Vice City: Multiplayer (VC:MP) 0.4 features for Squirrel
 * Author: sfwidde ([SS]Kelvin)
 * 2024-07-07
 */

#include "main.h"
#include "sqhook.h"
#include <VCMP.h>
#include <squirrel/SQModule.h>

extern PluginFuncs* vcmpFunctions;
extern HSQAPI sq;

// -----------------------------------------------------------------------------

/*
 * ----------------
 * Helper functions
 * ----------------
 */

// Get Vector instance off the Squirrel stack
static VectorHook* GetVectorInstance(HSQUIRRELVM v, SQInteger stackIndex)
{
	SQUserPointer p;
	sq->getinstanceup(v, stackIndex, &p, NULL);
	return (VectorHook*)p;
}

// Get CPlayer instance off the Squirrel stack
static CPlayerHook* GetCPlayerInstance(HSQUIRRELVM v, SQInteger stackIndex)
{
	SQUserPointer p;
	sq->getinstanceup(v, stackIndex, &p, NULL);
	return (CPlayerHook*)p;
}

// Get CVehicle instance off the Squirrel stack
static CVehicleHook* GetCVehicleInstance(HSQUIRRELVM v, SQInteger stackIndex)
{
	SQUserPointer p;
	sq->getinstanceup(v, stackIndex, &p, NULL);
	return (CVehicleHook*)p;
}

// -----------------------------------------------------------------------------

/*
 * ----------------
 * Global functions
 * ----------------
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
 * -----------------------------------
 * CPlayer class read-write properties
 * -----------------------------------
 */

/* (bool) -> void */
static SQInteger CPlayer_SetBleeding(HSQUIRRELVM v)
{
	SQBool enable;
	sq->tobool(v, 2, &enable);
	vcmpFunctions->SetPlayerOption(GetCPlayerInstance(v, 1)->nPlayerId, vcmpPlayerOptionBleeding, !!enable);
	return 0;
}

/* (void) -> bool */
static SQInteger CPlayer_GetBleeding(HSQUIRRELVM v)
{
	sq->pushbool(v, !!vcmpFunctions->GetPlayerOption(GetCPlayerInstance(v, 1)->nPlayerId, vcmpPlayerOptionBleeding));
	return 1;
}

/* (int) -> void */
static SQInteger CPlayer_SetDrunkVisuals(HSQUIRRELVM v)
{
	SQInteger drunkLevel = 0;
	sq->getinteger(v, 2, &drunkLevel);
	vcmpFunctions->SetPlayerDrunkVisuals(GetCPlayerInstance(v, 1)->nPlayerId, (uint8_t)drunkLevel);
	return 0;
}

/* (void) -> int */
static SQInteger CPlayer_GetDrunkVisuals(HSQUIRRELVM v)
{
	sq->pushinteger(v, vcmpFunctions->GetPlayerDrunkVisuals(GetCPlayerInstance(v, 1)->nPlayerId));
	return 1;
}

/* (int) -> void */
static SQInteger CPlayer_SetDrunkHandling(HSQUIRRELVM v)
{
	SQInteger drunkLevel = 0;
	sq->getinteger(v, 2, &drunkLevel);
	vcmpFunctions->SetPlayerDrunkHandling(GetCPlayerInstance(v, 1)->nPlayerId, (uint32_t)drunkLevel);
	return 0;
}

/* (void) -> int */
static SQInteger CPlayer_GetDrunkHandling(HSQUIRRELVM v)
{
	sq->pushinteger(v, (SQInteger)vcmpFunctions->GetPlayerDrunkHandling(GetCPlayerInstance(v, 1)->nPlayerId));
	return 1;
}

/*
 * ---------------------
 * CPlayer class methods
 * ---------------------
 */

/* (void) -> void */
static SQInteger CPlayer_Kill(HSQUIRRELVM v)
{
	vcmpFunctions->KillPlayer(GetCPlayerInstance(v, 1)->nPlayerId);
	return 0;
}

/* (Vector, Vector[, int]) -> void */
static SQInteger CPlayer_SetCameraPos(HSQUIRRELVM v)
{
	if (!SQLF_IsSquirrelObjectInstanceOfClass(v, 2, SQID_VECTOR) &&
		!SQLF_IsSquirrelObjectInstanceOfClass(v, 2, SQID_ENTITYVECTOR))
	{
		return sq->throwerror(v, _SC("camera position must be a " SQID_VECTOR " " 
			"or an " SQID_ENTITYVECTOR " class instance"));
	}
	if (!SQLF_IsSquirrelObjectInstanceOfClass(v, 3, SQID_VECTOR) &&
		!SQLF_IsSquirrelObjectInstanceOfClass(v, 3, SQID_ENTITYVECTOR))
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

	const CPlayerHook* player = GetCPlayerInstance(v, 1);
	const VectorHook* cameraPos = GetVectorInstance(v, 2);
	const VectorHook* cameraLook = GetVectorInstance(v, 3);
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
	if (!SQLF_IsSquirrelObjectInstanceOfClass(v, 2, SQID_PLAYER)) {
		return sq->throwerror(v, _SC("player to show must be a " SQID_PLAYER " class instance"));
	}

	SQBool enable;
	sq->getbool(v, 3, &enable);
	vcmpFunctions->SetPlayer3DArrowForPlayer(
		GetCPlayerInstance(v, 1)->nPlayerId,
		GetCPlayerInstance(v, 2)->nPlayerId,
		!!enable
	);
	return 0;
}

/* (CPlayer) -> bool */
static SQInteger CPlayer_Get3DArrowForPlayer(HSQUIRRELVM v)
{
	if (!SQLF_IsSquirrelObjectInstanceOfClass(v, 2, SQID_PLAYER)) {
		return sq->throwerror(v, _SC("player to show must be a " SQID_PLAYER " class instance"));
	}

	sq->pushbool(v,
		!!vcmpFunctions->GetPlayer3DArrowForPlayer(
			GetCPlayerInstance(v, 1)->nPlayerId,
			GetCPlayerInstance(v, 2)->nPlayerId
		));
	return 1;
}

/*
 * ------------------------------------
 * CVehicle class read-write properties
 * ------------------------------------
 */

/* (bool) -> void */
static SQInteger CVehicle_SetEngineEnabled(HSQUIRRELVM v)
{
	SQBool enable;
	sq->tobool(v, 2, &enable);
	vcmpFunctions->SetVehicleOption(GetCVehicleInstance(v, 1)->nVehicleId, vcmpVehicleOptionEngineDisabled, !enable);
	return 0;
}

/* (void) -> bool */
static SQInteger CVehicle_GetEngineEnabled(HSQUIRRELVM v)
{
	sq->pushbool(v, !vcmpFunctions->GetVehicleOption(GetCVehicleInstance(v, 1)->nVehicleId, vcmpVehicleOptionEngineDisabled));
	return 1;
}

/* (bool) -> void */
static SQInteger CVehicle_SetBonnetOpen(HSQUIRRELVM v)
{
	SQBool enable;
	sq->tobool(v, 2, &enable);
	vcmpFunctions->SetVehicleOption(GetCVehicleInstance(v, 1)->nVehicleId, vcmpVehicleOptionBonnetOpen, !!enable);
	return 0;
}

/* (void) -> bool */
static SQInteger CVehicle_GetBonnetOpen(HSQUIRRELVM v)
{
	sq->pushbool(v, !!vcmpFunctions->GetVehicleOption(GetCVehicleInstance(v, 1)->nVehicleId, vcmpVehicleOptionBonnetOpen));
	return 1;
}

/* (bool) -> void */
static SQInteger CVehicle_SetBootOpen(HSQUIRRELVM v)
{
	SQBool enable;
	sq->tobool(v, 2, &enable);
	vcmpFunctions->SetVehicleOption(GetCVehicleInstance(v, 1)->nVehicleId, vcmpVehicleOptionBootOpen, !!enable);
	return 0;
}

/* (void) -> bool */
static SQInteger CVehicle_GetBootOpen(HSQUIRRELVM v)
{
	sq->pushbool(v, !!vcmpFunctions->GetVehicleOption(GetCVehicleInstance(v, 1)->nVehicleId, vcmpVehicleOptionBootOpen));
	return 1;
}

/*
 * ----------------------
 * CVehicle class methods
 * ----------------------
 */

/* (CPlayer, bool) -> void */
static SQInteger CVehicle_Set3DArrowForPlayer(HSQUIRRELVM v)
{
	if (!SQLF_IsSquirrelObjectInstanceOfClass(v, 2, SQID_PLAYER)) {
		return sq->throwerror(v, _SC("player to show must be a " SQID_PLAYER " class instance"));
	}

	SQBool enable;
	sq->getbool(v, 3, &enable);
	vcmpFunctions->SetVehicle3DArrowForPlayer(
		GetCVehicleInstance(v, 1)->nVehicleId,
		GetCPlayerInstance(v, 2)->nPlayerId,
		!!enable
	);
	return 0;
}

/* (CPlayer) -> bool */
static SQInteger CVehicle_Get3DArrowForPlayer(HSQUIRRELVM v)
{
	if (!SQLF_IsSquirrelObjectInstanceOfClass(v, 2, SQID_PLAYER)) {
		return sq->throwerror(v, _SC("player to show must be a " SQID_PLAYER " class instance"));
	}

	sq->pushbool(v,
		!!vcmpFunctions->GetVehicle3DArrowForPlayer(
			GetCVehicleInstance(v, 1)->nVehicleId,
			GetCPlayerInstance(v, 2)->nPlayerId
		));
	return 1;
}

// -----------------------------------------------------------------------------

void SQLF_RegisterNewSquirrelDefinitions(HSQUIRRELVM v)
{
	// TODO: Can this mess be improved?

	/* Global functions */
	SQLF_RegisterSquirrelFunction(v, SetCrouchEnabled, _SC("SetCrouchEnabled"), SQ_MATCHTYPEMASKSTRING, _SC("tb"));
	SQLF_RegisterSquirrelFunction(v, GetCrouchEnabled, _SC("GetCrouchEnabled"), SQ_MATCHTYPEMASKSTRING, _SC("t"));

	/* CPlayer class read-write properties */
	SQLF_RegisterSqratClassProperty(v, CPlayer_SetBleeding,      CPlayer_GetBleeding,      SQID_PLAYER, _SC("Bleeding"));
	SQLF_RegisterSqratClassProperty(v, CPlayer_SetBleeding,      CPlayer_GetBleeding,      SQID_PLAYER, _SC("IsBleeding"));
	SQLF_RegisterSqratClassProperty(v, CPlayer_SetDrunkVisuals,  CPlayer_GetDrunkVisuals,  SQID_PLAYER, _SC("DrunkVisuals"));
	SQLF_RegisterSqratClassProperty(v, CPlayer_SetDrunkHandling, CPlayer_GetDrunkHandling, SQID_PLAYER, _SC("DrunkHandling"));
	/* CPlayer class methods */
	SQLF_RegisterSquirrelClassMethod(v, CPlayer_Kill,                SQID_PLAYER, _SC("Kill"),                SQ_MATCHTYPEMASKSTRING, _SC("x"));
	SQLF_RegisterSquirrelClassMethod(v, CPlayer_SetCameraPos,        SQID_PLAYER, _SC("SetCameraPos"),        -3,                     _SC("xxxi"));
	SQLF_RegisterSquirrelClassMethod(v, CPlayer_Set3DArrowForPlayer, SQID_PLAYER, _SC("Set3DArrowForPlayer"), SQ_MATCHTYPEMASKSTRING, _SC("xxb"));
	SQLF_RegisterSquirrelClassMethod(v, CPlayer_Get3DArrowForPlayer, SQID_PLAYER, _SC("Get3DArrowForPlayer"), SQ_MATCHTYPEMASKSTRING, _SC("xx"));

	/* CVehicle class read-write properties */
	SQLF_RegisterSqratClassProperty(v, CVehicle_SetEngineEnabled, CVehicle_GetEngineEnabled, SQID_VEHICLE, _SC("EngineEnabled"));
	SQLF_RegisterSqratClassProperty(v, CVehicle_SetEngineEnabled, CVehicle_GetEngineEnabled, SQID_VEHICLE, _SC("IsEngineEnabled"));
	SQLF_RegisterSqratClassProperty(v, CVehicle_SetBonnetOpen,    CVehicle_GetBonnetOpen,    SQID_VEHICLE, _SC("BonnetOpen"));
	SQLF_RegisterSqratClassProperty(v, CVehicle_SetBonnetOpen,    CVehicle_GetBonnetOpen,    SQID_VEHICLE, _SC("IsBonnetOpen"));
	SQLF_RegisterSqratClassProperty(v, CVehicle_SetBootOpen,      CVehicle_GetBootOpen,      SQID_VEHICLE, _SC("BootOpen"));
	SQLF_RegisterSqratClassProperty(v, CVehicle_SetBootOpen,      CVehicle_GetBootOpen,      SQID_VEHICLE, _SC("IsBootOpen"));
	/* CVehicle class methods */
	SQLF_RegisterSquirrelClassMethod(v, CVehicle_Set3DArrowForPlayer, SQID_VEHICLE, _SC("Set3DArrowForPlayer"), SQ_MATCHTYPEMASKSTRING, _SC("xxb"));
	SQLF_RegisterSquirrelClassMethod(v, CVehicle_Get3DArrowForPlayer, SQID_VEHICLE, _SC("Get3DArrowForPlayer"), SQ_MATCHTYPEMASKSTRING, _SC("xx"));
}
