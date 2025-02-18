/*
 * Latest Vice City: Multiplayer (VC:MP) 0.4 features for Squirrel
 * Author: sfwidde ([SS]Kelvin)
 * 2024-07-07
 */

#include "main.hpp"
#include "console.hpp"
#include <VCMP.h>
#include <squirrel/SQImports.h>
#include <stdio.h>
#include <utils.hpp>
#include <assert.h>
#include <string.h>

PluginFuncs* vcmpFunctions;
HSQAPI sq;

// -----------------------------------------------------------------------------

/* functions.cpp */
void RegisterNewSquirrelDefinitions(HSQUIRRELVM v);

// -----------------------------------------------------------------------------

static void HookSquirrel()
{
	// Attempt to find Squirrel plugin
	int32_t pluginId = vcmpFunctions->FindPlugin("SQHost2");
	if (pluginId == -1)
	{
		OUTPUT_WARNING("Unable to find Squirrel module. " PLUGIN_NAME " definitions will be unavailable.");
		return;
	}

	// Attempt to retrieve exports data
	size_t dataSize;
	const void** data = vcmpFunctions->GetPluginExports(pluginId, &dataSize);
	if (!data || !*data || dataSize != sizeof(SquirrelImports))
	{
		OUTPUT_WARNING("The Squirrel module provided an invalid SquirrelImports structure. "
			PLUGIN_NAME " definitions will be unavailable.");
		return;
	}

	// Do the hook
	SquirrelImports* sqImports = *(SquirrelImports**)data;
	sq = *sqImports->GetSquirrelAPI();
	RegisterNewSquirrelDefinitions(*sqImports->GetSquirrelVM());
}

// -----------------------------------------------------------------------------

static uint8_t OnServerInitialise()
{
	putchar('\n');
	OUTPUT_INFO("Loaded " PLUGIN_NAME " v" PLUGIN_VERSION_STR " by sfwidde ([SS]Kelvin).");
	return 1;
}

static uint8_t OnPluginCommand(uint32_t commandIdentifier, const char* message)
{
	switch (commandIdentifier)
	{
	case 0x7D6E22D8: // Squirrel scripts load
		HookSquirrel();
	default:
		return 1;
	}
}

// https://forum.vc-mp.org/index.php?topic=13.0
BEGIN_C_LINKAGE
LIBRARY_EXPORT uint32_t VcmpPluginInit(PluginFuncs* pluginFuncs, PluginCallbacks* pluginCalls, PluginInfo* pluginInfo)
{
	// Set up plugin info
	assert(sizeof(PLUGIN_NAME) <= sizeof(pluginInfo->name));
	strcpy(pluginInfo->name, PLUGIN_NAME);
	pluginInfo->pluginVersion = PLUGIN_VERSION_INT;
	pluginInfo->apiMajorVersion = PLUGIN_API_MAJOR;
	pluginInfo->apiMinorVersion = PLUGIN_API_MINOR;

	// VC:MP functions are now accessible to us
	vcmpFunctions = pluginFuncs;

	// Set up plugin callbacks
	pluginCalls->OnServerInitialise = OnServerInitialise;
	pluginCalls->OnPluginCommand = OnPluginCommand;

	return 1;
}
END_C_LINKAGE

// -----------------------------------------------------------------------------
