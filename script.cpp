#include "script.h"
#include "keyboard.h"

using namespace std;

bool isHorseAgitated;

Hash key(const char* text)
{
	return MISC::GET_HASH_KEY(text);
}

// ui function
void showSubtitle(const char* text)
{
	UILOG::_UILOG_SET_CACHED_OBJECTIVE((const char*)MISC::_CREATE_VAR_STRING(10, "LITERAL_STRING", text));
	UILOG::_UILOG_PRINT_CACHED_OBJECTIVE();
	UILOG::_UILOG_CLEAR_CACHED_OBJECTIVE();
}

// prompt functions
void togglePrompt(int prompt, bool visible, bool disable)
{
	HUD::_UIPROMPT_SET_VISIBLE(prompt, visible);
	HUD::_UIPROMPT_SET_ENABLED(prompt, disable);
}

// math functions
float distanceBetween(Vector3 a, Vector3 b)
{
	return BUILTIN::VDIST(a.x, a.y, a.z, b.x, b.y, b.z);
}

float calculateTime(float x, float y)
{
	return x / y;
}

// uses built in natives probably faster
int bFloor(float x)
{
	return BUILTIN::FLOOR(x);
}

int bCeil(float x)
{
	return BUILTIN::CEIL(x);
}

int bRound(float x)
{
	return BUILTIN::ROUND(x);
}

// coreIndex HP: 0, ST: 1, DE: 2
enum class Core {
	Health,
	Stamina,
	DeadEye
};

float calculateCoreDrain(Core core, float time) // core index, time
{
	switch (core)
	{
	case Core::Health:
		return 8.3f * time;
	case Core::Stamina:
		return 16.7f * time;
	case Core::DeadEye:
		return 10.0f * time;
	default:
		return 0;
	}
}

// time functions
void advanceTimeBy(float x)
{
	int hours = bRound(x);
	float minutesR = (x - hours) * 60.0f;
	int minutes = bRound(minutesR);
	float secondsR = (minutes - minutesR) * 60.0f;
	int seconds = bRound(secondsR);

	CLOCK::ADD_TO_CLOCK_TIME(hours, minutes, seconds);
}

int getGameTimer()
{
	return MISC::GET_GAME_TIMER();
}

void waitMs(int ms) // use this function to wait for some ticks before proceeding
{
	int time = getGameTimer() + ms;
	while (getGameTimer() < time)
	{
		WAIT(0);
	}
}

// positioning functions
Vector3 playerPositionByPedID()
{
	return ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true, NULL);
}

Vector3 mountPositionBySaddleID()
{
	return ENTITY::GET_ENTITY_COORDS(PLAYER::_GET_SADDLE_HORSE_FOR_PLAYER(PLAYER::PLAYER_ID()), true, NULL);
}

bool isOnGround(float x, float y, float height, float *z)
{
	return MISC::GET_GROUND_Z_FOR_3D_COORD(x, y, height, z, false);
}

void setEntityHeight(Entity entity, float x, float y, float height)
{
	ENTITY::SET_ENTITY_COORDS_NO_OFFSET(entity, x, y, height, false, false, true);
}

void setEntityCoords(Entity entity, Vector3 vector)
{
	ENTITY::SET_ENTITY_COORDS(entity, vector.x, vector.y, vector.z, false, false, true, false);
}

// entity related functions
int getCore(Ped ped, Core core)
{
	return ATTRIBUTE::_GET_ATTRIBUTE_CORE_VALUE(ped, static_cast<int>(core));
}

void setCore(Ped ped, Core core, int Value)
{
	ATTRIBUTE::_SET_ATTRIBUTE_CORE_VALUE(ped, static_cast<int>(core), Value);
}

int horseSpeed(Ped ped)  // return maximum speed rank
{
	return ATTRIBUTE::GET_ATTRIBUTE_RANK(ped, 5);
}

bool isStaminaCoreSufficient(Ped ped, float value)
{
	return (getCore(ped, Core::Stamina) - value >= 0) ? true : false;
}

void showCoresMs(bool playerOnly, int ms)
{
	if(!playerOnly) HUD::_SHOW_HORSE_CORES(1);
	HUD::_SHOW_PLAYER_CORES(1);
	waitMs(ms);
	if (!playerOnly) HUD::_SHOW_HORSE_CORES(0);
	HUD::_SHOW_PLAYER_CORES(0);
}

// conditional functions

bool isWaypointActive()
{
	return MAP::IS_WAYPOINT_ACTIVE();
}

bool isCinematicCamActive()
{
	return CAM::IS_CINEMATIC_CAM_RENDERING();
}

bool isPlayerPursued()
{
	return (PLAYER::GET_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID()) > 0) ? true : false;
}

bool isPlayerInCombat()
{
	return PED::IS_PED_IN_COMBAT(PLAYER::PLAYER_PED_ID(), NULL) || PED::IS_PED_IN_MELEE_COMBAT(PLAYER::PLAYER_PED_ID());
}

bool isPlayerInMission()
{
	return MISC::GET_MISSION_FLAG() || MISC::IS_MINIGAME_IN_PROGRESS();
}

bool isPlayerOnMount()
{
	return PED::IS_PED_ON_MOUNT(PLAYER::PLAYER_PED_ID()) || PED::IS_PED_SITTING_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID());
}

bool canInstantTravel()
{
	return isWaypointActive() && isCinematicCamActive() && isPlayerOnMount() && !isPlayerInCombat() && !isPlayerPursued() && !isPlayerInMission();
}

bool canInstantTravel2()
{
	return isWaypointActive() && !isPlayerInCombat() && !isPlayerPursued() && !isPlayerInMission();
}

bool isLeadingHorse()
{
	isHorseAgitated = false;
	return PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_INTERACT_LEAD_ANIMAL"));
}

bool isStopLeadingHorse()
{
	return PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_STOP_LEADING_ANIMAL"))
		|| PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_INTERACT_HORSE_FEED"))
		|| PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_HORSE_COMMAND_FLEE"))
		|| PAD::IS_CONTROL_PRESSED(0, MISC::GET_HASH_KEY("INPUT_INTERACT_HORSE_BRUSH"))
		|| isHorseAgitated;
}

bool isPlayerCoreOverpowered(Core coreIndex)
{
	return ATTRIBUTE::_0x200373A8DF081F22(PLAYER::PLAYER_PED_ID(), static_cast<int>(coreIndex));
}

bool isHorseCoreOverpowered(Core coreIndex)
{
	Ped playerID = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	Ped horsePed = (PED::IS_PED_ON_MOUNT(playerPed)) ? PED::GET_MOUNT(playerPed): PLAYER::_GET_SADDLE_HORSE_FOR_PLAYER(playerID);
	return ATTRIBUTE::_0x200373A8DF081F22(horsePed, static_cast<int>(coreIndex));
}

bool isThisModelAWagon(Hash hash)
{
	return !PED::_IS_THIS_MODEL_A_HORSE(hash) && !VEHICLE::IS_THIS_MODEL_A_BOAT(hash) && !VEHICLE::IS_THIS_MODEL_A_TRAIN(hash);
}

// transition functions
void cinematicEffect(bool effect)
{
	CAM::SET_CINEMATIC_MODE_ACTIVE(effect);
}

// effect; 1 = in, 0 = out
void fadeEffect(int ms, bool effect)
{
	effect ? CAM::DO_SCREEN_FADE_IN(ms) : CAM::DO_SCREEN_FADE_OUT(ms);
}

// 0 = departure, 1 = arrival, 2 = tired
void playOnEvent(int event)
{
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	const char* playerModel;

	struct ScriptedSpeechParams
	{
		const char* speechName;
		const char* speechParam;
		alignas(8) int v3;
		alignas(8) Hash v4;
		alignas(8) Entity entity;
		alignas(8) BOOL v6;
		alignas(8) int v7;
		alignas(8) int v8;
	};

	ScriptedSpeechParams params{};
	
	if (PED::IS_PED_MODEL(playerPed, 0xD7114C9)) playerModel = "ARTHUR";
	else if (PED::IS_PED_MODEL(playerPed, 0xB69710)) playerModel = "JOHN_PLAYER";
	else playerModel = "0";

	if(event == 0) params = { "MOUNT_HORSE", playerModel, -1, 0x67F3AB43, -1, true, -1, -1 };
	else if (event == 1) params = { "HITCH_HORSE_WORKED_HARD", playerModel, -1, 0x67F3AB43, -1, true, -1, -1 };
	else if (event == 2) params = { "SOOTH_AGITATED_HORSE", playerModel, -1, 0x67F3AB43, -1, true, -1, -1 };

	AUDIO::_PLAY_AMBIENT_SPEECH1(playerPed, (Any*)&params);
}

// core idea
void instantTravel(bool cam)
{
	Player playerID = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	Entity currentTransport{};

	bool isStaminaNeeded{ false };
	bool isTravelingWithWagon{ false };

	if (cam) // if instantTravel initiated via cinematic cam
	{
		if (PED::IS_PED_ON_MOUNT(playerPed)) // check if player is on horse mount 
		{
			if (ENTITY::DOES_ENTITY_EXIST(PED::GET_MOUNT(playerPed)))
			{
				currentTransport = PED::GET_MOUNT(playerPed); // so any mount player is currently using can be pass
			}
			else if(!ENTITY::DOES_ENTITY_EXIST(PED::GET_MOUNT(playerPed)))
			{
				return;
			}
		}
		else if(!PED::IS_PED_ON_MOUNT(playerPed))  // if player is not on mount
		{
			if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, false) || PED::IS_PED_SITTING_IN_ANY_VEHICLE(playerPed)) // but on any vehicle instead
			{
				if (ENTITY::DOES_ENTITY_EXIST(PED::GET_VEHICLE_PED_IS_USING(playerPed)))
				{
					if (isThisModelAWagon(ENTITY::GET_ENTITY_MODEL(PED::GET_VEHICLE_PED_IS_USING(playerPed))) && !isTravelingWithWagon) // and is exclusively using wagons
					{
						currentTransport = PED::GET_VEHICLE_PED_IS_USING(playerPed);
						ENTITY::SET_ENTITY_AS_MISSION_ENTITY(currentTransport, true, true);
						isTravelingWithWagon = true;
					}
				}
			}
		}
	}
	else if (!cam) // if instantTravel initiated via leading horse instead of cinematic cam
	{
		if (ENTITY::DOES_ENTITY_EXIST(PLAYER::_GET_SADDLE_HORSE_FOR_PLAYER(playerID)))
		{
			currentTransport = PLAYER::_GET_SADDLE_HORSE_FOR_PLAYER(playerID);
		}
		else if (!ENTITY::DOES_ENTITY_EXIST(PLAYER::_GET_SADDLE_HORSE_FOR_PLAYER(playerID)))
		{
			if (ENTITY::DOES_ENTITY_EXIST(PLAYER::_GET_MOUNT_OF_PLAYER(playerID)))
			{
				currentTransport = PLAYER::_GET_MOUNT_OF_PLAYER(playerID);
			}
			else if (!ENTITY::DOES_ENTITY_EXIST(PLAYER::_GET_MOUNT_OF_PLAYER(playerID)))
			{
				return;
			}
		}
	}

	isStaminaNeeded = (isTravelingWithWagon) ? false : true; // horse stamina doesn't need calculated if using wagon

	Vector3 waypoint = MAP::_GET_WAYPOINT_COORDS(); // coordinates of waypoint set on map
	Vector3 playerCoords = playerPositionByPedID(); // coordinates of current player position

	float toWaypointDistance = distanceBetween(playerCoords, waypoint); // calculate distance between these two coordinates
	float timeTaken = (isStaminaNeeded && !isTravelingWithWagon) ? calculateTime(toWaypointDistance / 1000.0f, horseSpeed(currentTransport) * 0.2f) : calculateTime(toWaypointDistance / 1000.0f, 4.0f); // in hours supposedly, hardcoded 4.0f for when using wagons
	float waitTimeMs = 3000.0f ; // bare minimum 3 seconds

	if (isStaminaNeeded && !isTravelingWithWagon) // travelling with horse, needed to calculate stamina
	{
		if (!isHorseCoreOverpowered(Core::Stamina)) // if stamina overpowered, skip stamina drain calculation
		{
			float horseStDrain = calculateCoreDrain(Core::Stamina, timeTaken);

			if (!isStaminaCoreSufficient(currentTransport, horseStDrain)) // if stamina is insufficient to make the trip
			{
				cinematicEffect(false); // deactivate cinematic effect
				if (!cam) TASK::CLEAR_PED_TASKS(playerPed, 0, 0); // if not using cinematic cam, instantTravel from leading horse, clear leading task
				PED::_0xBAE08F00021BFFB2(currentTransport, 0); // agitates the horse
				playOnEvent(2); // play voice line on player ped, on tired
				isHorseAgitated = true; // global variable to track if horse agitated
				return;
			}
			else if (isStaminaCoreSufficient(currentTransport, horseStDrain))
				playOnEvent(0); // play voice line on player ped, on departure
		}
	}
	else if (!isStaminaNeeded && isTravelingWithWagon) // skip stamina calculation when instantTravel with wagons
		playOnEvent(0); // play voice line on player ped, on departure

	waitTimeMs = (toWaypointDistance > waitTimeMs) ? toWaypointDistance : waitTimeMs; // the further the distance the longer wait time

	// transition stuffs
	fadeEffect(1500, false);
	waitMs(bRound(waitTimeMs));

	if (!isOnGround(waypoint.x, waypoint.y, 100.0f, &waypoint.z)) // if waypoint is not on ground, calculate below to identify optimal ground
	{
		const float heights[] = { 
			0.0f, 20.0f, 40.0f, 60.0f, 80.0f, 100.0f, 
			120.0f, 140.0f, 160.0f, 180.0f, 200.0f, 
			220.0f, 240.0f, 260.0f, 280.0f, 300.0f, 
			320.0f, 340.0f, 360.0f, 380.0f, 400.0f,
			420.0f, 440.0f, 460.0f, 480.0f, 500.0f												
		};

		for (const float height: heights)
		{
			if(!cam) setEntityHeight(playerPed, waypoint.x, waypoint.y, height); // if not using cinematic cam, instantTravel from leading horse, need to set playerPed height too
			setEntityHeight(currentTransport, waypoint.x, waypoint.y, height); // set currentTransport whether a horse or wagon
			waitMs(100);
			if (isOnGround(waypoint.x, waypoint.y, height, &waypoint.z)) // break whenever current height is on ground
			{
				waypoint.z = (!isTravelingWithWagon) ? waypoint.z + 0.4f : waypoint.z + 0.8f;
				break;
			}
		}
	}

	setEntityCoords(currentTransport, waypoint); // set entity coordinates based on newly set z coords

	if (!cam) // if not using cinematic cam, instantTravel from leading horse, adjust a bit x & y coords
	{
		waypoint.x = waypoint.x + 0.5f;
		waypoint.y = waypoint.y + 0.5f;
		setEntityCoords(playerPed, waypoint);
		TASK::TASK_LEAD_HORSE(playerPed, currentTransport); // keep leading horse
	}

	while (PED::IS_PED_FALLING(playerPed) || PED::IS_PED_FALLING(currentTransport)) // failsafe if ped is falling, set to be invincible
	{
		ENTITY::SET_ENTITY_INVINCIBLE(playerPed, true);
		if (!isTravelingWithWagon)
			ENTITY::SET_ENTITY_INVINCIBLE(currentTransport, true);
		else if (isTravelingWithWagon)
			VEHICLE::SET_VEHICLE_CAN_BREAK(currentTransport, false);
		waitMs(1);
	}

	// transition stuffs
	advanceTimeBy(timeTaken);
	MISC::CLEAR_WEATHER_TYPE_PERSIST(); // clear current weather
	MISC::SET_RANDOM_WEATHER_TYPE(false, true); // set random weather to mitigate issues

	if (isStaminaNeeded && !isTravelingWithWagon)
	{
		// if core is overpowered, set to no drain
		int horseHpDrain = (isHorseCoreOverpowered(Core::Health)) ? 0 : bFloor(calculateCoreDrain(Core::Health, timeTaken));
		int horseStDrain = (isHorseCoreOverpowered(Core::Stamina)) ? 0 : bFloor(calculateCoreDrain(Core::Stamina, timeTaken));

		// horse core drain 
		(getCore(currentTransport, Core::Health) - horseHpDrain <= 0) ? setCore(currentTransport, Core::Health, 0) : setCore(currentTransport, Core::Health, getCore(currentTransport, Core::Health) - horseHpDrain);
		(getCore(currentTransport, Core::Stamina) - horseStDrain <= 0) ? setCore(currentTransport, Core::Stamina, 0) : setCore(currentTransport, Core::Stamina, getCore(currentTransport, Core::Stamina) - horseStDrain);
	}
	
	// if core is overpowered, set to no drain
	int playerHpDrain = (isPlayerCoreOverpowered(Core::Health)) ? 0 : bCeil(calculateCoreDrain(Core::Health, timeTaken) * 0.4f);
	int playerStDrain = (isPlayerCoreOverpowered(Core::Stamina)) ? 0 : bCeil(calculateCoreDrain(Core::Stamina, timeTaken) * 0.4f);
	int playerDeDrain = (isPlayerCoreOverpowered(Core::DeadEye)) ? 0 : bCeil(calculateCoreDrain(Core::DeadEye, timeTaken) * 0.4f);

	// player core drain
	(getCore(playerPed, Core::Health) - playerHpDrain <= 0) ? setCore(playerPed, Core::Health, 0) : setCore(playerPed, Core::Health, getCore(playerPed, Core::Health) - playerHpDrain);
	(getCore(playerPed, Core::Stamina) - playerStDrain <= 0) ? setCore(playerPed, Core::Stamina, 0) : setCore(playerPed, Core::Stamina, getCore(playerPed, Core::Stamina) - playerStDrain);
	(getCore(playerPed, Core::DeadEye) - playerDeDrain <= 0) ? setCore(playerPed, Core::DeadEye, 0) : setCore(playerPed, Core::DeadEye, getCore(playerPed, Core::DeadEye) - playerDeDrain);

	// transition stuffs
	waitMs(bRound(waitTimeMs));
	fadeEffect(bRound(waitTimeMs), true);
	cinematicEffect(false);

	while (!CAM::IS_SCREEN_FADED_OUT())
	{
		if (CAM::IS_SCREEN_FADED_IN())
		{
			(isStaminaNeeded && !isTravelingWithWagon) ? showCoresMs(false, 2000) : showCoresMs(true, 2000);

			ENTITY::SET_ENTITY_INVINCIBLE(playerPed, false);
			if (!isTravelingWithWagon)
				ENTITY::SET_ENTITY_INVINCIBLE(currentTransport, false);
			else if (isTravelingWithWagon)
			{
				ENTITY::SET_ENTITY_AS_MISSION_ENTITY(currentTransport, false, false);
				VEHICLE::SET_VEHICLE_CAN_BREAK(currentTransport, true);
			}
			break;
		}
		else if (!CAM::IS_SCREEN_FADED_IN())
		{
			waitMs(1);
		}
	}	
}

void main()
{
	// prompt creation here
	int cinematicPrompt = HUD::_UIPROMPT_REGISTER_BEGIN(); // prompt for cinematic camera
	HUD::_UIPROMPT_SET_CONTROL_ACTION(cinematicPrompt, key("INPUT_JUMP"));
	HUD::_UIPROMPT_SET_TEXT(cinematicPrompt, MISC::_CREATE_VAR_STRING(10, "LITERAL_STRING", "Instant Travel"));
	HUD::_UIPROMPT_SET_STANDARD_MODE(cinematicPrompt, 1);
	HUD::_UIPROMPT_REGISTER_END(cinematicPrompt);
	togglePrompt(cinematicPrompt, false, false);

	int horsePrompt = HUD::_UIPROMPT_REGISTER_BEGIN(); // prompt for when leading horse
	HUD::_UIPROMPT_SET_CONTROL_ACTION(horsePrompt, key("INPUT_SPRINT"));
	HUD::_UIPROMPT_SET_TEXT(horsePrompt, MISC::_CREATE_VAR_STRING(10, "LITERAL_STRING", "Instant Travel"));
	HUD::_UIPROMPT_SET_HOLD_MODE(horsePrompt, 1);
	HUD::_UIPROMPT_REGISTER_END(horsePrompt);
	togglePrompt(horsePrompt, false, false);

	bool canShowTravelPrompt{ false };

	while (true)
	{
		// variables that need constantly updated here
		Player playerID = PLAYER::PLAYER_ID();
		Ped playerPed = PLAYER::PLAYER_PED_ID();
		Ped saddleHorse = PLAYER::_GET_SADDLE_HORSE_FOR_PLAYER(playerID);
		Ped mountHorse = PLAYER::_GET_MOUNT_OF_PLAYER(playerID);

		canInstantTravel() ? togglePrompt(cinematicPrompt, true, true) : togglePrompt(cinematicPrompt, false, false);

		if (HUD::_UIPROMPT_HAS_STANDARD_MODE_COMPLETED(cinematicPrompt, 0))
		{
			HUD::_UIPROMPT_SET_VISIBLE(cinematicPrompt, false);
			instantTravel(true);
		}

		Entity horse;

		if (PLAYER::GET_PLAYER_TARGET_ENTITY(playerID, &horse))
		{
			if (horse == saddleHorse || horse == mountHorse)
			{
				int horsePromptGroup = HUD::_UIPROMPT_GET_GROUP_ID_FOR_TARGET_ENTITY(horse);
				HUD::_UIPROMPT_SET_GROUP(horsePrompt, horsePromptGroup, 0);

				if (isLeadingHorse()) canShowTravelPrompt = true;
			}
			else
				canShowTravelPrompt = false;
		}

		if (isStopLeadingHorse()) canShowTravelPrompt = false;

		if (canShowTravelPrompt)
		{
			togglePrompt(horsePrompt, true, true);
			canInstantTravel2() ? togglePrompt(horsePrompt, true, true) : togglePrompt(horsePrompt, true, false);
		}
		else
			togglePrompt(horsePrompt, false, false);

		if (HUD::_UIPROMPT_HAS_HOLD_MODE_COMPLETED(horsePrompt)) instantTravel(0);
		
		WAIT(0);
	}
}

void ScriptMain()
{
	srand(static_cast<int>(GetTickCount64()));
	main();
}