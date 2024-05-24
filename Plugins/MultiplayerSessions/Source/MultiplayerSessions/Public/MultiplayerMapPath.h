#pragma once

namespace FMultiplayerMapPath
{
	/*
	 *	Key
	 */
	// FOnlineSessionSettings에 MatchMode 값을 저장하기 위한 Key
	inline static constexpr const TCHAR* MatchModeKey{ TEXT("LBlaster_MatchMode") };

	inline static constexpr const TCHAR* GoalScoreKey{ TEXT("LBlaster_GoalScore") };

	/*
	 *	Path
	 */
	// 세션을 생성하고 ServerTravel로 이동할 로비 맵의 경로
	inline static constexpr const TCHAR* LobbyPath{ TEXT("/Game/LBlaster/Maps/Lobby?listen") };

	inline static constexpr const TCHAR* TeamDeathMatchMapPath{ TEXT("/Game/LBlaster/Maps/TeamDeathMatch?listen") };
	
	inline static constexpr const TCHAR* FreeForAllMapPath{ TEXT("/Game/LBlaster/Maps/FreeForAll?listen") };

	inline static constexpr const TCHAR* TitleMapPath{ TEXT("/Game/LBlaster/Maps/GameStartupMap") };

	/*
	 *	Debug
	 */
	inline static constexpr const TCHAR* TeamDeathMatchTestMapPath{ TEXT("/Game/LBlaster/Maps/TestMapTeamDeathMatch?listen") };
	
	inline static constexpr const TCHAR* FreeForAllTestMapPath{ TEXT("/Game/LBlaster/Maps/TestMapFreeForAll?listen") };
}
