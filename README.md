# PrototypeX

> 언리얼 엔진 5 기반 **소울라이크 액션 프로토타입**
> 내일배움캠프 언리얼 트랙 8기 · 챕터 3 팀 프로젝트 (2026-04-27 ~ 05-28)

패링과 스태미나를 축으로 한 근접 전투에, 아이템 획득과 보스 전투를 붙인 3인칭 액션 프로토타입입니다.

<br>

## 플레이 영상

https://www.youtube.com/watch?v=tWZ66kZ_0fg

<br>

## 역할 분담

| 담당자 | 파트 | 주요 구현 | 소스 경로 | 상세 |
|---|---|---|---|---|
| 곽민우 | 적 AI | 적(몬스터) AI | `PrototypeXMob.h/.cpp` | |
| 장성윤 | UI | UI 전반 | Blueprint / UMG | |
| 이중원 | 캐릭터 | 캐릭터 이동 로직 · 아이템 | `LJW/Character`, `LJW/Controller`, `LJW/Item` | |
| 김승하 | 전투 · GameMode | 게임모드 및 전투 로직 | `Combat/`, `MainGameMode.h/.cpp` | [문서 →](https://github.com/ClimbSunSet/portfolio/blob/main/PrototypeX.md) |

> 담당 파트를 따로 정리한 문서가 있으면 **상세** 칸에 링크를 추가해 주세요.

<br>

## 기술 스택

**Unreal Engine 5** · C++ · Blueprint

DataTable 기반 수치 관리 · AnimNotify 연동 히트 판정 · ActorComponent 기반 전투 설계 · 멀티캐스트 델리게이트

<br>

## 프로젝트 구조

```
Source/TeamProject/
├── Combat/                          전투 시스템
│   ├── AttackSystemComponent        히트 판정 · 패링 · 히트스톱/슬로우 · 보스 패턴
│   ├── StatusComponent              HP · 스태미나 · 데미지 계산 · 사망
│   └── DataTable/                   캐릭터 / 공격 / 보스 / 스태미나 소모 스탯 행
│
├── LJW/                             캐릭터 · 아이템
│   ├── Character/                   플레이어 캐릭터
│   ├── Controller/                  플레이어 컨트롤러
│   ├── Item/                        아이템 · 인벤토리 · 트리거 박스
│   └── GameUtilHeader/
│
├── PrototypeXMob                    적 몬스터
├── MainGameMode                     게임 오버 판정
└── MainGameInstance                 레벨 간 플레이어 상태 · 인벤토리 보관
```

<br>

## 주요 시스템

**전투** — 공격(`AttackSystemComponent`)과 상태(`StatusComponent`) 두 컴포넌트로 나눠, 플레이어·일반 몹·보스가 같은 컴포넌트를 공유하고 차이는 데이터테이블 행 이름으로만 냅니다. 덕분에 패링이 별도 상태 머신 없이 성립합니다 — 몹의 공격 노티파이가 자기 판정 윈도우를 열어두면 플레이어가 그 값을 읽어 판정합니다.

**타격감** — 애님 노티파이 구간 동안 무기 두 지점을 잇는 라인트레이스로 히트를 판정하고, `CustomTimeDilation`으로 히트스톱과 히트슬로우를 적용합니다.

**수치 관리** — 체력·공격력·패링 범위·히트스톱 시간·보스 반경을 4종 `FTableRowBase`로 외부화해, C++ 빌드 없이 CSV 수정만으로 밸런스를 조정할 수 있습니다.
