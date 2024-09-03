#include "main.h"
#include <VCMP.h>
#include <squirrel/SQModule.h>
#include <stdbool.h>

extern PluginFuncs* vcmpFunctions;
extern HSQAPI sq;

// -----------------------------------------------------------------------------

/* Helper functions */

static SQInteger SetVehicleOptionHelper(HSQUIRRELVM v, vcmpVehicleOption option, bool invertToggle)
{
	SQInteger vehId;
	SQBool enable;
	if (SQ_FAILED(sq->getinteger(v, 2, &vehId))) { return sq->throwerror(v, _SC("unable to retrieve vehicle ID"));    }
	if (SQ_FAILED(sq->getbool(v, 3, &enable)))   { return sq->throwerror(v, _SC("unable to retrieve option toggle")); }

	int32_t vehicleId = (int32_t)vehId;
	bool toggleStatus = (invertToggle ? !enable : !!enable);

	/* Specific vehicle */
	if (vehicleId >= 0)
	{
		if (!vcmpFunctions->CheckEntityExists(vcmpEntityPoolVehicle, vehicleId))
		{
			return sq->throwerror(v, _SC("no vehicle with such ID exists"));
		}

		vcmpFunctions->SetVehicleOption(vehicleId, option, toggleStatus);
		sq->pushinteger(v, 1);
		return 1;
	}

	/* All vehicles */
	SQInteger count = 0;
	for (vehicleId = 1; vehicleId <= SQLF_MAX_VEHICLES; ++vehicleId)
	{
		if (vcmpFunctions->CheckEntityExists(vcmpEntityPoolVehicle, vehicleId))
		{
			vcmpFunctions->SetVehicleOption(vehicleId, option, toggleStatus);
			++count;
		}
	}
	sq->pushinteger(v, count);
	return 1;
}

static SQInteger GetVehicleOptionHelper(HSQUIRRELVM v, vcmpVehicleOption option, bool invertToggle)
{
	SQInteger vehId;
	if (SQ_FAILED(sq->getinteger(v, 2, &vehId)))
	{
		return sq->throwerror(v, _SC("unable to retrieve vehicle ID"));
	}

	int32_t vehicleId = (int32_t)vehId;
	if (!vcmpFunctions->CheckEntityExists(vcmpEntityPoolVehicle, vehicleId))
	{
		return sq->throwerror(v, _SC("no vehicle with such ID exists"));
	}

	sq->pushbool(v, invertToggle ?
		!vcmpFunctions->GetVehicleOption(vehicleId, option) :
		!!vcmpFunctions->GetVehicleOption(vehicleId, option));
	return 1;
}

// -----------------------------------------------------------------------------

/* Server */

// (bool) -> void
static SQInteger SetCrouchEnabled(HSQUIRRELVM v)
{
	SQBool enable;
	if (SQ_FAILED(sq->getbool(v, 2, &enable)))
	{
		return sq->throwerror(v, _SC("unable to retrieve option toggle"));
	}

	vcmpFunctions->SetServerOption(vcmpServerOptionDisableCrouch, !enable);
	return 0;
}

// (void) -> bool
static SQInteger GetCrouchEnabled(HSQUIRRELVM v)
{
	sq->pushbool(v, !vcmpFunctions->GetServerOption(vcmpServerOptionDisableCrouch));
	return 1;
}

/* Player */

// (int) -> int
static SQInteger KillPlayer(HSQUIRRELVM v)
{
	SQInteger plrId;
	if (SQ_FAILED(sq->getinteger(v, 2, &plrId)))
	{
		return sq->throwerror(v, _SC("unable to retrieve player ID"));
	}

	int32_t playerId = (int32_t)plrId;

	/* Specific player */
	if (playerId >= 0)
	{
		if (!vcmpFunctions->IsPlayerConnected(playerId))
		{
			return sq->throwerror(v, _SC("no player with such ID exists"));
		}

		// Do nothing if player is not spawned or if they are already dying.
		if (!vcmpFunctions->IsPlayerSpawned(playerId) ||
			(vcmpFunctions->GetPlayerHealth(playerId) <= 0.0f))
		{
			sq->pushinteger(v, 0);
			return 1;
		}

		vcmpFunctions->KillPlayer(playerId);
		sq->pushinteger(v, 1);
		return 1;
	}

	/* Everyone on the server */
	SQInteger count = 0;
	for (playerId = 0; playerId < SQLF_MAX_PLAYERS; ++playerId)
	{
		if (vcmpFunctions->IsPlayerConnected(playerId) &&
			vcmpFunctions->IsPlayerSpawned(playerId) &&
			(vcmpFunctions->GetPlayerHealth(playerId) > 0.0f))
		{
			vcmpFunctions->KillPlayer(playerId);
			++count;
		}
	}
	sq->pushinteger(v, count);
	return 1;
}

// (int, i/f, i/f, i/f, i/f, i/f, i/f, int) -> int
static SQInteger SetPlayerCameraPos(HSQUIRRELVM v)
{
	SQInteger plrId;
	SQFloat coords[6]; // posX, posY, posZ, lookX, lookY, lookZ
	SQInteger ms;
	if (SQ_FAILED(sq->getinteger(v, 2, &plrId)))   { return sq->throwerror(v, _SC("unable to retrieve player ID"));             }
	if (SQ_FAILED(sq->getfloat(v, 3, coords)))     { return sq->throwerror(v, _SC("unable to retrieve X position coordinate")); }
	if (SQ_FAILED(sq->getfloat(v, 4, coords + 1))) { return sq->throwerror(v, _SC("unable to retrieve Y position coordinate")); }
	if (SQ_FAILED(sq->getfloat(v, 5, coords + 2))) { return sq->throwerror(v, _SC("unable to retrieve Z position coordinate")); }
	if (SQ_FAILED(sq->getfloat(v, 6, coords + 3))) { return sq->throwerror(v, _SC("unable to retrieve X look coordinate"));     }
	if (SQ_FAILED(sq->getfloat(v, 7, coords + 4))) { return sq->throwerror(v, _SC("unable to retrieve Y look coordinate"));     }
	if (SQ_FAILED(sq->getfloat(v, 8, coords + 5))) { return sq->throwerror(v, _SC("unable to retrieve Z look coordinate"));     }
	if (SQ_FAILED(sq->getinteger(v, 9, &ms)))      { return sq->throwerror(v, _SC("unable to retrieve interpolation time"));    }

	int32_t playerId = (int32_t)plrId;
	uint32_t interpTimeMS = (uint32_t)ms;

	/* Specific player */
	if (playerId >= 0)
	{
		if (!vcmpFunctions->IsPlayerConnected(playerId))
		{
			return sq->throwerror(v, _SC("no player with such ID exists"));
		}

		vcmpFunctions->SetCameraPosition(
			playerId,
			(float)coords[0], // posX
			(float)coords[1], // posY
			(float)coords[2], // posZ
			(float)coords[3], // lookX
			(float)coords[4], // lookY
			(float)coords[5]  // lookZ
		);
		if (interpTimeMS)
		{
			vcmpFunctions->InterpolateCameraLookAt(
				playerId,
				(float)coords[3], // lookX
				(float)coords[4], // lookY
				(float)coords[5], // lookZ
				interpTimeMS
			);
		}
		sq->pushinteger(v, 1);
		return 1;
	}

	/* Everyone on the server */
	SQInteger count = 0;
	for (playerId = 0; playerId < SQLF_MAX_PLAYERS; ++playerId)
	{
		if (!vcmpFunctions->IsPlayerConnected(playerId)) { continue; }

		vcmpFunctions->SetCameraPosition(
			playerId,
			(float)coords[0], // posX
			(float)coords[1], // posY
			(float)coords[2], // posZ
			(float)coords[3], // lookX
			(float)coords[4], // lookY
			(float)coords[5]  // lookZ
		);
		if (interpTimeMS)
		{
			vcmpFunctions->InterpolateCameraLookAt(
				playerId,
				(float)coords[3], // lookX
				(float)coords[4], // lookY
				(float)coords[5], // lookZ
				interpTimeMS
			);
		}
		++count;
	}
	sq->pushinteger(v, count);
	return 1;
}

// (int, int) -> int
static SQInteger SetPlayerDrunkVisuals(HSQUIRRELVM v)
{
	SQInteger plrId;
	SQInteger drunkLevel;
	if (SQ_FAILED(sq->getinteger(v, 2, &plrId)))      { return sq->throwerror(v, _SC("unable to retrieve player ID"));       }
	if (SQ_FAILED(sq->getinteger(v, 3, &drunkLevel))) { return sq->throwerror(v, _SC("unable to retrieve drunkness level")); }

	int32_t playerId = (int32_t)plrId;

	/* Specific player */
	if (playerId >= 0)
	{
		if (!vcmpFunctions->IsPlayerConnected(playerId))
		{
			return sq->throwerror(v, _SC("no player with such ID exists"));
		}

		vcmpFunctions->SetPlayerDrunkVisuals(playerId, (uint8_t)drunkLevel);
		sq->pushinteger(v, 1);
		return 1;
	}

	/* Everyone on the server */
	SQInteger count = 0;
	for (playerId = 0; playerId < SQLF_MAX_PLAYERS; ++playerId)
	{
		if (vcmpFunctions->IsPlayerConnected(playerId))
		{
			vcmpFunctions->SetPlayerDrunkVisuals(playerId, (uint8_t)drunkLevel);
			++count;
		}
	}
	sq->pushinteger(v, count);
	return 1;
}

// (int) -> int
static SQInteger GetPlayerDrunkVisuals(HSQUIRRELVM v)
{
	SQInteger plrId;
	if (SQ_FAILED(sq->getinteger(v, 2, &plrId)))
	{
		return sq->throwerror(v, _SC("unable to retrieve player ID"));
	}

	int32_t playerId = (int32_t)plrId;
	if (!vcmpFunctions->IsPlayerConnected(playerId))
	{
		return sq->throwerror(v, _SC("no player with such ID exists"));
	}

	sq->pushinteger(v, vcmpFunctions->GetPlayerDrunkVisuals(playerId));
	return 1;
}

// (int, int) -> int
static SQInteger SetPlayerDrunkHandling(HSQUIRRELVM v)
{
	SQInteger plrId;
	SQInteger drunkLevel;
	if (SQ_FAILED(sq->getinteger(v, 2, &plrId)))      { return sq->throwerror(v, _SC("unable to retrieve player ID"));       }
	if (SQ_FAILED(sq->getinteger(v, 3, &drunkLevel))) { return sq->throwerror(v, _SC("unable to retrieve drunkness level")); }

	int32_t playerId = (int32_t)plrId;

	/* Specific player */
	if (playerId >= 0)
	{
		if (!vcmpFunctions->IsPlayerConnected(playerId))
		{
			return sq->throwerror(v, _SC("no player with such ID exists"));
		}

		vcmpFunctions->SetPlayerDrunkHandling(playerId, (uint32_t)drunkLevel);
		sq->pushinteger(v, 1);
		return 1;
	}

	/* Everyone on the server */
	SQInteger count = 0;
	for (playerId = 0; playerId < SQLF_MAX_PLAYERS; ++playerId)
	{
		if (vcmpFunctions->IsPlayerConnected(playerId))
		{
			vcmpFunctions->SetPlayerDrunkHandling(playerId, (uint32_t)drunkLevel);
			++count;
		}
	}
	sq->pushinteger(v, count);
	return 1;
}

// (int) -> int
static SQInteger GetPlayerDrunkHandling(HSQUIRRELVM v)
{
	SQInteger plrId;
	if (SQ_FAILED(sq->getinteger(v, 2, &plrId)))
	{
		return sq->throwerror(v, _SC("unable to retrieve player ID"));
	}

	int32_t playerId = (int32_t)plrId;
	if (!vcmpFunctions->IsPlayerConnected(playerId))
	{
		return sq->throwerror(v, _SC("no player with such ID exists"));
	}

	sq->pushinteger(v, (SQInteger)vcmpFunctions->GetPlayerDrunkHandling(playerId));
	return 1;
}

// (int, bool) -> int
static SQInteger SetPlayerBleeding(HSQUIRRELVM v)
{
	SQInteger plrId;
	SQBool enable;
	if (SQ_FAILED(sq->getinteger(v, 2, &plrId))) { return sq->throwerror(v, _SC("unable to retrieve player ID"));     }
	if (SQ_FAILED(sq->getbool(v, 3, &enable)))   { return sq->throwerror(v, _SC("unable to retrieve option toggle")); }

	int32_t playerId = (int32_t)plrId;

	/* Specific player */
	if (playerId >= 0)
	{
		if (!vcmpFunctions->IsPlayerConnected(playerId))
		{
			return sq->throwerror(v, _SC("no player with such ID exists"));
		}

		vcmpFunctions->SetPlayerOption(playerId, vcmpPlayerOptionBleeding, !!enable);
		sq->pushinteger(v, 1);
		return 1;
	}

	/* Everyone on the server */
	SQInteger count = 0;
	for (playerId = 0; playerId < SQLF_MAX_PLAYERS; ++playerId)
	{
		if (vcmpFunctions->IsPlayerConnected(playerId))
		{
			vcmpFunctions->SetPlayerOption(playerId, vcmpPlayerOptionBleeding, !!enable);
			++count;
		}
	}
	sq->pushinteger(v, count);
	return 1;
}

// (int) -> bool
static SQInteger IsPlayerBleeding(HSQUIRRELVM v)
{
	SQInteger plrId;
	if (SQ_FAILED(sq->getinteger(v, 2, &plrId)))
	{
		return sq->throwerror(v, _SC("unable to retrieve player ID"));
	}

	int32_t playerId = (int32_t)plrId;
	if (!vcmpFunctions->IsPlayerConnected(playerId))
	{
		return sq->throwerror(v, _SC("no player with such ID exists"));
	}

	sq->pushbool(v, !!vcmpFunctions->GetPlayerOption(playerId, vcmpPlayerOptionBleeding));
	return 1;
}

// (int, int, bool) -> int
static SQInteger SetPlayer3DArrowEnabled(HSQUIRRELVM v)
{
	SQInteger plrId;
	SQInteger plrToShowId;
	SQBool enable;
	if (SQ_FAILED(sq->getinteger(v, 2, &plrId)))       { return sq->throwerror(v, _SC("unable to retrieve player ID"));           }
	if (SQ_FAILED(sq->getinteger(v, 3, &plrToShowId))) { return sq->throwerror(v, _SC("unable to retrieve player (to show) ID")); }
	if (SQ_FAILED(sq->getbool(v, 4, &enable)))         { return sq->throwerror(v, _SC("unable to retrieve option toggle"));       }

	int32_t playerId = (int32_t)plrId;
	if (!vcmpFunctions->IsPlayerConnected(playerId))
	{
		return sq->throwerror(v, _SC("no player with such ID exists"));
	}

	int32_t playerToShowId = (int32_t)plrToShowId;

	/* Specific player */
	if (playerToShowId >= 0)
	{
		if (!vcmpFunctions->IsPlayerConnected(playerToShowId))
		{
			return sq->throwerror(v, _SC("no player (to show) with such ID exists"));
		}

		// Cannot set arrow for oneself.
		if (playerToShowId == playerId)
		{
			sq->pushinteger(v, 0);
			return 1;
		}

		vcmpFunctions->SetPlayer3DArrowForPlayer(playerId, playerToShowId, !!enable);
		sq->pushinteger(v, 1);
		return 1;
	}

	/* Everyone on the server */
	SQInteger count = 0;
	for (playerToShowId = 0; playerToShowId < SQLF_MAX_PLAYERS; ++playerToShowId)
	{
		if (vcmpFunctions->IsPlayerConnected(playerToShowId) && (playerToShowId != playerId))
		{
			vcmpFunctions->SetPlayer3DArrowForPlayer(playerId, playerToShowId, !!enable);
			++count;
		}
	}
	sq->pushinteger(v, count);
	return 1;
}

// (int, int) -> bool
static SQInteger IsPlayer3DArrowEnabled(HSQUIRRELVM v)
{
	SQInteger plrId;
	SQInteger plrToShowId;
	if (SQ_FAILED(sq->getinteger(v, 2, &plrId)))       { return sq->throwerror(v, _SC("unable to retrieve player ID"));           }
	if (SQ_FAILED(sq->getinteger(v, 3, &plrToShowId))) { return sq->throwerror(v, _SC("unable to retrieve player (to show) ID")); }

	int32_t playerId = (int32_t)plrId;
	if (!vcmpFunctions->IsPlayerConnected(playerId))
	{
		return sq->throwerror(v, _SC("no player with such ID exists"));
	}

	int32_t playerToShowId = (int32_t)plrToShowId;
	if (!vcmpFunctions->IsPlayerConnected(playerToShowId))
	{
		return sq->throwerror(v, _SC("no player (to show) with such ID exists"));
	}

	sq->pushbool(v, !!vcmpFunctions->GetPlayer3DArrowForPlayer(playerId, playerToShowId));
	return 1;
}

/* Vehicle */

// (int, bool) -> int
static SQInteger SetVehicleEngineEnabled(HSQUIRRELVM v)
{
	return SetVehicleOptionHelper(v, vcmpVehicleOptionEngineDisabled, true);
}

// (int) -> bool
static SQInteger IsVehicleEngineEnabled(HSQUIRRELVM v)
{
	return GetVehicleOptionHelper(v, vcmpVehicleOptionEngineDisabled, true);
}

// (int, bool) -> int
static SQInteger SetVehicleBonnetOpen(HSQUIRRELVM v)
{
	return SetVehicleOptionHelper(v, vcmpVehicleOptionBonnetOpen, false);
}

// (int) -> bool
static SQInteger IsVehicleBonnetOpen(HSQUIRRELVM v)
{
	return GetVehicleOptionHelper(v, vcmpVehicleOptionBonnetOpen, false);
}

// (int, bool) -> int
static SQInteger SetVehicleBootOpen(HSQUIRRELVM v)
{
	return SetVehicleOptionHelper(v, vcmpVehicleOptionBootOpen, false);
}

// (int) -> bool
static SQInteger IsVehicleBootOpen(HSQUIRRELVM v)
{
	return GetVehicleOptionHelper(v, vcmpVehicleOptionBootOpen, false);
}

// (int, int, bool) -> int
static SQInteger SetVehicle3DArrowEnabled(HSQUIRRELVM v)
{
	SQInteger vehId;
	SQInteger plrToShowId;
	SQBool enable;
	if (SQ_FAILED(sq->getinteger(v, 2, &vehId)))       { return sq->throwerror(v, _SC("unable to retrieve vehicle ID"));          }
	if (SQ_FAILED(sq->getinteger(v, 3, &plrToShowId))) { return sq->throwerror(v, _SC("unable to retrieve player (to show) ID")); }
	if (SQ_FAILED(sq->getbool(v, 4, &enable)))         { return sq->throwerror(v, _SC("unable to retrieve option toggle"));       }

	int32_t vehicleId = (int32_t)vehId;
	if (!vcmpFunctions->CheckEntityExists(vcmpEntityPoolVehicle, vehicleId))
	{
		return sq->throwerror(v, _SC("no vehicle with such ID exists"));
	}

	int32_t playerToShowId = (int32_t)plrToShowId;

	/* Specific player */
	if (playerToShowId >= 0)
	{
		if (!vcmpFunctions->IsPlayerConnected(playerToShowId))
		{
			return sq->throwerror(v, _SC("no player (to show) with such ID exists"));
		}

		vcmpFunctions->SetVehicle3DArrowForPlayer(vehicleId, playerToShowId, !!enable);
		sq->pushinteger(v, 1);
		return 1;
	}

	/* Everyone on the server */
	SQInteger count = 0;
	for (playerToShowId = 0; playerToShowId < SQLF_MAX_PLAYERS; ++playerToShowId)
	{
		if (vcmpFunctions->IsPlayerConnected(playerToShowId))
		{
			vcmpFunctions->SetVehicle3DArrowForPlayer(vehicleId, playerToShowId, !!enable);
			++count;
		}
	}
	sq->pushinteger(v, count);
	return 1;
}

// (int, int) -> bool
static SQInteger IsVehicle3DArrowEnabled(HSQUIRRELVM v)
{
	SQInteger vehId;
	SQInteger plrToShowId;
	if (SQ_FAILED(sq->getinteger(v, 2, &vehId)))       { return sq->throwerror(v, _SC("unable to retrieve vehicle ID"));          }
	if (SQ_FAILED(sq->getinteger(v, 3, &plrToShowId))) { return sq->throwerror(v, _SC("unable to retrieve player (to show) ID")); }

	int32_t vehicleId = (int32_t)vehId;
	if (!vcmpFunctions->CheckEntityExists(vcmpEntityPoolVehicle, vehicleId))
	{
		return sq->throwerror(v, _SC("no vehicle with such ID exists"));
	}

	int32_t playerToShowId = (int32_t)plrToShowId;
	if (!vcmpFunctions->IsPlayerConnected(playerToShowId))
	{
		return sq->throwerror(v, _SC("no player (to show) with such ID exists"));
	}

	sq->pushbool(v, !!vcmpFunctions->GetVehicle3DArrowForPlayer(vehicleId, playerToShowId));
	return 1;
}

// -----------------------------------------------------------------------------

// The following function was taken from here:
// https://forum.vc-mp.org/index.php?topic=206.msg3206#msg3206
// It is, however, slightly modified for convenience.
static void RegisterSquirrelFunction(HSQUIRRELVM v, SQFUNCTION function, const SQChar* functionName,
	SQInteger parameterCount, const SQChar* parameterMask)
{
	sq->pushroottable(v);
	sq->pushstring(v, functionName, -1);
	sq->newclosure(v, function, 0);
	sq->setparamscheck(v, parameterCount, parameterMask);
	sq->setnativeclosurename(v, -1, functionName);
	sq->newslot(v, -3, SQFalse);
	sq->pop(v, 1);
}

#define REGISTER_SQ_FUNCTION(function, parameterMask) \
	RegisterSquirrelFunction(v, (function), _SC(#function), SQ_MATCHTYPEMASKSTRING, _SC("t" parameterMask))

void SQLF_RegisterSquirrelFunctions(HSQUIRRELVM v)
{
	REGISTER_SQ_FUNCTION(SetCrouchEnabled,         "b");
	REGISTER_SQ_FUNCTION(GetCrouchEnabled,         "");
	REGISTER_SQ_FUNCTION(KillPlayer,               "i");
	REGISTER_SQ_FUNCTION(SetPlayerCameraPos,       "innnnnni");
	REGISTER_SQ_FUNCTION(SetPlayerDrunkVisuals,    "ii");
	REGISTER_SQ_FUNCTION(GetPlayerDrunkVisuals,    "i");
	REGISTER_SQ_FUNCTION(SetPlayerDrunkHandling,   "ii");
	REGISTER_SQ_FUNCTION(GetPlayerDrunkHandling,   "i");
	REGISTER_SQ_FUNCTION(SetPlayerBleeding,        "ib");
	REGISTER_SQ_FUNCTION(IsPlayerBleeding,         "i");
	REGISTER_SQ_FUNCTION(SetPlayer3DArrowEnabled,  "iib");
	REGISTER_SQ_FUNCTION(IsPlayer3DArrowEnabled,   "ii");
	REGISTER_SQ_FUNCTION(SetVehicleEngineEnabled,  "ib");
	REGISTER_SQ_FUNCTION(IsVehicleEngineEnabled,   "i");
	REGISTER_SQ_FUNCTION(SetVehicleBonnetOpen,     "ib");
	REGISTER_SQ_FUNCTION(IsVehicleBonnetOpen,      "i");
	REGISTER_SQ_FUNCTION(SetVehicleBootOpen,       "ib");
	REGISTER_SQ_FUNCTION(IsVehicleBootOpen,        "i");
	REGISTER_SQ_FUNCTION(SetVehicle3DArrowEnabled, "iib");
	REGISTER_SQ_FUNCTION(IsVehicle3DArrowEnabled,  "ii");
}

#undef REGISTER_SQ_FUNCTION
