#include "main.h"
#include "console.h"
#include <VCMP.h>
#include <squirrel/SQImports.h>
#include <stdio.h>
#include <utils.h>
#include <assert.h>
#include <string.h>

PluginFuncs* vcmpFunctions;
HSQAPI sq;

/* functions.c */
void RegisterSQLatestFeaturesFunctions(HSQUIRRELVM v);

// -----------------------------------------------------------------------------

static void HookSquirrel(void)
{
	// Attempt to find Squirrel plugin.
	int32_t pluginId = vcmpFunctions->FindPlugin("SQHost2");
	if (pluginId == -1)
	{
		OUTPUT_ERROR("Unable to find Squirrel module. " PLUGIN_NAME " functions will be unavailable.");
		return;
	}

	// Attempt to retrieve exports data.
	size_t dataSize;
	const void** data = vcmpFunctions->GetPluginExports(pluginId, &dataSize);
	if (!data || !*data || dataSize != sizeof(SquirrelImports))
	{
		OUTPUT_ERROR("The Squirrel module provided an invalid SquirrelImports structure. "
			PLUGIN_NAME " functions will be unavailable.");
		return;
	}

	// Do the hook.
	SquirrelImports* sqImports = *(SquirrelImports**)data;
	sq = *sqImports->GetSquirrelAPI();
	RegisterSQLatestFeaturesFunctions(*sqImports->GetSquirrelVM());
}

// -----------------------------------------------------------------------------

static uint8_t OnServerInitialise(void)
{
	putchar('\n');
	OUTPUT_INFO("Loaded " PLUGIN_NAME " module v" PLUGIN_VERSION_STR " by sfwidde ([R3V]Kelvin).");
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
LIBRARY_EXPORT uint32_t VcmpPluginInit(PluginFuncs* pluginFuncs, PluginCallbacks* pluginCalls, PluginInfo* pluginInfo)
{
	assert(sizeof(PLUGIN_NAME) <= sizeof(pluginInfo->name));
	strcpy(pluginInfo->name, PLUGIN_NAME);
	pluginInfo->pluginVersion = PLUGIN_VERSION_INT;
	pluginInfo->apiMajorVersion = PLUGIN_API_MAJOR;
	pluginInfo->apiMinorVersion = PLUGIN_API_MINOR;

	vcmpFunctions = pluginFuncs;

	pluginCalls->OnServerInitialise = OnServerInitialise;
	pluginCalls->OnPluginCommand = OnPluginCommand;

	return 1;
}
