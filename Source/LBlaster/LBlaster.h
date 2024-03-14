// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/*
 * Trace Channel
 */
#define ECC_SkeletalMesh ECollisionChannel::ECC_GameTraceChannel1
#define ECC_FootPlacement ECollisionChannel::ECC_GameTraceChannel2

/*
 * Custom Log Category
 */

// 현재 액터의 ENetRole 타입의 Local Network Role 값을 가져온다.
// ENetRole은 UEnum()으로 지정되어 있으므로 UEnum::GetValueAsString를 통해 값을 가져올 수 있다.
#define LOG_LOCALROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetLocalRole()))
// 현재 액터의 ENetRole 타입의 Remote Network Role 값을 가져온다.
// 로그를 찍을 때 Local Role과 Remote Role을 함께 찍어준다.
#define LOG_REMOTEROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetRemoteRole()))

// 액터에 소속된 서브오브젝트들이 사용
#define LOG_SUBLOCALROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetLocalRole()))
#define LOG_SUBREMOTEROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetRemoteRole()))

// PIE 환경이 아니면 -1을 가지고 PIE 환경일 때만 유효한 값을 가지는 전역변수 GPlayInEditorID를 사용해 몇번 클라이언트인지 찍는다.
// 또한 서버의 경우 이 프로젝트에서는 Listen Server만 사용할 것이므로 간단하게 SERVER로만 표시한다.
#define LOG_NETMODEINFO ((GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("CLIENT%d"), GPlayInEditorID) : ((GetNetMode() == ENetMode::NM_Standalone) ? TEXT("Standalone") : TEXT("SERVER")))

// 로그 매크로 호출 위치 __FUNCTION__을 기본 스트링 TCHAR로 변환
#define LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)

#define LB_LOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s][%s/%s] %s %s"), LOG_NETMODEINFO, LOG_LOCALROLEINFO, LOG_REMOTEROLEINFO, LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

// 액터에 소속된 서브오브젝트들이 사용
#define LB_SUBLOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s][%s/%s] %s %s"), LOG_NETMODEINFO, LOG_SUBLOCALROLEINFO, LOG_SUBREMOTEROLEINFO, LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

DECLARE_LOG_CATEGORY_EXTERN(LogNetwork, Log, All);