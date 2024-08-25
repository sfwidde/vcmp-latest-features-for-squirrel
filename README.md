# Latest Vice City: Multiplayer (VC:MP) 0.4 features for Squirrel servers
![Image](https://imgur.com/RLaNR8v.png)

With VC:MP's latest update (v0.4.7), there are new functionalities that were
added to the server through the new plugin SDK. Unfortunately, official Squirrel
module has not been updated in a while (6 years!), making these features
inaccessible for Squirrel server developers. So, as a temporary solution to this
problem, I've decided to make this plugin so that they can enjoy from these
features on their servers too!

## Installation steps
1. Head to the
[**Releases** page](https://github.com/sfwidde/vcmp-latest-features-for-squirrel/releases/latest),
choose a plugin based on your needs, then, download it.
2. Load the plugin by appending its name to the `plugins` line in your server's
`server.cfg`.

## Documentation
### This plugin adds the following Squirrel-only functions:
- `void SetCrouchEnabled(bool enable)`
- `bool GetCrouchEnabled(void)`
- `int KillPlayer(int playerId/-1)`
- `int SetPlayerCameraPos(int playerId/-1, float posX, float posY, float posZ,
float lookX, float lookY, float lookZ, int interpTimeMS)`
	- If `interpTimeMS` is 0, it is equivalent to doing
	`FindPlayer(playerId).SetCameraPos(Vector(posX, posY, posZ),
	Vector(lookX, lookY, lookZ))`
- `int SetPlayerDrunkVisuals(int playerId/-1, int drunkLevel)`
- `int GetPlayerDrunkVisuals(int playerId)`
- `int SetPlayerDrunkHandling(int playerId/-1, int drunkLevel)`
- `int GetPlayerDrunkHandling(int playerId)`
- `int SetPlayerBleeding(int playerId/-1, bool enable)`
- `bool IsPlayerBleeding(int playerId)`
- `int SetPlayer3DArrowEnabled(int playerId, int playerToShowId/-1,
bool enable)`
- `bool IsPlayer3DArrowEnabled(int playerId, int playerToShowId)`
- `int SetVehicleBonnetOpen(int vehicleId/-1, bool enable)`
- `bool IsVehicleBonnetOpen(int vehicleId)`
- `int SetVehicleBootOpen(int vehicleId/-1, bool enable)`
- `bool IsVehicleBootOpen(int vehicleId)`
- `int SetVehicle3DArrowEnabled(int vehicleId, int playerToShowId/-1, bool enable)`
- `bool IsVehicle3DArrowEnabled(int vehicleId, int playerToShowId)`

#### Notes
*Some* functions that expect a valid player/vehicle ID can also be passed a
negative integer (typically, -1) to them, which means 'everyone on the server'
for players, and 'all vehicles' for vehicles. These functions return the number
of entities affected by this function call. For instance, `KillPlayer` will
return the number of players that were killed by a call to this function.

If you are not familiarized with C/C++ programming languages, `void` type must
be unknown to you. In the context of a function's parameter list, this means
that the function takes no parameters. In the context of a function's return
value, this means that the function returns nothing; or, in other words,
`null`.

## Examples
- Custom class selector script example making use of the new
`InterpolateCameraLookAt` SDK function (`SetPlayerCameraPos`):
https://github.com/sfwidde/vcmp-custom-class-selector

## Credits
Files in `src/include/squirrel/` were gotten from
[here](https://github.com/vicecitymultiplayer/plugin-squirrel).