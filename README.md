# LBlaster
LBlaster 는 언리얼 엔진으로 개발된 Multiplayer TPS Shooting Game 입니다.  

## 소개

이 프로젝트는 지금까지 습득해왔던 다양한 기술과 개념을 실제로 적용해보고 게임 개발의 경험을 늘리기 위해 만든 샘플 프로젝트입니다.  
주로 C++를 사용해 개발했으며, Animation 제어와 Widget의 간단한 제어에는 Blueprint Node를 사용했습니다.
리소스에 대한 부담을 줄이기 위해 무료 애셋, Epic Games에서 제공하는 애셋, 온라인 강의에서 제공하는 애셋을 사용해 개발했습니다.

## 기술

- **게임 엔진**
    - **Unreal Engine v5.2.1**
- **프로그래밍 언어**
  - **C++**
  - **Blueprint Node**
- **통합 개발 환경 (IDE)**
  - **Rider**

## 목차

- [주요 특징](#주요-특징)
  - [Multiplayer Session System](#multiplayer-session-system)
  - [Enhanced Input](#enhanced-input)
  - [Combat](#combat)
  - [Client-Side Prediction](#client-side-prediction)
  - [Lag Compensation - Server-Side Rewind](#lag-compensation---server-side-rewind)
  - [Animation](#animation)
  - [UI](#ui)
- [게임플레이](#게임-플레이)
- [회고](#회고)

## 주요 특징

## Multiplayer Session System

[온라인 서브시스템](https://dev.epicgames.com/documentation/ko-kr/unreal-engine/online-subsystem-in-unreal-engine?application_version=5.2)  
[온라인 서브시스템 Steam](https://dev.epicgames.com/documentation/ko-kr/unreal-engine/online-subsystem-steam-interface-in-unreal-engine?application_version=5.2)  
[세션 인터페이스](https://dev.epicgames.com/documentation/ko-kr/unreal-engine/online-subsystem-session-interface-in-unreal-engine?application_version=5.2)

언리얼 엔진과 스팀에서 제공하는 Online Subsystem Steam API 의 Session Interface를 이용해 동일한 Presence에 존재하는 Steam 유저 간의 세션 생성, 참가 등의 관리를 가능하게 해 Multiplayer 기능을 구현한다.  
또한 이 시스템을 모듈화하여 Plugin으로 생성해 개별적으로 동작하고 추가할 수 있게 했다. 

위의 Multiplayer 플러그인을 사용해 호스트가 세션을 생성하고, 다른 플레이어는 세션을 검색해 원하는 세션에 참가한다.  
이 프로젝트는 네트워크 모드로 Listen Server를 사용하므로 세션을 생성한 호스트가 플레이어임과 동시에 Server 역할을 수행하고, 세션에 참가한 플레이어는 클라이언트로 동작한다.

Multiplayer 플러그인의 대략적인 구현은 아래와 같다.

### Delegates, Callbacks for Online Session Interface

Online Subsystem의 Session Interface에 요청한 작업이 완료되면 내부에서 델레게이트를 호출해 작업 결과와 시점을 얻을 수 있다.  

```c++
	/*
	* Internal callbacks for the delegates we'll add to the Online Session Interface delegate list.
	* This don't need to be called outside this class.
	*/
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

    ~~~

    /*
	 *	To add to the Online Session Interface delegate list.
	 *	We'll bind our MultiplayerSessionsSubsystem internal callbacks to these.
	 */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
```
```c++
UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem() :
	OnCreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnCreateSessionComplete)),
	OnFindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnFindSessionComplete)),
	OnJoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnJoinSessionComplete)),
	OnDestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::OnDestroySessionComplete))
{
}
```
Session Interface에서 정의된 델레게이트의 변수를 멤버 변수로 저장하고 해당 델레게이트에 콜백 함수를 등록한다.  
이후 Session Interface에 작업을 요청할 때 해당 델레게이트를 전달하면 이후 작업이 완료될 때 전달한 델레게이트의 콜백 함수가 호출되어 작업이 끝나는 시점에서 원하는 작업을 수행할 수 있다.  
전달한 델레게이트를 Session Interface의 외부인 Subsystem에서 관리하기 위해 `FDelegateHandle`을 이용한다.

### Delegates, Callbacks for Player
```c++
DECLARE_MULTICAST_DELEGATE_OneParam(FLBOnCreateSessionCompleteDelegate, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FLBOnFindSessionsCompleteDelegate, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FLBOnJoinSessionCompleteDelegate, EOnJoinSessionCompleteResult::Type Result);
DECLARE_MULTICAST_DELEGATE_OneParam(FLBOnDestroySessionCompleteDelegate, bool bWasSuccessful);

FLBOnCreateSessionCompleteDelegate LBOnCreateSessionCompleteDelegate;
FLBOnFindSessionsCompleteDelegate LBOnFindSessionsCompleteDelegate;
FLBOnJoinSessionCompleteDelegate LBOnJoinSessionCompleteDelegate;
FLBOnDestroySessionCompleteDelegate LBOnDestroySessionCompleteDelegate;
```
실제 플레이어의 요청에 응답하기 위해 델레게이트를 정의하고, 플레이어가 이 플러그인을 사용할 때 이 델레게이트에 콜백 함수를 등록해 플러그인 외부에서도 작업이 종료된 시점과 동일한 시점을 얻을 수 있다.  
Session Interface에 요청한 작업이 완료되고 등록한 델레게이트의 콜백 함수가 호출되면, 위의 플레이어 용 델레게이트를 브로드캐스팅해 플레이어에도 작업 종료를 알린다.

### CreateSession

```c++
void UMultiplayerSessionsSubsystem::CreateSession(EMatchMode InMatchModeType, int32 InNumMaxPlayer, int32 InGoalKillScore)
{
	~~~

	// 이미 세션이 존재하면 세션 제거. 제거가 완료되면 다시 CreateSession() 호출됨
	if (SessionInterface->GetNamedSession(NAME_GameSession))
	{
		bCreateSessionOnDestroy = true;
		DestroySession();
		return;
	}

	// Store the delegate in a FDelegateHandle so we can later remove it from the delegate list
	OnCreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

	~~~ 생성할 세션 옵션 설정
	
	if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
	{
		if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
		{
			// 세션 생성에 실패하면 델리게이트 핸들 초기화
			SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			LBOnCreateSessionCompleteDelegate.Broadcast(false);
		}	
	}
}
```
Session Interface의 CreateSession을 호출해 세션 생성 작업을 요청한다.  
이때 델레게이트 핸들을 이용해 Session Interface에 델레게이트를 전달하거나 초기화한다.

```c++
void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	LBOnCreateSessionCompleteDelegate.Broadcast(bWasSuccessful);
	
	if (IsValidSessionInterface())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
	}

	if (bWasSuccessful)
	{
		if (UWorld* World = GetWorld())
		{
			World->ServerTravel(FMultiplayerMapPath::LobbyPath);
		}
	}
}
```
세션 생성이 완료되면 ServerTravel로 호스트를 로비로 이동시킨다.

### FindSessions

```c++
void UMultiplayerSessionsSubsystem::FindSessions(int32 InMaxSearchResults)
{
	if (!IsValidSessionInterface())
    {
		LBOnFindSessionsCompleteDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
        return;
    }

    OnFindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

    ~~~ 세션 쿼리 옵션 설정

    if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
    {
    	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
    	{
    		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
    		LBOnFindSessionsCompleteDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
    	}	
    }
}
```
설정한 쿼리 옵션 대로 세션을 찾을 것을 Session Interface에 요청한다.

```c++
void UMultiplayerSessionsSubsystem::OnFindSessionComplete(bool bWasSuccessful)
{
	LBOnFindSessionsCompleteDelegate.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);
	
	if (IsValidSessionInterface())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
	}
}
```
Find 작업이 완료되면 검색 결과와 함께 브로드캐스팅해 플레이어에도 알린다.

### JoinSession

```c++
void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if (!IsValidSessionInterface())
	{
		LBOnJoinSessionCompleteDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}
	
	OnJoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
		LBOnJoinSessionCompleteDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}
```
Session Interface의 JoinSession 작업을 요청한다.

```c++
void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	LBOnJoinSessionCompleteDelegate.Broadcast(Result);

	if (!IsValidSessionInterface())
	{
		return;
	}

	SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
	
	FString Address;
	if (SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		if (APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
		{
			PlayerController->ClientTravel(Address, TRAVEL_Absolute);
		}
	}
}
```
실제로 JoinSession이 성공하면 로컬 플레이어를 호스트가 서버로 존재하는 레벨로 클라이언트로 이동한다.

### DestroySession

```c++
void UMultiplayerSessionsSubsystem::DestroySession()
{
	if (!IsValidSessionInterface())
	{
		return;
	}

	OnDestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);

	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
		LBOnDestroySessionCompleteDelegate.Broadcast(false);
	}
}
```
Session Interface의 DestroySession 작업을 요청한다.

```c++
void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	LBOnDestroySessionCompleteDelegate.Broadcast(bWasSuccessful);
	
	if (IsValidSessionInterface())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
	}

	// 세션을 성공적으로 삭제했고, 세션 생성 중에 지운 경우라면 다시 세션 생성
	if (bWasSuccessful && bCreateSessionOnDestroy)
	{
		bCreateSessionOnDestroy = false;
		CreateSession(MatchModeType, NumPublicConnections, GoalKillScore);
	}
}
```
DestroySession 작업이 종료됨을 플레이어에 알린다.  
추가적으로 호스트가 세션을 생성할 때 이미 생성한 세션이 존재하면 기존 세션을 파괴하고 새로 생성하도록 구현한다.  

만약 다른 세션에 참가중인 플레이어에서 DestroySession이 성공하면 해당 플레이어만 그 세션에서 등록 해제된다.


### Plugin

[새 플러그인 생성하기](https://dev.epicgames.com/documentation/ko-kr/unreal-engine/plugins-in-unreal-engine?application_version=5.2#%EC%83%88%ED%94%8C%EB%9F%AC%EA%B7%B8%EC%9D%B8%EC%83%9D%EC%84%B1%ED%95%98%EA%B8%B0)  
[프로그래밍 서브시스템](https://dev.epicgames.com/documentation/ko-kr/unreal-engine/programming-subsystems-in-unreal-engine?application_version=5.2)

플러그인화 된 Multiplayer 시스템은 실제 기능을 `UGameInstanceSubsystem`의 하위 클래스로 구현했기 때문에 해당 플러그인의 외부에서도 싱글톤 클래스와 유사하게 플러그인의 작동 지침을 알 필요 없이 쉽게 접근하고 사용할 수 있다.

<img src="https://github.com/hayoonleeMe/UE_5.2.1_LBlaster/assets/73100820/3fdf9d50-5b7e-41c6-b49c-5dbb96763ff8">

## Enhanced Input

[향상된 입력](https://dev.epicgames.com/documentation/ko-kr/unreal-engine/enhanced-input-in-unreal-engine?application_version=5.2)

특정 액션을 수행하는 입력을 나타내는 Input Action을 생성한다.  
Input Action은 Input Modifier를 추가해 원시 입력값을 변경하고, Input Trigger를 추가해 입력이 트리거되는 상황을 조절할 수 있다.  

Input Action과 실제 입력 키를 연동할 수 있는 Input Mapping Context를 생성한다.  
Input Mapping Context를 여러 상황에 맞춰 별도로 생성하고 활성화하여 특정 상황에 따라 다른 Key Binding을 적용할 수 있다.

### Input Action

<img src="https://github.com/hayoonleeMe/UE_5.2.1_LBlaster/assets/73100820/1e593e6d-c434-44c4-b3e4-9c8eaaf488cd">

Equip Action을 나타내는 Input Action으로, Pressed 이벤트로 트리거되고 입력 값은 `true`이다.

### Bind Input Action to Input Mapping Context

<img src="https://github.com/hayoonleeMe/UE_5.2.1_LBlaster/assets/73100820/3fe67541-ce70-4b87-868a-79f47925125e" width="50%" height="50%">

일반적인 게임플레이에서 사용할 Input Mapping Context이다.  
Input Action을 트리거할 키 입력을 연동할 수 있다.

### Callbacks for Input Action Trigger

```c++
void ALBlasterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	
	/* IMC_LBContext */
	EnhancedInputComponent->BindAction(FocusChatAction, ETriggerEvent::Triggered, this, &ThisClass::FocusChat);
	EnhancedInputComponent->BindAction(ChatScrollAction, ETriggerEvent::Triggered, this, &ThisClass::ChatScroll);
	EnhancedInputComponent->BindAction(ChangeChatModeAction, ETriggerEvent::Triggered, this, &ThisClass::ChangeChatMode);
	EnhancedInputComponent->BindAction(ScoreboardAction, ETriggerEvent::Triggered, this, &ThisClass::ShowScoreboard);

	/* IMC_PauseMenuContext */
	EnhancedInputComponent->BindAction(PauseMenuAction, ETriggerEvent::Triggered, this, &ThisClass::ShowPauseMenu);

	/* IMC_MenuContext */
	EnhancedInputComponent->BindAction(ReturnMenuAction, ETriggerEvent::Triggered, this, &ThisClass::ReturnMenu);
}
```

`SetupInputComponent`에서 Input Action에 연동된 키 입력에 의해 Input Action이 트리거되면 어떤 동작을 수행할 지 정의한다.  
이때 현재 활성화된 Input Mapping Context에 해당 Input Action이 연동되어 있어야 한다.

### Activate Input Mapping Context

```c++
void ALBlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	/* Input */
	if (GEngine && GetWorld() && IsLocallyControlled())
	{
		if (const ULocalPlayer* Player = GEngine->GetFirstGamePlayer(GetWorld()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Player))
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}	
		}
	}
}
```
`BeginPlay`에서 로컬 플레이어가 사용할 Input Mapping Context를 활성화한다.

```c++
Subsystem->RemoveMappingContext(DefaultMappingContext);
Subsystem->AddMappingContext(PauseMenuMappingContext, 0);
```
기존에 활성화된 Input Mapping Context를 제거하고 다른 Input Mapping Context를 활성화해 다른 Key Binding을 사용할 수 있다.

## Combat

대략적인 전투 관련 로직은 Client-Server 모델에 따라 언리얼 엔진의 Replication과 RPC를 사용해 구현했다.  
하지만 이 프로젝트는 실시간으로 전투가 이뤄지는 TPS Shooter 장르이기 때문에 네트워크 상태에 많은 영향을 받고, Steam을 통해 세션을 관리하기 때문에 네트워크 상황이 불안정해질 가능성이 크다.  
따라서 단순히 Replication과 RPC만을 사용하면 불안정한 네트워크에 의해 서버와 클라이언트의 동기화에 문제가 생길 가능성이 높으므로 몇몇 핵심 로직은 기존과는 다르게 구현했다.    

이 섹션에서는 Replication과 RPC만을 이용해 서버와 클라이언트 간의 단순한 통신만으로 구현할 수 있는 전투 로직만 설명한다.   

**이하 추가 예정**

## Client-Side Prediction

**이하 추가 예정**

## Lag Compensation - Server-Side Rewind

총을 발사하여 데미지를 입힐 때 Authoritative한 Server에서만 데미지를 입힐 수 있기 때문에, 네트워크가 불안정하면 클라이언트에서 총알이 캐릭터에 맞는 시점과 그 작업이 서버로 넘어가 서버에서 총알이 캐릭터에 맞는 시점이 차이가 난다.  
네트워크가 불안정 할수록 클라이언트와 서버 간의 통신에 걸리는 시간이 커져 차이가 커지고, 그 차이가 클수록 더 불공평한 상황이 발생할 수 있다.    
이 불공평함을 줄이기 위해 Server-Side Rewind 기법을 사용해 클라이언트에서 총알이 캐릭터에 맞았을 때 데미지 판정을 위해 서버에게 요청을 보낼 때 총알이 맞았을 때의 Time Stamp를 함께 보내고, 서버에서 그 시간으로 상태를 되돌려 실제로 서버에서도 맞았는지 체크하여 데미지를 입히도록 한다.  
모든 과정을 한 프레임에서 수행하기 때문에 실제 플레이어 입장에서는 이질감을 느끼지 못한다.

이처럼 실제 발생한 랙에 대해 보상, 보정하는 기법을 Lag Compensation이라 한다.  

하지만 좋은 상태의 네트워크를 구축한 플레이어 입장에서는 네트워크 상태에 따른 보정을 역차별로 느낄 수 있기 때문에 일정 수준의 지연 시간에만 적용하도록 제한하는 것이 좋을 것으로 보인다.

### Request SSR at Client

```c++
--- In AProjectileBullet ---

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	~~~
	else if (!OwnerCharacter->HasAuthority() && OwnerCharacter->IsLocallyControlled() && OwnerCharacter->IsServerSideRewindEnabled())
		{
			// Apply Damage With Server-Side Rewind
			if (IsValidOwnerController())
			{
				const float HitTime = OwnerController->GetServerTime() - OwnerController->GetSingleTripTime();
				OwnerWeapon->CallServerScoreRequest(HitCharacter, TraceStart, InitialVelocity, HitTime, GetDamage(), HeadshotMultiplier, GetProjectileGravityScale());
			}
		}
}
```
```c++
--- In AProjectileWeapon ---

void AProjectileWeapon::ServerScoreRequest_Implementation(ALBlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime, float InDamage, float InHeadshotMultiplier, float InProjectileGravityScale)
{
	if (IsValidOwnerCharacter() && OwnerCharacter->GetLagCompensationComponent() && HitCharacter && GetWorld())
	{
		const FServerSideRewindResult Confirm = OwnerCharacter->GetLagCompensationComponent()->ProjectileServerSideRewind(HitCharacter, TraceStart, InitialVelocity, HitTime, InProjectileGravityScale);
		if (Confirm.bHitConfirmed && OwnerCharacter->GetController())
		{
			// Play HitReact Montage
			HitCharacter->SetLastHitNormal(Confirm.ImpactNormal);

			float DamageToCause = InDamage * GetDamageFallOffMultiplier(Confirm.HitDistanceMeter);
			if (Confirm.bHeadShot)
			{
				DamageToCause *= InHeadshotMultiplier;
			}
			// Apply Damage
			UGameplayStatics::ApplyDamage(HitCharacter, DamageToCause, OwnerCharacter->GetController(), this, UDamageType::StaticClass());
		}
	}
}
```
SSR을 사용한다면, 클라이언트의 로컬 플레이어가 발사한 총알에서 Hit Event가 발생했을 때 그 때의 시간과 이 시점으로 되돌려 판정하기 위해 필요한 정보를 서버로 전달한다.  
클라이언트에서 총알에 맞았다고 주장하는 Hit Character의 SSR을 담당하는 LagCompensationComponent에 접근해 SSR을 수행해 실제로 맞았는지 판정하도록 한다.  
실제로 서버에서 그 시점에서도 맞았다면 데미지를 적용시킨다.

### Hit Boxes for SSR

<img src="https://github.com/hayoonleeMe/UE_5.2.1_LBlaster/assets/73100820/e9c07a54-0205-4a3b-812d-4d273093c728">

클라이언트가 총알이 맞았다고 주장하는 특정 캐릭터를 서버에서 클라이언트에서의 시점으로 되돌리기 위해 충돌을 감지할 수 있는 `UBoxComponent`를 캐릭터의 Bone과 유사하게 배치하고 해당 Box들의 위치를 되돌려 캐릭터를 대신하도록 한다.  
다소 러프하지만 실제 캐릭터 자체를 되돌리는 것보다 안전하고 구현이 간편한 방법이다.

### Server-Side Rewind to Judge

```c++
USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	FVector BoxExtent;
};
```
Hit Box의 정보를 하나의 구조체로 관리한다.

```c++
USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time;

	UPROPERTY()
	TMap<FName, FBoxInformation> HitBoxInfo;
};
```
특정 시간의 프레임에서의 모든 Hit Box들의 정보를 모아 하나의 구조체로 관리한다.  

```c++
void ULagCompensationComponent::SaveFramePackage()
{
	// 일정 시간만큼의 프레임 정보만 유지
	if (FrameHistory.Num() > 1)
	{
		float HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		while (HistoryLength > MaxRecordTime)
		{
			FrameHistory.RemoveNode(FrameHistory.GetTail());
			HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		}
	}

	// 프레임 정보 저장
	FFramePackage ThisFrame;
	if (IsValidOwnerCharacter() && GetWorld())
	{
		ThisFrame.Time = GetWorld()->GetTimeSeconds();
		for (const TTuple<FName, UBoxComponent*>& BoxPair : OwnerCharacter->GetHitCollisionBoxes())
		{
			FBoxInformation BoxInformation;
			BoxInformation.Location = BoxPair.Value->GetComponentLocation();
			BoxInformation.Rotation = BoxPair.Value->GetComponentRotation();
			BoxInformation.BoxExtent = BoxPair.Value->GetScaledBoxExtent();
			ThisFrame.HitBoxInfo.Emplace(BoxPair.Key, BoxInformation);
		}
	}
	FrameHistory.AddHead(ThisFrame);
}
```
일정 시간만큼의 프레임 정보를 저장한다.  
일정 시간만큼의 프레임 정보는 TDoubleLinkedList 컨테이너에 저장한다.  
매 프레임 내부에 접근하고 추가, 삭제를 수행하므로 링크드 리스트를 이용해 성능 상의 이점을 얻을 수 있다.

```c++
FServerSideRewindResult ULagCompensationComponent::ProjectileServerSideRewind(ALBlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart,
	const FVector_NetQuantize100& InitialVelocity, float HitTime, float GravityScale)
{
	FFramePackage FrameToCheck = GetFrameToCheck(HitCharacter, HitTime);

	// FrameToCheck Confirm Hit
	return ProjectileConfirmHit(FrameToCheck, HitCharacter, TraceStart, InitialVelocity, HitTime, GravityScale);
}
```
클라이언트로부터 요청을 받으면 클라이언트에서의 시간에서의 프레임을 찾아 실제 Hit을 판정한다.

```c++
FServerSideRewindResult ULagCompensationComponent::ProjectileConfirmHit(const FFramePackage& InPackage, ALBlasterCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime, float GravityScale)
{
	~~~ 판정할 프레임에서의 위치로 캐릭터의 모든 Hit Box들을 이동시킴

	// 머리 먼저
	~~~ Hit Box 충돌 설정

	~~~ PathParams 설정
	FPredictProjectilePathResult PathResult;
	UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);

	// hit head, return early
	if (PathResult.HitResult.bBlockingHit)
	{
		ResetHitBoxes(HitCharacter, CurrentFrame);
		EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
		const float HitDistanceMeter = (PathResult.HitResult.ImpactPoint - TraceStart).Length() / 100.f;
		return FServerSideRewindResult{ true, true, PathResult.HitResult.ImpactNormal, HitDistanceMeter };
	}
	
	// 다른 박스 체크
	~~~ Hit Box 충돌 설정
	
	UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);
	if (PathResult.HitResult.bBlockingHit)
	{
		ResetHitBoxes(HitCharacter, CurrentFrame);
		EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
		const float HitDistanceMeter = (PathResult.HitResult.ImpactPoint - TraceStart).Length() / 100.f;
		return FServerSideRewindResult{ true, false, PathResult.HitResult.ImpactNormal, HitDistanceMeter };
	}

	ResetHitBoxes(HitCharacter, CurrentFrame);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
	return FServerSideRewindResult{ false, false };
}
```
그 프레임에서의 모든 Hit Box들의 정보를 이용해 시뮬레이션 해본다.  
위 함수는 총알에 의한 Hit 판정이므로 UGameplayStatics::PredictProjectilePath()를 사용해 Hit을 판정한다.  
HitScan을 사용하는 무기의 경우 LineTrace를 수행한다.  
`FBoxInformation`에 저장된 위치 정보로 현재 캐릭터의 Hit Box들을 이동시키기 때문에 실제로 총알에 맞는지 판정할 수 있다.  

## Animation

Animation 시스템은 언리얼 엔진에서 샘플 게임플레이 프로젝트로써 제공되는 Lyra Starter Game 프로젝트의 Animation 시스템을 대부분 차용했다.  
실제 Lyra 프로젝트를 분해하고 분석해 이 프로젝트에 이식했다.  
언리얼 엔진 5의 유용한 기능이 많이 적용되어 적은 양의 애니메이션 애셋으로도 제법 훌륭하게 애니메이션이 동작한다.  

[라이라의 애니메이션](https://dev.epicgames.com/documentation/ko-kr/unreal-engine/animation-in-lyra-sample-game-in-unreal-engine?application_version=5.2)  
Linked Layer Animation Blueprint를 이용해 메인 애니메이션 블루프린트의 여러 부분을 오버라이드할 수 있다.  
이를 통해 착용 중인 무기에 따라 다른 애니메이션 애셋을 사용하거나 다른 로코모션 행동을 수행하게 할 수 있다.  

### Anim Layer Interface

<img src="https://github.com/hayoonleeMe/UE_5.2.1_LBlaster/assets/73100820/b3e5a896-5eda-48b3-8767-8d2a4881495b" width="60%">

Anim Layer Interface로 애니메이션 블루프린트에서 오버라이드할 부분을 지정할 수 있다.  

<img src="https://github.com/hayoonleeMe/UE_5.2.1_LBlaster/assets/73100820/f4ec3eaf-6cea-4756-8039-836de11cceb3" width="80%">

Interface로 정의한 Layer를 AnimGraph에서 사용할 수 있다.  
이때 ABP_LBlasterAnimInstance는 메인 애니메이션 블루프린트로, 위의 Anim Layer Interface인 ALI_ItemAnimLayers를 Implement한다.  
캐릭터는 기본적으로 ABP_LBlasterAnimInstance를 사용한다.

<img src="https://github.com/hayoonleeMe/UE_5.2.1_LBlaster/assets/73100820/c093c210-5754-4229-9884-84848a64f7f8" width="80%">

ABP_ItemAnimLayersBase는 동일하게 ALI_ItemAnimLayers를 Implement하는 애니메이션 블루프린트로, 실제 정의된 Animation Layer를 구현한다.

### Link Anim Layer

<img src="https://github.com/hayoonleeMe/UE_5.2.1_LBlaster/assets/73100820/7d899b90-55e9-47a3-8719-1207126d7339">

실제 Animation Layer를 구현하는 ABP_ItemAnimLayersBase를 상속받는 ABP_RifleAnimLayers로, 상위의 ABP_ItemAnimLayersBase의 변수들을 오버라이드하여 설정할 수 있다.  
이를 통해 무기 별로 서로 다른 애니메이션 셋을 설정해 다르게 동작하도록 할 수 있다.  

```c++
void ALBlasterCharacter::SetWeaponAnimLayers(EWeaponType InWeaponType, TSubclassOf<UAnimInstance> InWeaponAnimLayer)
{
	// BaseAnimLayerClass
	if (InWeaponType == EWeaponType::EWT_Unarmed)
	{
		if (BaseAnimLayerClass)
		{
			GetMesh()->LinkAnimClassLayers(BaseAnimLayerClass);
		}
	}
	else if (InWeaponAnimLayer)
	{
		GetMesh()->LinkAnimClassLayers(InWeaponAnimLayer);
	}
}
```
C++ 코드에서 동적으로 현재 적용되는 Anim Layer를 변경할 수 있다.  
위 코드에서, 무기 별로 설정된 Anim Layer를 나타내는 Animation Blueprint를 통해 Anim Layer를 변경한다.

## UI

[UMG UI 디자이너](https://dev.epicgames.com/documentation/ko-kr/unreal-engine/umg-ui-designer-for-unreal-engine?application_version=5.2)

언리얼 엔진의 UMG를 이용해 게임 내 UI를 구현한다.  
위젯 블루프린트로 UI를 설계, 배치하고, AHUD로 화면에 위젯을 생성하고 관리한다.  
UserWidget을 상속받는 새로운 클래스를 생성하고 위젯 블루프린트의 부모 클래스로 사용한다.  
클래스의 프로퍼티와 위젯 블루프린트의 위젯을 연동해 C++로 위젯을 제어한다.  
맵 별로 다른 HUD를 사용해 맵에 정해진 UI를 표시하고 관리한다.

### Update Widget

<img src="https://github.com/hayoonleeMe/UE_5.2.1_LBlaster/assets/73100820/b2a1b7ab-c93a-4d83-beca-ea3929a1fc21">

```c++
--- UCharacterOverlay ---

UPROPERTY(meta = (BindWidget))
TObjectPtr<UTextBlock> AmmoAmount;
```

위젯 블루프린트의 위젯 요소를 클래스의 프로퍼티로 바인딩해 C++로 접근할 수 있게 한다.  



```c++
// 총을 발사해 탄약을 소모한다.
void AWeapon::SpendRound()
{
	AddAmmo(-1);
}

// 변경된 탄약 수를 전달해 업데이트를 요청한다.
void AWeapon::SetHUDAmmo()
{
	if (IsValidOwnerCharacter() && bSelected)
	{
		OwnerCharacter->SetHUDAmmo(Ammo);
	}
}

void ALBlasterCharacter::SetHUDAmmo(int32 InAmmo)
{
	if (ALBlasterPlayerController* PlayerController = Cast<ALBlasterPlayerController>(Controller))
	{
		PlayerController->SetHUDAmmo(InAmmo);
	}
}

void ALBlasterPlayerController::SetHUDAmmo(int32 InAmmo)
{
	if (IsValidOwningHUD())
	{
		OwningHUD->SetHUDAmmo(InAmmo);
	}
}

void ALBlasterHUD::SetHUDAmmo(int32 InAmmo)
{
	if (CharacterOverlay)
	{
		CharacterOverlay->SetAmmoAmount(InAmmo);
	}
}

void UCharacterOverlay::SetAmmoAmount(int32 InAmmo)
{
	AmmoAmount->SetText(FText::AsNumber(InAmmo));
}
```

탄약을 소모하면 HUD에서 AmmoAmount TextBlock이 존재하는 UCharacterOverlay에 접근해 AmmoAmount TextBlock을 업데이트한다.

## 게임 플레이

<img src="https://github.com/hayoonleeMe/UE_5.2.1_LBlaster/assets/73100820/12b864be-4f29-41e8-8cae-8fd8106bd3ae">

<img src="https://github.com/hayoonleeMe/UE_5.2.1_LBlaster/assets/73100820/c3e5ca11-d83f-49e4-88d1-30b2de559ebe">


## 회고

앞으로 프로젝트를 계속 추가할 때 리마인드 시킬 수 있게 프로젝트를 진행하면서 아쉬운 점, 깨달은 점 등을 편하게 적어본다.

이번 프로젝트를 진행할 때 진행이 많이 더뎠던 점이 가장 아쉽다.  
회고를 작성하는 현재 (2024.05.27) 도 아직 부족한 기능, 버그가 많이 존재한다.  
이전에 수강했던 온라인 강의를 노트로 정리해뒀기 때문에 그 노트를 참고해가면서 프로젝트를 진행했는데, 전체적인 구조 설계, 계획 등을 제대로 정립하지 않아서 점차 온라인 강의의 순서를 따라가게 되었고, 그에 따라 이미 개발했던 구조, 시스템 등을 변경하느라 진행이 크게 더뎌지게 된 것 같다.  
또한 크게 생소했던 Lyra의 애니메이션 시스템을 이식하는 데에도 많은 시간이 들어갔다.  
간단한 수준의 설계라도 수행해 전체적인 틀을 잡는 것이 얼마나 중요한지 깨달을 수 있었다.  

위의 아쉬운 점과도 약간 연결이 되는 것이, 전체적인 설계가 미흡해 프로젝트를 진행할 워크 플로우와 그 과정에서 사용될 프레임워크들에 대한 생각도 부족했다.  
Git을 이용해 프로젝트를 관리하기는 했지만 단순히 커밋만 쌓였을 뿐이다.  
그에 따라 많은 작업을 일관된 카테고리로 분류해 관리하지 못하고 작업을 수행할 계획도 부족해 중구난방으로 작업을 수행했다.  
프로젝트 진행 중간에 개인적인 일로 인해 거의 진행하지 못한 기간이 있는데, 그 기간 이후에 다시 개발에 착수하려고 하니 현재까지의 진행 상황, 앞으로의 예정 등을 알기가 힘들어 어려움을 겪었다.  
그에 따라 다음 프로젝트를 수행하기 전에 Git에 대해 다시 공부하고 깃허브와 연동해 Issue, Project 등의 기능을 이용해 프로젝트를 관리하고자 하는 목표가 생겼다.  

이 프로젝트 이후부터는 블로그 작성도 다시 시작하고 정리, 기록 등을 적극적으로 수행해 좀 더 생산적인 활동을 지속해나가야 겠다는 다짐이 생겼다.