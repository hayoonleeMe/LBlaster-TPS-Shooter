#pragma once

UENUM()
enum class EChatMode : uint8
{
	/* Team DeathMatch */
	ECM_All,
	ECM_FriendlyTeam,
	/* Free For All */
	ECM_FreeForAll,
	/* Lobby */
	ECM_Lobby,
	ECM_Max
};
