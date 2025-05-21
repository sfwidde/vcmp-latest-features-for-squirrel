# Latest Vice City: Multiplayer (VC:MP) 0.4 features for Squirrel servers
![](https://imgur.com/RLaNR8v.png)

With VC:MP's latest update (v0.4.7), there are new functionalities that were
added to the server through the new plugin SDK. Unfortunately, official Squirrel
module has not been updated in a while (6 years!), making these features
inaccessible for Squirrel server developers. So, as a temporary solution to this
problem, I've decided to make this plugin so that they can enjoy from these
features on their servers too!

## Installation steps
1. Head to the [**Releases**](https://github.com/sfwidde/vcmp-latest-features-for-squirrel/releases/latest)
page, choose a plugin based on your needs, then download it.
2. Load the plugin by appending its name to the `plugins` line in your server's
`server.cfg`.

## Documentation
This plugin adds the following Squirrel-specific definitions:

### Global functions
- `void SetCrouchEnabled(bool enable)`
- `bool GetCrouchEnabled(void)`

### Player
#### Read-write properties
- (bool) `Player.Bleeding` / `Player.IsBleeding`
- (int) `Player.DrunkVisuals`
- (int) `Player.DrunkHandling`
#### Methods
- `void Player.Kill(void)`
- `void Player.SetCameraPos(Vector cameraPos, Vector cameraLook[, int interpTimeMS = 0])`
- `void Player.Set3DArrowForPlayer(Player playerToShow, bool enable)`
- `bool Player.Get3DArrowForPlayer(Player playerToShow)`

### Vehicle
#### Read-write properties
- (bool) `Vehicle.EngineEnabled` / `Vehicle.IsEngineEnabled`
- (bool) `Vehicle.BonnetOpen` / `Vehicle.IsBonnetOpen`
- (bool) `Vehicle.BootOpen` / `Vehicle.IsBootOpen`
#### Methods
- `void Vehicle.Set3DArrowForPlayer(Player playerToShow, bool enable)`
- `bool Vehicle.Get3DArrowForPlayer(Player playerToShow)`

### Notes
If you are not familiarized with C/C++ programming languages, `void` type must
be unknown to you. In the context of a function's parameter list, this means
that the function takes no parameters. In the context of a function's return
value, this means that the function returns nothing, or, in other words, `null`.

`Player.SetCameraPos()` already exists by default in the official Squirrel
module, the only difference that you will find is that this plugin adds a new
(but optional) third parameter `interpTimeMS` to it, which, in case it is not
omitted and does not equal to zero, will internally call the new
`InterpolateCameraLookAt()` SDK function.

## Examples
<details>
<summary>Make a player bleed when their health is very low</summary>

```
function onPlayerHealthChange(player, lastHP, newHP)
{
	player.IsBleeding = ((newHP > 0.0) && (newHP < 10.0));
}
```
</details>

<details>
<summary>Smoothly situate player's camera 100m right above them</summary>

```
local cameraLook = player.Pos;
local cameraPos = (cameraLook + Vector(0.0, 0.0, 100.0));
player.SetCameraPos(cameraPos, cameraLook, 2000 /* (new!) */);
```
</details>

<details>
<summary>Enable 3D arrow for vehicle with ID 1, which is visible
to all players</summary>

```
function onPlayerSpawn(player)
{
	local vehicle = FindVehicle(1); // 1st vehicle on server
	// Make sure this vehicle exists
	if (vehicle)
	{
		// (Check below is not actually needed here but we will
		// perform it anyway for the sake of this example)
		//
		// Player does not have 3D arrow enabled for this vehicle
		if (!vehicle.Get3DArrowForPlayer(player))
		{
			// Enable it now!
			vehicle.Set3DArrowForPlayer(player, true);
		}
	}
}
```
</details>