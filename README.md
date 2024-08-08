# LBlaster
LBlaster 는 언리얼 엔진으로 개발된 Multiplayer TPS Shooting Game 입니다.  

## 목차
- [소개](#소개)
- [기술](#기술)
- [목차](#목차)
- [구현 상세 설명](#구현-상세-설명)
- [게임플레이 및 기능](#게임플레이-및-기능)
  - [Weapons](#weapons)
  - [Miscellaneous](#miscellaneous)
  - [Game Mode](#game-mode)
  - [Chat](#chat)
  - [Settings](#settings)
- [회고](#회고)


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

## 구현 상세 설명
각 기능의 구현은 아래 링크에서 자세히 설명합니다.  
링크 추가

## 게임플레이 및 기능
게임의 주요 게임플레이 및 기능을 간단하게 설명합니다.

### Weapons

#### Pistol
![Pistol](https://github.com/user-attachments/assets/ce6cf613-fed2-4cb6-8e2a-7627c4018e45)

![Pistol-ADS](https://github.com/user-attachments/assets/9721c4c1-387b-437f-bd45-16a459a8203c)

#### Assault Rifle
![AR](https://github.com/user-attachments/assets/0cc97515-ce4e-40c7-9e0a-8597831b51f6)

![AR-ADS](https://github.com/user-attachments/assets/79e2e7d1-ed1a-4096-83ea-2f5a35bb163d)

#### Submachine Gun
![SMG](https://github.com/user-attachments/assets/cc1f6695-1afd-4e93-97f7-d6e9e870bbac)

![SMG-ADS](https://github.com/user-attachments/assets/eb79969e-04f9-4ce2-8733-6383625cf63c)

#### Snipe Rifle
![SR](https://github.com/user-attachments/assets/fbe83236-db6b-4e21-8f2f-c94a6677633d)

![SR-ADS](https://github.com/user-attachments/assets/234f5dbf-cdc4-4b1d-b60d-efb7f94e8dd5)

#### Shotgun
![SG](https://github.com/user-attachments/assets/9d1f3107-2255-471d-8e9d-3f0c19c04fdb)

![SG-ADS](https://github.com/user-attachments/assets/489ba47f-c9a3-4a0d-ab8a-2ef2af052cf6)

#### Grenade Launcher
애니메이션 애셋을 충분히 확보하지 못해 어색한 부분이 있습니다.

![GL](https://github.com/user-attachments/assets/d0729887-7a0b-43ee-805b-f4190aaa48c2)

#### Rocket Launcher
애니메이션 애셋을 충분히 확보하지 못해 어색한 부분이 있습니다.

![RL](https://github.com/user-attachments/assets/e4358ba0-a001-4110-858e-a5bfd555b313)

#### Grenade
![Grenade](https://github.com/user-attachments/assets/3f7aab81-fe58-4103-9a34-80bc54563fd1)

---

### Miscellaneous

#### Hit Marker
적에게 데미지를 주면 크로스헤어 주변에 Hit Marker가 표시됩니다.  
적을 처치하면 이를 강조하는 Hit Marker가 표시됩니다.

![AR](https://github.com/user-attachments/assets/0cc97515-ce4e-40c7-9e0a-8597831b51f6)

#### Impact Indicator
실제 총알이 부딪힐 부분을 Impact Indicator로 표시합니다.

![image](https://github.com/user-attachments/assets/eb662534-b71f-44b5-8608-f9984b4b9c02)

#### Hit Direction Indicator
공격을 맞은 방향을 알려주는 Hit Direction Indicator가 표시됩니다.

![HitDirectionIndicator](https://github.com/user-attachments/assets/581eea6f-f245-47b8-8987-902ab2369589)

#### Elimination
체력이 0이 되면 사망하고, 일정 시간이 지나면 리스폰됩니다.  

![Elimination](https://github.com/user-attachments/assets/28a55a6b-83ea-4f25-922c-6bd453fbd7a6)

#### Invincible
리스폰된 후 일정 시간 동안 무적 상태가 되어 데미지를 입지 않습니다.


#### Scoreboard, Mini Scoreboard
플레이어의 입장, 퇴장, 킬, 데스 등의 이벤트가 실시간으로 Scoreboard와 Mini Scoreboard에 연동됩니다.

플레이어가 입장하거나 퇴장하면 실시간으로 Scoreboard에 표시됩니다.

![Update-Scoreboard-Player-In](https://github.com/user-attachments/assets/d7baf344-3e09-4083-80ba-11259c39c383)

![Update-Scoreboard-Player-Out](https://github.com/user-attachments/assets/728c1c01-785a-41c6-abea-a5a2ded9146f)

적을 처치하거나 플레이어가 처치되면 실시간으로 Scoreboard와 상단 타이머 아래의 Mini Scoreboard에 표시됩니다.

![Update-Scoreboard-Kill](https://github.com/user-attachments/assets/2341e847-1fed-4d6a-8f0f-d61d5c64f0fc)

![Update-Scoreboard-Death](https://github.com/user-attachments/assets/de4d6cd9-2014-415a-91e8-1d9cc72aed00)

---

### Game Mode
게임 모드는 개인전과 팀 데스매치가 있습니다.
두 모드 모두 게임에 참가할 수 있는 최대 플레이어 수와 게임의 목표 킬 수를 설정할 수 있습니다.

![image](https://github.com/user-attachments/assets/bc6cdb30-3fa5-4816-8ba9-ff37b9933b52)

![image](https://github.com/user-attachments/assets/c6692598-1267-4933-9c91-b14d8928888b)

#### 팀 데스매치
세션 생성 창에서 팀 데스매치 세션을 생성할 수 있습니다.

![image](https://github.com/user-attachments/assets/37c035b3-9446-4bee-8cc7-46ad54baf57c)

로비에서 게임에 참가할 팀을 선택할 수 있습니다.

맵에 각 팀의 진영이 존재하며, 플레이어는 자신이 속한 팀의 진영에서만 리스폰됩니다.  
한 팀이 목표 킬 수를 달성하거나 제한 시간이 종료되면 게임이 종료됩니다.  
먼저 목표 킬 수를 달성한 팀이 승리합니다.


#### 개인전
세션 생성 창에서 개인전 세션을 생성할 수 있습니다.

![image](https://github.com/user-attachments/assets/66d0a604-b9e8-4a3e-84e8-42a3a94301c4)

맵에 배치된 Player Spawn 중 랜덤한 곳에서 리스폰됩니다.  
모든 플레이어의 킬 수 합이 목표 킬 수를 달성하거나 제한 시간이 종료되면 게임이 종료됩니다.    
게임이 종료되면 순위가 매겨집니다.


---

### Chat
실시간으로 표시되는 채팅 기능을 제공합니다.  
플레이어의 입장, 퇴장을 알리는 시스템 메시지가 출력됩니다.

#### 로비 내
로비에서 모든 플레이어에게 표시되는 전체 채팅을 보낼 수 있습니다.
![FFA-Lobby-Chat](https://github.com/user-attachments/assets/466fa628-44b2-4a11-836f-f8b2a196097f)

![TDM-Lobby-Chat](https://github.com/user-attachments/assets/d7c1f623-4c29-4fd6-8eab-317ac9e5b166)

#### 매치 내
개인전에서 모든 플레이어에게 표시되는 전체 채팅을 보낼 수 있습니다.

![FFA-InGame-All-Chat](https://github.com/user-attachments/assets/95b6060d-ea7b-4dcb-a6fc-86d87ea58bbd)

팀 데스매치에서 채팅을 보낼 대상을 전체 또는 팀 중에서 선택할 수 있습니다.  
아군은 파란색, 적군은 빨간색으로 이름을 표시해 구분합니다.

![TDM-InGame-All-Chat](https://github.com/user-attachments/assets/48a9b1de-baca-4a34-afdd-c8d1049b0cc6)

![TDM-InGame-Team-Chat](https://github.com/user-attachments/assets/31b4fe89-e209-40d7-9e77-a9fde050939f)

---

### Settings
간단한 환경 설정을 제공합니다.

![image](https://github.com/user-attachments/assets/704e1393-eaef-484c-a5bf-516a05fe8f6e)

#### 비디오
언리얼 엔진의 Engine Scalability에 따른 그래픽 설정을 제공합니다.  
하드웨어 품질에 적합한 설정을 선택해주는 품질 자동 설정 기능을 제공합니다.

![image](https://github.com/user-attachments/assets/521c4ab8-b933-46cf-968a-ba5cadb53be0)

![image](https://github.com/user-attachments/assets/f9a63161-1c3d-4001-a6c5-db38639a8ab6)

![image](https://github.com/user-attachments/assets/d7e85831-cc2b-4ed8-acdf-0bae66334fa0)

FPS, Ping을 표시하는 성능 표시 기능을 제공합니다.

![PerformaneIndicator2](https://github.com/user-attachments/assets/ccabae2d-9d8d-4033-9323-7485f0f286a5)

#### 오디오
마스터 볼륨 설정 기능을 제공합니다.

![image](https://github.com/user-attachments/assets/fb315fbc-3803-4fa0-8d48-59f2b8a16768)

### 마우스
마스터 감도 설정 기능을 제공합니다.

![image](https://github.com/user-attachments/assets/8d4f97c6-7700-4fd2-85cc-a2be952642f0)


## 회고
프로젝트를 진행하면서 느낀 경험을 아쉬웠던 점과, 이를 개선해 다음 프로젝트에서 시도해볼 점으로 나누어 회고를 남겨본다.  

### 아쉬웠던 점
프로젝트를 진행하면서 여러 가지 아쉬운 점을 많이 느꼈다.  

#### 기획과 설계의 부족
프로젝트의 기획과 설계가 부족하여 효율적인 시간 관리와 프로젝트 관리가 미흡했다.  
개발을 진행하면서 기획이 수시로 바뀌어 기존 기능을 버리고 새롭게 기능을 구현하는 경우가 많았고, 이는 많은 시간을 소모하게 만들었다.  
또한, 리소스의 부재로 인해 리소스를 찾거나 대체하는 방법을 사용해야 했고, 이에 따른 추가적인 시간도 많이 소요되었다.  

#### 개발 실력 부족
직접적인 개발 실력의 부족함 역시 아쉬운 점이었다.  
프로토타입을 넘어 디테일한 부분을 구현하기 위해서는 더 높은 언리얼 엔진의 숙련도가 필요했다.  
특히 애니메이션 부분의 디테일을 살리기 위해 언리얼 엔진5의 샘플 프로젝트인 Lyra 프로젝트의 애니메이션 구현을 참고했지만, 참고하는 과정 조차 쉽지 않았다.  
성능 최적화 부분도 부족함을 느꼈다.

### 다음에 시도해볼 점
아쉬웠던 점을 개선하거나 새롭게 다음 프로젝트에서 시도해볼 점을 남겨본다.  

#### 기획과 설계 강화
프로젝트를 새롭게 시작할 때 최소한의 기획과 설계를 먼저 수행해 좀 더 체계화된 일정으로 개발을 진행할 필요가 있다.  
이를 통해 대략적으로 필요한 리소스를 미리 준비하는 방식도 유용할 것이다.  

#### Git과 Github 활용
Git과 Github를 활용하여 이슈를 발행하고 브랜치를 나누어 개발하는 방식을 이 프로젝트 후반부에서 사용했는데, 혼자 개발하더라도 충분히 사용 가치가 높다고 느꼈다.  
앞으로도 이 방식을 사용할 것이다.  

#### GAS(Game Ability System) 도입
다음 프로젝트에서는 GAS 방식을 적극 채용할 것이다.  
이를 통해 멀티플레이어를 고려해 개발하는 것이 더 편리할 것이라 기대한다.  
GAS 방식은 기존의 언리얼 엔진을 통한 게임 개발의 방식과는 차별화된 방식이기 떄문에 습득할 필요가 있으며, 이번 프로젝트를 진행하면서 그 필요성을 많이 느꼈다.  

#### 최적화 향상
최적화 부분은 다음 프로젝트를 진행하기 전에 이번 프로젝트에서 좀 더 숙련도를 높일 필요가 있다.  
단순한 성능 뿐만 아니라 네트워크 관련 기능에서도 최적화가 더 필요함을 느꼈다.  

#### 지식 확장
언리얼 엔진의 숙련도 뿐만 아니라 C++, 수학, 그래픽스 지식 모두 더 필요하다는 것을 깨달았다.  
다음 프로젝트를 진행하면서도 꾸준히 필요한 지식들을 공부해나갈 것이다.
