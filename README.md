# Minigames

여러 명이 함께 즐기는 **멀티플레이어 파티 미니게임** 프로젝트입니다.
타이틀 → 로비 → 여러 라운드의 미니게임 → 최종 시상대로 이어지는 토너먼트 구조로,
매 라운드 서로 다른 미니게임을 진행하며 순위를 겨룹니다.

## 데모 영상

[![데모 영상](https://img.youtube.com/vi/2TVSEGGkTDs/maxresdefault.jpg)](https://www.youtube.com/watch?v=2TVSEGGkTDs)

## 개요

| 항목 | 내용 |
| --- | --- |
| 장르 | 멀티플레이어 파티 게임 |
| 개발 기간 | 2026.06.25 ~ 2026.07.21 |
| 인원 | 6명 |
| 엔진 | Unreal Engine **5.5** |
| 언어 | C++ (핵심 로직) + Blueprint (연출·에셋 연결) |
| 플랫폼 | Windows |
| 네트워크 | 리슨 서버 + **EOS(Epic Online Services)** P2P (IP 직접 접속은 로컬 테스트용) |
| 게임 모듈 | `Minigames` (Runtime) |

## 미니게임 목록

프로젝트에는 세 가지 미니게임이 구현되어 있습니다.

| 미니게임 | 레벨 | 설명 |
| --- | --- | --- |
| **Pass the Bomb** (폭탄 돌리기) | `L_MG_01_BombTag` | 폭탄을 다른 플레이어에게 넘기며 폭발을 피하는 술래잡기형 게임 |
| **Flag Game** (깃발 뺏기) | `L_MG_02_Flag` | 깃발을 차지·유지하며 점수를 겨루는 게임 |
| **Button Ownership** (버튼 점령) | `L_MG_03_Button` | 버튼/발판을 점령해 소유권을 다투는 플랫폼형 게임 |

## 게임 흐름

```
Title  →  Lobby  →  Round 1  →  Round 2  →  Round 3  →  Final Result
(타이틀)  (로비)     (미니게임)   (미니게임)   (미니게임)   (시상대)
```

- **Title** (`Title`) — 시작 화면, 방 생성/접속 진입점
- **Lobby** (`L_Lobby`) — 플레이어 대기, **색상 선택**, 레디/카운트다운, 닉네임 표시
  - 로비에서 **미니게임 순서를 랜덤**으로 정해 매 판마다 라운드 구성이 달라집니다.
- **Round 1~3** — 랜덤으로 정해진 순서대로 미니게임 진행
- **Final Result** (`L_FinalResult`) — 3D 시상대 + 최종 순위 UI

라운드 진행 상태는 `ERoundState`(Lobby / Round1~3 / FinalResult), 각 매치 내부 상태는
`EMatchState`(Entering → Waiting → PlayingCutScene → Playing → RoundResult → Ending)로 관리됩니다.

## 조작법

| 키 | 동작 |
| --- | --- |
| `W` `A` `S` `D` | 이동 |
| `Space` | 점프 |
| `E` | 폭탄 넘기기 / 깃발 뺏기 |

## 주요 기능

- **심리스 트래블(Seamless Travel)** 기반 레벨 전환 및 로딩/규칙 안내 화면
- **로비 색상 배정** — 플레이어별 고유 색상 선택 및 복제
- **닉네임 시스템** — 접속 시 입력한 닉네임을 머리 위 이름표로 표시
- **채팅** — 인게임 채팅 및 팝업 알림
- **미니맵** — 아군/적/오브젝트 아이콘 표시
- **관전(Spectator)** — 탈락/사망 후 관전 폰 전환
- **HUD / 결과 UI** — 미니게임별 HUD, 라운드 결과창, 최종 시상대

## 프로젝트 구조

```
Minigames.uproject          # 프로젝트 파일 (UE 5.5)
Config/                     # DefaultEngine.ini 등 설정
Content/
  Minigames/
    Level/                  # 게임 레벨 (Title, L_Lobby, L_MG_01~03, L_FinalResult)
    Blueprint/              # BP 클래스 (GameInstance 등)
Source/Minigames/
  GameMode/                 # 미니게임별 GameMode (Flag, PassBomb, Lobby, FinalResult ...)
  GameState/ PlayerState/   # 복제되는 게임/플레이어 상태
  Character/                # 플레이어 캐릭터, 관전 폰
  Controller/               # 플레이어/타이틀 컨트롤러
  Component/                # 상태·깃발·이름표·미니맵 등 액터 컴포넌트
  Gimmick/                  # 폭탄, 깃발, 버튼, 이동 발판 등 상호작용 액터
  GameInstance/             # GameInstance, 온라인 서브시스템(EOS)
  UI/                       # UMG 위젯 (로비/채팅/미니맵/결과 등)
  Data/ Type/               # 데이터 애셋, 열거형·구조체 정의
```

클래스는 `MG` 접두사(예: `MGFlagGameModeBase`), UMG 위젯은 `UW_` 접두사(예: `UW_LobbyLayout`)를 사용합니다.

## 빌드 및 실행

### 요구 사항

- **Unreal Engine 5.5**
- IDE: JetBrains **Rider** 또는 **Visual Studio 2022** (C++ 게임 개발 워크로드)
- Windows

### 순서

1. 저장소를 클론합니다.
2. `Minigames.uproject`를 우클릭 → **Generate Visual Studio project files**.
3. `Minigames.sln`을 열어 빌드하거나, `.uproject`를 더블 클릭해 에디터를 실행합니다.
4. 에디터에서 플레이어 수를 2명 이상으로 설정하고 **Play**로 멀티플레이 테스트를 진행합니다.

> EOS 온라인 기능을 사용하려면 Epic Dev Portal에서 발급받은 자격 정보를 EOS 설정에 입력해야 합니다.

## 에셋 크레딧

### 3D · 아트 에셋 ([Fab](https://fab.com))

모든 에셋은 [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/) 라이선스로, 출처 표기 후 사용했습니다.

| 용도 | 에셋 | 제작자 | 링크 |
| --- | --- | --- | --- |
| 캐릭터 | Free Pack - Stick Man (Rigged) | PolyOne Studio | https://fab.com/s/cbfac6296041 |
| 버튼 맵 | Solarpunk Village | LowPolyBoy | https://www.fab.com/listings/2e12eb0e-fdd7-45b8-b599-d1dbaa2c01d1 |
| 버튼 맵 | Very Casual Asset Pack of Flying Stone Islands | Incg5764 | https://www.fab.com/listings/e2ef66d1-9f83-4d20-8af8-5935916beb77 |
| 폭탄 | Bomb | Berkan | https://www.fab.com/listings/3f15e62b-664d-48bb-be66-fd4879ab5d9d |
| 깃발 | 3D icon PINS | Inna Sparrow | https://www.fab.com/listings/8adeaad7-b81d-49b5-aa73-9c3b87702a4c |
| 버튼 (코인 에셋으로 대체) | Low poly gold coin | hannah3dart | https://www.fab.com/listings/d31c337a-f246-4026-9a5f-3809b242e608 |

### 사운드 ([freesound.org](https://freesound.org))

| 용도 | 사운드 | 제작자 | 링크 | 라이선스 |
| --- | --- | --- | --- | --- |
| 타이틀 | Game background Music loop short | ManuelGraf | https://freesound.org/s/410574/ | [Attribution 4.0](https://creativecommons.org/licenses/by/4.0/) |
| 버튼 | Shaky Platforms | code_box | https://freesound.org/s/653811/ | [Creative Commons 0](https://creativecommons.org/publicdomain/zero/1.0/) |
| 폭탄 | Cartoon Game Theme Loop 4 | Mrthenoronha | https://freesound.org/s/513667/ | [Attribution NonCommercial 4.0](https://creativecommons.org/licenses/by-nc/4.0/) |
| 깃발 | Happy Family - bpm 100 loop | DenKyschuk | https://freesound.org/s/743726/ | [Attribution NonCommercial 4.0](https://creativecommons.org/licenses/by-nc/4.0/) |
| 시상대 | Success Resolution Video Game Fanfare Sound Effect | FunWithSound | https://freesound.org/s/456968/ | [Attribution 4.0](https://creativecommons.org/licenses/by/4.0/) |
| 미니게임 설명 UI | Waiting | LittleJest | https://freesound.org/s/361217/ | [Creative Commons 0](https://creativecommons.org/publicdomain/zero/1.0/) |
| 로비 | Free Music Background Loop 003 - Var 08 | Slaking_97 | https://freesound.org/s/586101/ | [Attribution 4.0](https://creativecommons.org/licenses/by/4.0/) |

## 팀

**8기 11팀 CH4 프로젝트** (6명)

| 팀원 | 담당 |
| --- | --- |
| **고예현** | 기획, 파이프라인, 로비, Final Result, 게임 설명 인트로, 타이틀, EOS, 미니게임(깃발 뺏기) |
| **김대경** | 파이프라인, 미니게임(폭탄 돌리기), 미니맵, 컷신 |
| **김환** | 미니게임(폭탄 돌리기), 캐릭터 애니메이션 |
| **심수민** | 미니게임(버튼 소유권 쟁탈), 채팅, 레벨 사운드 |
| **김태인** | 미니게임(버튼 소유권 쟁탈), 라운드 결과 창 |
| **이채호** | 미니게임(깃발 뺏기), 캐릭터 사운드 |
