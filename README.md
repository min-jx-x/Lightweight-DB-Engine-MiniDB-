# Lightweight-DB-Engine-MiniDB

디스크 기반 B+Tree 인덱스 엔진 (C++). 포인터 대신 페이지 ID로 노드를 참조하는 방식으로, 인메모리가 아니라 실제 파일 I/O 위에서 동작하는 B+Tree를 처음부터 구현한 프로젝트입니다.

## 주요 기능

- **DiskManager**: 4096바이트 고정 크기 페이지 단위로 파일을 읽고 쓰는 저수준 저장 계층
- **디스크 기반 B+Tree**
  - `Insert(key, value)`: 리프 노드가 가득 차면 자동으로 분할(split) 후 재삽입
  - `Search(key)`: 루트부터 리프까지 페이지 ID를 따라 내려가며 탐색
  - `RangeSearch(start, end)`: 리프 노드 간 연결 리스트를 이용한 범위 검색
  - `Remove(key)`: 삭제 후 필요 시 재분배(redistribute) 또는 병합(merge), 루트 높이 조정(adjustRoot)까지 처리
- **메타 페이지(MetaPage)**: 루트 페이지 ID 등 트리 메타정보를 0번 페이지에 별도로 관리해 재시작 시에도 트리 상태를 복원

## 아키텍처

```
main.cpp
  └── BPlusTree (B-PlusTree.h/.cpp)
        └── DiskManager (DiskManager.h/.cpp)
              └── Page (4096 bytes, Common.h)
```

- 모든 노드는 메모리 포인터가 아니라 **페이지 ID(int)**로 서로를 참조합니다. 트리를 디스크에 그대로 저장하고 다시 읽어올 수 있도록 하기 위한 설계입니다.
- 리프 노드는 `nextLeafPageID`로 서로 연결되어 있어, 범위 검색 시 리프 레벨만 순차적으로 순회합니다.
- 삽입/삭제로 트리 구조가 바뀔 때마다 관련 페이지를 즉시 `WritePage`로 디스크에 반영합니다.

## 빌드 및 실행

Windows + Visual Studio 환경 기준입니다.

1. `MiniDB.sln`을 Visual Studio(2019 이상 권장)에서 엽니다.
2. 빌드(Ctrl+Shift+B) 후 실행(F5, 또는 Ctrl+F5)합니다.
3. `main.cpp`가 데모 겸 동작 확인용 코드입니다.
   - 실행 시 기존 `test_bptree.db` 파일을 삭제하고 새로 시작합니다.
   - 1~20 삽입 → 트리 구조 출력(BFS) → 범위 검색(5~8, 3~17, 18~30, 0~100) → 단일 키(15) 검색 순서로 동작을 확인합니다.

## 한계 및 알려진 제약

프로덕션 데이터베이스가 아니라 B+Tree 인덱스 구조 자체를 디스크 위에서 동작하도록 만드는 데 집중한 프로젝트입니다. 아래는 의도적으로 범위에서 제외한 부분입니다.

- **Free list 없음**: 노드 병합으로 비워진 페이지가 재사용되지 않고 그대로 남습니다.
- **동시성 제어 없음**: 단일 스레드 사용을 가정합니다 (락, 트랜잭션 없음).
- **쿼리 계층 없음**: SQL이나 별도 쿼리 언어 없이, Key-Value 형태의 인덱스 엔진 코어만 구현했습니다.
- **자동화된 테스트 프레임워크 없음**: `main.cpp`의 콘솔 출력으로 동작을 육안 확인하는 수준입니다.

## 향후 개선 방향

- [ ] 삭제된 페이지를 재사용하는 free list 구현
- [ ] 멀티스레드 환경을 위한 동시성 제어 (락 또는 latch)
- [ ] 최소한의 쿼리 인터페이스 (CLI 기반 CRUD 명령)
- [ ] 단위 테스트 추가

## 기여자 및 역할

페어 프로젝트로 진행했습니다.

- **[DaeHyeon Kim](https://github.com/eogus6833)**: DiskManager(페이지 기반 파일 I/O) 초기 구현, PrintTree(BFS 시각화)·Search 초기 버전, Insert/부모 노드 갱신 로직, 통합 테스트
- **[min-jx-x](https://github.com/min-jx-x)**: 포인터 기반 → 디스크(페이지 ID) 기반 리팩터링, splitLeaf/splitInternal, 삭제 시 재분배·병합(rebalancing) 로직, MetaPage를 통한 루트 정보 영속화·복구
