#include "../squirrel/SQModule.h"
#include "main.h"
#include <VCMP.h>
#include <stdarg.h>
#include <stdio.h>
#include <utils.h>
#include <inttypes.h>
#include <stdbool.h>

extern PluginFuncs* vcmpFunctions;
extern HSQAPI sq;

// -----------------------------------------------------------------------------

static SQInteger SquirrelThrow(HSQUIRRELVM v, const SQChar* format, ...)
{
	va_list ap;
	SQChar errMsg[128];
	va_start(ap, format);
	scvsprintf(errMsg, ARRAY_SIZE(errMsg), format, ap);
	va_end(ap);

	return sq->throwerror(v, errMsg);
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
	SQInteger playerId;
	if (SQ_FAILED(sq->getinteger(v, 2, &playerId)))
	{
		return sq->throwerror(v, _SC("unable to retrieve player ID"));
	}

	int32_t plrId = (int32_t)playerId;
	if (plrId < 0)
	{
		SQInteger count = 0;
		for (plrId = 0; plrId < MAX_PLAYERS; ++plrId)
		{
			if (vcmpFunctions->IsPlayerConnected(plrId) &&
				vcmpFunctions->IsPlayerSpawned(plrId) &&
				(vcmpFunctions->GetPlayerHealth(plrId) > 0.0f))
			{
				vcmpFunctions->KillPlayer(plrId);
				++count;
			}
		}

		sq->pushinteger(v, count);
		return 1;
	}
	else if (!vcmpFunctions->IsPlayerConnected(plrId))
	{
		return SquirrelThrow(v, _SC("player with ID %" PRId32 " does not exist"), plrId);
	}

	vcmpFunctions->KillPlayer(plrId);
	sq->pushinteger(v, 1);
	return 1;
}

// (int, i/f, i/f, i/f, i/f, i/f, i/f, int) -> int
static SQInteger SetPlayerCameraPos(HSQUIRRELVM v)
{
	SQInteger playerId;
	SQFloat coords[6]; // posX, posY, posZ, lookX, lookY, lookZ
	SQInteger interpTimeMS;
	if (SQ_FAILED(sq->getinteger(v, 2, &playerId)))     { return sq->throwerror(v, _SC("unable to retrieve player ID"));             }
	if (SQ_FAILED(sq->getfloat(v, 3, coords)))          { return sq->throwerror(v, _SC("unable to retrieve X position coordinate")); }
	if (SQ_FAILED(sq->getfloat(v, 4, coords + 1)))      { return sq->throwerror(v, _SC("unable to retrieve Y position coordinate")); }
	if (SQ_FAILED(sq->getfloat(v, 5, coords + 2)))      { return sq->throwerror(v, _SC("unable to retrieve Z position coordinate")); }
	if (SQ_FAILED(sq->getfloat(v, 6, coords + 3)))      { return sq->throwerror(v, _SC("unable to retrieve X look coordinate"));     }
	if (SQ_FAILED(sq->getfloat(v, 7, coords + 4)))      { return sq->throwerror(v, _SC("unable to retrieve Y look coordinate"));     }
	if (SQ_FAILED(sq->getfloat(v, 8, coords + 5)))      { return sq->throwerror(v, _SC("unable to retrieve Z look coordinate"));     }
	if (SQ_FAILED(sq->getinteger(v, 9, &interpTimeMS))) { return sq->throwerror(v, _SC("unable to retrieve interpolation time"));    }

	int32_t plrId = (int32_t)playerId;
	bool interpolate = (interpTimeMS > 0);
	if (plrId < 0)
	{
		SQInteger count = 0;
		for (plrId = 0; plrId < MAX_PLAYERS; ++plrId)
		{
			if (!vcmpFunctions->IsPlayerConnected(plrId)) { continue; }

			vcmpFunctions->SetCameraPosition(
				plrId,
				(float)coords[0], // posX
				(float)coords[1], // posY
				(float)coords[2], // posZ
				(float)coords[3], // lookX
				(float)coords[4], // lookY
				(float)coords[5]  // lookZ
			);
			if (interpolate)
			{
				vcmpFunctions->InterpolateCameraLookAt(
					plrId,
					(float)coords[3], // lookX
					(float)coords[4], // lookY
					(float)coords[5], // lookZ
					(uint32_t)interpTimeMS
				);
			}
			++count;
		}

		sq->pushinteger(v, count);
		return 1;
	}
	else if (!vcmpFunctions->IsPlayerConnected(plrId))
	{
		return SquirrelThrow(v, _SC("player with ID %" PRId32 " does not exist"), plrId);
	}

	vcmpFunctions->SetCameraPosition(
		plrId,
		(float)coords[0], // posX
		(float)coords[1], // posY
		(float)coords[2], // posZ
		(float)coords[3], // lookX
		(float)coords[4], // lookY
		(float)coords[5]  // lookZ
	);
	if (interpolate)
	{
		vcmpFunctions->InterpolateCameraLookAt(
			plrId,
			(float)coords[3], // lookX
			(float)coords[4], // lookY
			(float)coords[5], // lookZ
			(uint32_t)interpTimeMS
		);
	}
	sq->pushinteger(v, 1);
	return 1;
}

// (int, int) -> int
static SQInteger SetPlayerDrunkVisuals(HSQUIRRELVM v)
{
	SQInteger playerId;
	SQInteger drunkLevel;
	if (SQ_FAILED(sq->getinteger(v, 2, &playerId)))   { return sq->throwerror(v, _SC("unable to retrieve player ID"));       }
	if (SQ_FAILED(sq->getinteger(v, 3, &drunkLevel))) { return sq->throwerror(v, _SC("unable to retrieve drunkness level")); }

	int32_t plrId = (int32_t)playerId;
	if (plrId < 0)
	{
		SQInteger count = 0;
		for (plrId = 0; plrId < MAX_PLAYERS; ++plrId)
		{
			if (vcmpFunctions->IsPlayerConnected(plrId))
			{
				vcmpFunctions->SetPlayerDrunkVisuals(plrId, (uint8_t)drunkLevel);
				++count;
			}
		}

		sq->pushinteger(v, count);
		return 1;
	}
	else if (!vcmpFunctions->IsPlayerConnected(plrId))
	{
		return SquirrelThrow(v, _SC("player with ID %" PRId32 " does not exist"), plrId);
	}

	vcmpFunctions->SetPlayerDrunkVisuals(plrId, (uint8_t)drunkLevel);
	sq->pushinteger(v, 1);
	return 1;
}

// (int) -> int
static SQInteger GetPlayerDrunkVisuals(HSQUIRRELVM v)
{
	SQInteger playerId;
	if (SQ_FAILED(sq->getinteger(v, 2, &playerId)))
	{
		return sq->throwerror(v, _SC("unable to retrieve player ID"));
	}

	int32_t plrId = (int32_t)playerId;
	if (!vcmpFunctions->IsPlayerConnected(plrId))
	{
		return SquirrelThrow(v, _SC("player with ID %" PRId32 " does not exist"), plrId);
	}

	sq->pushinteger(v, vcmpFunctions->GetPlayerDrunkVisuals(plrId));
	return 1;
}

// (int, int) -> int
static SQInteger SetPlayerDrunkHandling(HSQUIRRELVM v)
{
	SQInteger playerId;
	SQInteger drunkLevel;
	if (SQ_FAILED(sq->getinteger(v, 2, &playerId)))   { return sq->throwerror(v, _SC("unable to retrieve player ID"));       }
	if (SQ_FAILED(sq->getinteger(v, 3, &drunkLevel))) { return sq->throwerror(v, _SC("unable to retrieve drunkness level")); }

	int32_t plrId = (int32_t)playerId;
	if (plrId < 0)
	{
		SQInteger count = 0;
		for (plrId = 0; plrId < MAX_PLAYERS; ++plrId)
		{
			if (vcmpFunctions->IsPlayerConnected(plrId))
			{
				vcmpFunctions->SetPlayerDrunkHandling(plrId, (uint32_t)drunkLevel);
				++count;
			}
		}

		sq->pushinteger(v, count);
		return 1;
	}
	else if (!vcmpFunctions->IsPlayerConnected(plrId))
	{
		return SquirrelThrow(v, _SC("player with ID %" PRId32 " does not exist"), plrId);
	}

	vcmpFunctions->SetPlayerDrunkHandling(plrId, (uint32_t)drunkLevel);
	sq->pushinteger(v, 1);
	return 1;
}

// (int) -> int
static SQInteger GetPlayerDrunkHandling(HSQUIRRELVM v)
{
	SQInteger playerId;
	if (SQ_FAILED(sq->getinteger(v, 2, &playerId)))
	{
		return sq->throwerror(v, _SC("unable to retrieve player ID"));
	}

	int32_t plrId = (int32_t)playerId;
	if (!vcmpFunctions->IsPlayerConnected(plrId))
	{
		return SquirrelThrow(v, _SC("player with ID %" PRId32 " does not exist"), plrId);
	}

	sq->pushinteger(v, vcmpFunctions->GetPlayerDrunkHandling(plrId));
	return 1;
}

// (int, bool) -> int
static SQInteger SetPlayerBleeding(HSQUIRRELVM v)
{
	SQInteger playerId;
	SQBool enable;
	if (SQ_FAILED(sq->getinteger(v, 2, &playerId))) { return sq->throwerror(v, _SC("unable to retrieve player ID"));     }
	if (SQ_FAILED(sq->getbool(v, 3, &enable)))      { return sq->throwerror(v, _SC("unable to retrieve option toggle")); }

	int32_t plrId = (int32_t)playerId;
	if (plrId < 0)
	{
		SQInteger count = 0;
		for (plrId = 0; plrId < MAX_PLAYERS; ++plrId)
		{
			if (vcmpFunctions->IsPlayerConnected(plrId))
			{
				vcmpFunctions->SetPlayerOption(plrId, vcmpPlayerOptionBleeding, (bool)enable);
				++count;
			}
		}

		sq->pushinteger(v, count);
		return 1;
	}
	else if (!vcmpFunctions->IsPlayerConnected(plrId))
	{
		return SquirrelThrow(v, _SC("player with ID %" PRId32 " does not exist"), plrId);
	}

	vcmpFunctions->SetPlayerOption(plrId, vcmpPlayerOptionBleeding, (bool)enable);
	sq->pushinteger(v, 1);
	return 1;
}

// (int) -> bool
static SQInteger IsPlayerBleeding(HSQUIRRELVM v)
{
	SQInteger playerId;
	if (SQ_FAILED(sq->getinteger(v, 2, &playerId)))
	{
		return sq->throwerror(v, _SC("unable to retrieve player ID"));
	}

	int32_t plrId = (int32_t)playerId;
	if (!vcmpFunctions->IsPlayerConnected(plrId))
	{
		return SquirrelThrow(v, _SC("player with ID %" PRId32 " does not exist"), plrId);
	}

	sq->pushbool(v, (bool)vcmpFunctions->GetPlayerOption(plrId, vcmpPlayerOptionBleeding));
	return 1;
}

// (int, int, bool) -> int
static SQInteger SetPlayer3DArrowEnabled(HSQUIRRELVM v)
{
	SQInteger targetPlayerId;
	SQInteger playerId;
	SQBool enable;
	if (SQ_FAILED(sq->getinteger(v, 2, &targetPlayerId))) { return sq->throwerror(v, _SC("unable to retrieve target player ID")); }
	if (SQ_FAILED(sq->getinteger(v, 3, &playerId)))       { return sq->throwerror(v, _SC("unable to retrieve player ID"));        }
	if (SQ_FAILED(sq->getbool(v, 4, &enable)))            { return sq->throwerror(v, _SC("unable to retrieve option toggle"));    }

	int32_t targetPlrId = (int32_t)targetPlayerId;
	if (!vcmpFunctions->IsPlayerConnected(targetPlrId))
	{
		return SquirrelThrow(v, _SC("(target) player with ID %" PRId32 " does not exist"), targetPlrId);
	}

	int32_t plrId = (int32_t)playerId;
	if (plrId < 0)
	{
		SQInteger count = 0;
		for (plrId = 0; plrId < MAX_PLAYERS; ++plrId)
		{
			if (vcmpFunctions->IsPlayerConnected(plrId) && (plrId != targetPlrId))
			{
				vcmpFunctions->SetPlayer3DArrowForPlayer(targetPlrId, plrId, (bool)enable);
				++count;
			}
		}

		sq->pushinteger(v, count);
		return 1;
	}
	else if (!vcmpFunctions->IsPlayerConnected(plrId))
	{
		return SquirrelThrow(v, _SC("player with ID %" PRId32 " does not exist"), plrId);
	}

	// Cannot set arrow for oneself.
	if (plrId == targetPlrId)
	{
		sq->pushinteger(v, 0);
		return 1;
	}

	vcmpFunctions->SetPlayer3DArrowForPlayer(targetPlrId, plrId, (bool)enable);
	sq->pushinteger(v, 1);
	return 1;
}

// (int, int) -> bool
static SQInteger IsPlayer3DArrowEnabled(HSQUIRRELVM v)
{
	SQInteger targetPlayerId;
	SQInteger playerId;
	if (SQ_FAILED(sq->getinteger(v, 2, &targetPlayerId))) { return sq->throwerror(v, _SC("unable to retrieve target player ID")); }
	if (SQ_FAILED(sq->getinteger(v, 3, &playerId)))       { return sq->throwerror(v, _SC("unable to retrieve player ID"));        }

	int32_t plrId = (int32_t)playerId;
	if (!vcmpFunctions->IsPlayerConnected(plrId))
	{
		return SquirrelThrow(v, _SC("player with ID %" PRId32 " does not exist"), plrId);
	}

	int32_t targetPlrId = (int32_t)targetPlayerId;
	if (!vcmpFunctions->IsPlayerConnected(targetPlrId))
	{
		return SquirrelThrow(v, _SC("(target) player with ID %" PRId32 " does not exist"), targetPlrId);
	}

	sq->pushbool(v, (bool)vcmpFunctions->GetPlayer3DArrowForPlayer(targetPlrId, plrId));
	return 1;
}

/* Vehicle */

// (int, bool) -> void
static SQInteger SetVehicleBonnetOpen(HSQUIRRELVM v)
{
	SQInteger vehicleId;
	SQBool enable;
	if (SQ_FAILED(sq->getinteger(v, 2, &vehicleId))) { return sq->throwerror(v, _SC("unable to retrieve vehicle ID"));    }
	if (SQ_FAILED(sq->getbool(v, 3, &enable)))       { return sq->throwerror(v, _SC("unable to retrieve option toggle")); }

	int32_t vehId = (int32_t)vehicleId;
	if (!vcmpFunctions->CheckEntityExists(vcmpEntityPoolVehicle, vehId))
	{
		return SquirrelThrow(v, _SC("vehicle with ID %" PRId32 " does not exist"), vehId);
	}

	vcmpFunctions->SetVehicleOption(vehId, vcmpVehicleOptionBonnetOpen, (bool)enable);
	return 0;
}

// (int) -> bool
static SQInteger IsVehicleBonnetOpen(HSQUIRRELVM v)
{
	SQInteger vehicleId;
	if (SQ_FAILED(sq->getinteger(v, 2, &vehicleId)))
	{
		return sq->throwerror(v, _SC("unable to retrieve vehicle ID"));
	}

	int32_t vehId = (int32_t)vehicleId;
	if (!vcmpFunctions->CheckEntityExists(vcmpEntityPoolVehicle, vehId))
	{
		return SquirrelThrow(v, _SC("vehicle with ID %" PRId32 " does not exist"), vehId);
	}

	sq->pushbool(v, (bool)vcmpFunctions->GetVehicleOption(vehId, vcmpVehicleOptionBonnetOpen));
	return 1;
}

// (int, bool) -> void
static SQInteger SetVehicleBootOpen(HSQUIRRELVM v)
{
	SQInteger vehicleId;
	SQBool enable;
	if (SQ_FAILED(sq->getinteger(v, 2, &vehicleId))) { return sq->throwerror(v, _SC("unable to retrieve vehicle ID"));    }
	if (SQ_FAILED(sq->getbool(v, 3, &enable)))       { return sq->throwerror(v, _SC("unable to retrieve option toggle")); }

	int32_t vehId = (int32_t)vehicleId;
	if (!vcmpFunctions->CheckEntityExists(vcmpEntityPoolVehicle, vehId))
	{
		return SquirrelThrow(v, _SC("vehicle with ID %" PRId32 " does not exist"), vehId);
	}

	vcmpFunctions->SetVehicleOption(vehId, vcmpVehicleOptionBootOpen, (bool)enable);
	return 0;
}

// (int) -> bool
static SQInteger IsVehicleBootOpen(HSQUIRRELVM v)
{
	SQInteger vehicleId;
	if (SQ_FAILED(sq->getinteger(v, 2, &vehicleId)))
	{
		return sq->throwerror(v, _SC("unable to retrieve vehicle ID"));
	}

	int32_t vehId = (int32_t)vehicleId;
	if (!vcmpFunctions->CheckEntityExists(vcmpEntityPoolVehicle, vehId))
	{
		return SquirrelThrow(v, _SC("vehicle with ID %" PRId32 " does not exist"), vehId);
	}

	sq->pushbool(v, (bool)vcmpFunctions->GetVehicleOption(vehId, vcmpVehicleOptionBootOpen));
	return 1;
}

// (int, int, bool) -> int
static SQInteger SetVehicle3DArrowEnabled(HSQUIRRELVM v)
{
	SQInteger vehicleId;
	SQInteger playerId;
	SQBool enable;
	if (SQ_FAILED(sq->getinteger(v, 2, &vehicleId))) { return sq->throwerror(v, _SC("unable to retrieve vehicle ID"));    }
	if (SQ_FAILED(sq->getinteger(v, 3, &playerId)))  { return sq->throwerror(v, _SC("unable to retrieve player ID"));     }
	if (SQ_FAILED(sq->getbool(v, 4, &enable)))       { return sq->throwerror(v, _SC("unable to retrieve option toggle")); }

	int32_t vehId = (int32_t)vehicleId;
	if (!vcmpFunctions->CheckEntityExists(vcmpEntityPoolVehicle, vehId))
	{
		return SquirrelThrow(v, _SC("vehicle with ID %" PRId32 " does not exist"), vehId);
	}

	int32_t plrId = (int32_t)playerId;
	if (plrId < 0)
	{
		SQInteger count = 0;
		for (plrId = 0; plrId < MAX_PLAYERS; ++plrId)
		{
			if (vcmpFunctions->IsPlayerConnected(plrId))
			{
				vcmpFunctions->SetVehicle3DArrowForPlayer(vehId, plrId, (bool)enable);
				++count;
			}
		}

		sq->pushinteger(v, count);
		return 1;
	}
	else if (!vcmpFunctions->IsPlayerConnected(plrId))
	{
		return SquirrelThrow(v, _SC("player with ID %" PRId32 " does not exist"), plrId);
	}

	vcmpFunctions->SetVehicle3DArrowForPlayer(vehId, plrId, (bool)enable);
	sq->pushinteger(v, 1);
	return 1;
}

// (int, int) -> bool
static SQInteger IsVehicle3DArrowEnabled(HSQUIRRELVM v)
{
	SQInteger vehicleId;
	SQInteger playerId;
	if (SQ_FAILED(sq->getinteger(v, 2, &vehicleId))) { return sq->throwerror(v, _SC("unable to retrieve vehicle ID")); }
	if (SQ_FAILED(sq->getinteger(v, 3, &playerId)))  { return sq->throwerror(v, _SC("unable to retrieve player ID"));  }

	int32_t vehId = (int32_t)vehicleId;
	if (!vcmpFunctions->CheckEntityExists(vcmpEntityPoolVehicle, vehId))
	{
		return SquirrelThrow(v, _SC("vehicle with ID %" PRId32 " does not exist"), vehId);
	}

	int32_t plrId = (int32_t)playerId;
	if (!vcmpFunctions->IsPlayerConnected(plrId))
	{
		return SquirrelThrow(v, _SC("player with ID %" PRId32 " does not exist"), plrId);
	}

	sq->pushbool(v, (bool)vcmpFunctions->GetVehicle3DArrowForPlayer(vehId, plrId));
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

#define REGISTER_SQ_FUNCTION(v, function, parameterMask) \
	RegisterSquirrelFunction((v), (function), _SC(#function),  SQ_MATCHTYPEMASKSTRING, _SC(parameterMask))

void RegisterSquirrelFunctions(HSQUIRRELVM v)
{
	REGISTER_SQ_FUNCTION(v, SetCrouchEnabled,         "tb");
	REGISTER_SQ_FUNCTION(v, GetCrouchEnabled,         "t");
	REGISTER_SQ_FUNCTION(v, KillPlayer,               "ti");
	REGISTER_SQ_FUNCTION(v, SetPlayerCameraPos,       "tinnnnnni");
	REGISTER_SQ_FUNCTION(v, SetPlayerDrunkVisuals,    "tii");
	REGISTER_SQ_FUNCTION(v, GetPlayerDrunkVisuals,    "ti");
	REGISTER_SQ_FUNCTION(v, SetPlayerDrunkHandling,   "tii");
	REGISTER_SQ_FUNCTION(v, GetPlayerDrunkHandling,   "ti");
	REGISTER_SQ_FUNCTION(v, SetPlayerBleeding,        "tib");
	REGISTER_SQ_FUNCTION(v, IsPlayerBleeding,         "ti");
	REGISTER_SQ_FUNCTION(v, SetPlayer3DArrowEnabled,  "tiib");
	REGISTER_SQ_FUNCTION(v, IsPlayer3DArrowEnabled,   "tii");
	REGISTER_SQ_FUNCTION(v, SetVehicleBonnetOpen,     "tib");
	REGISTER_SQ_FUNCTION(v, IsVehicleBonnetOpen,      "ti");
	REGISTER_SQ_FUNCTION(v, SetVehicleBootOpen,       "tib");
	REGISTER_SQ_FUNCTION(v, IsVehicleBootOpen,        "ti");
	REGISTER_SQ_FUNCTION(v, SetVehicle3DArrowEnabled, "tiib");
	REGISTER_SQ_FUNCTION(v, IsVehicle3DArrowEnabled,  "tii");
}

#undef REGISTER_SQ_FUNCTION
