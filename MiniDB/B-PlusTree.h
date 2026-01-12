#pragma once
#include "Common.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

/**
 * @struct Node
 * @brief B+ Tree 노드 (인메모리 연산 및 직렬화 대상)
 */
struct Node {
    bool isLeaf;             ///< true: 리프 노드(데이터 O), false: 내부 노드(인덱스 X)
    int keyCount;            ///< 현재 저장된 유효 키(Key) 개수
    Node* parent;            ///< 부모 노드 포인터 (Split 역추적용)
    Node* nextLeaf;          ///< 다음 리프 노드 포인터 (Leaf 노드 전용)

    int keys[ORDER];         ///< 키(Key) 저장 배열 (고정 길이)
    Node* children[ORDER + 1]; ///< 자식 노드 포인터 배열 (Internal 노드 전용)
    string values[ORDER];    ///< 실제 데이터(Value) 배열 (Leaf 노드 전용)

    /// @brief 생성자 (leaf: 리프 여부)
    Node(bool leaf = false) {
        isLeaf = leaf;
        keyCount = 0;
        parent = nullptr;
        nextLeaf = nullptr;
        for (int i = 0; i < ORDER + 1; i++) children[i] = nullptr;
    }
};

/**
 * @class BPlusTree
 * @brief B+ Tree 자료구조 관리 (Insert, Search, Split 로직 포함)
 */
class BPlusTree {
private:
    Node* root; ///< 트리의 루트 노드

    // [Internal Logic]
    void splitLeaf(Node* node);     ///< 리프 노드 분할(Split) 수행
    void splitInternal(Node* node); ///< 내부 노드 분할 수행

    /// @brief 중간 키를 부모로 승격(Promote)
    void insertIntoParent(Node* oldChild, int key, Node* newChild);

public:
    BPlusTree(); ///< 생성자

    string Search(int key);             ///< Key로 Value 검색 (없으면 빈 문자열)
    void Insert(int key, string value); ///< 새로운 Key-Value 쌍 삽입
    void PrintTree();                   ///< 트리 구조 전체 출력 (디버깅)
};