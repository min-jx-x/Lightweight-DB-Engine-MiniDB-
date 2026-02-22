#pragma once
#include <cstring> // for memset, memcpy

// 페이지 크기 및 B+ Tree 차수 설정
const int PAGE_SIZE = 4096; ///< 디스크 페이지 크기 (4KB 고정)
const int ORDER = 3;        ///< B+ Tree 차수 (한 노드에 최대 3개 키 저장 가능)

struct MetaPage {
    int rootPageID;    // 루트 노드의 위치
    int nextPageID;    // 다음에 할당할 페이지 번호
    char padding[PAGE_SIZE - sizeof(int) * 2];

    MetaPage() : rootPageID(-1), nextPageID(1) {}
};

/**
 * @struct TreePage
 * @brief 디스크에 저장되는 B+ Tree 노드 구조체 (Node* 포인터 제거 버전)
 * @details Node 구조체를 대체하며, 파일의 4KB 블록 위에 그대로 덮어씌워 사용됩니다.
 */
struct TreePage {
    // [Header: 메타 데이터]
    int pageID;           ///< 현재 페이지의 고유 번호 (주민번호)
    int isLeaf;           ///< 1: 리프 노드, 0: 내부 노드 (bool 대신 int 사용)
    int keyCount;         ///< 현재 저장된 키(Key)의 개수
    int parentPageID;     ///< 부모 페이지의 ID (없으면 -1) [Node* parent 대체]
    int nextLeafPageID;   ///< 다음 리프 페이지의 ID (없으면 -1) [Node* nextLeaf 대체]

    // 실제 데이터 영역
    int keys[ORDER];                 ///< 키(Key) 저장 배열
    int childrenPageIDs[ORDER + 1];  ///< 자식 페이지 ID 배열 [Node* children 대체]
    char values[ORDER][128];         ///< 실제 데이터(Value) 배열 (string 대신 고정 길이 char 사용)

    // 4KB 크기 맞춤
    // 구조체 전체 크기가 정확히 4096 바이트가 되도록 남는 공간을 채움
    char padding[PAGE_SIZE - (
        sizeof(int) * 5 +               // Header 변수 5개 (pageID ~ nextLeafPageID)
        sizeof(int) * ORDER +           // keys 배열
        sizeof(int) * (ORDER + 1) +     // childrenPageIDs 배열
        128 * ORDER                     // values 배열
        )];

    /// @brief 생성자: 모든 데이터를 0 또는 -1로 초기화
    TreePage() {
        Clear();
    }

    /// @brief 메모리 초기화 함수 (재사용 시 필수)
    void Clear() {
        pageID = -1;
        isLeaf = 0;
        keyCount = 0;
        parentPageID = -1;
        nextLeafPageID = -1;

        // 배열 및 패딩 0으로 초기화
        std::memset(keys, 0, sizeof(keys));
        std::memset(childrenPageIDs, -1, sizeof(childrenPageIDs));
        std::memset(values, 0, sizeof(values));
        std::memset(padding, 0, sizeof(padding));
    }
};