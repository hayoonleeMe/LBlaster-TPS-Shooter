#pragma once

#include "CoreMinimal.h"
#include "Team.h"
#include "ChatParams.generated.h"

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
	ECM_System,
	ECM_Max
};

namespace ChatTextStyle
{
	inline const static FString FriendlyTeamTextStyle{TEXT("FriendlyTeam")};
	inline const static FString OpponentTeamTextStyle{TEXT("OpponentTeam")};
	inline const static FString DefaultTextStyle{TEXT("Default")};
	inline const static FString SystemTextStyle{TEXT("System")};

	inline FString GetChatTextStyle(EChatMode Value, bool bFriendlyTeam = false)
	{
		switch (Value)
		{
		case EChatMode::ECM_All:
			// fall through
		case EChatMode::ECM_FriendlyTeam:
			return bFriendlyTeam ? FriendlyTeamTextStyle : OpponentTeamTextStyle;

		case EChatMode::ECM_FreeForAll:
			// fall through
		case EChatMode::ECM_Lobby:
			return DefaultTextStyle;

		case EChatMode::ECM_System:
			return SystemTextStyle;

		default:
			return FString();
		}
	}

	inline const static FString ToAllPrefix{TEXT("[전체]")};
	inline const static FString ToFriendlyTeamPrefix{TEXT("[팀]")};
	inline const static FString SystemPrefix{TEXT("[시스템]")};

	inline FString GetChatTargetPrefix(EChatMode ChatMode)
	{
		switch (ChatMode)
		{
		case EChatMode::ECM_All:
		case EChatMode::ECM_FreeForAll:
		// fall through
		case EChatMode::ECM_Lobby:
			return ToAllPrefix;

		case EChatMode::ECM_FriendlyTeam:
			return ToFriendlyTeamPrefix;

		case EChatMode::ECM_System:
			return SystemPrefix;

		default:
			return FString();
		}
	}
}

UENUM()
enum class EChatSystemInfoTemplate
{
	ECSIT_Login,
	ECSIT_Logout
};

namespace ChatSystemInfoTemplate
{
	inline const static FString LoginTemplate{TEXT("님이 게임에 입장했습니다.")};
	inline const static FString LogoutTemplate{TEXT("님이 게임에서 나갔습니다.")};

	inline FString GetChatSystemInfoTemplate(EChatSystemInfoTemplate Value)
	{
		switch (Value)
		{
		case EChatSystemInfoTemplate::ECSIT_Login:
			return LoginTemplate;

		case EChatSystemInfoTemplate::ECSIT_Logout:
			return LogoutTemplate;

		default:
			return FString();
		}
	}
}

USTRUCT()
struct FChatParams
{
	GENERATED_BODY()

	FChatParams()
		: ChatMode(EChatMode::ECM_All), SenderPlayerTeam(ETeam::ET_MAX), ChatSystemInfoTemplate(EChatSystemInfoTemplate::ECSIT_Login)
	{
	}

	UPROPERTY()
	EChatMode ChatMode;

	UPROPERTY()
	FString SenderPlayerName;

	UPROPERTY()
	FString Content;

	UPROPERTY()
	ETeam SenderPlayerTeam;

	UPROPERTY()
	EChatSystemInfoTemplate ChatSystemInfoTemplate;
};
