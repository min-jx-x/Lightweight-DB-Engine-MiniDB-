#pragma once
#include "Common.h" 
#include <iostream>
#include <string>
#include <vector>
using namespace std;

/* [구조체] Node
 B+ Tree의 기본 구성 단위입니다.
 나중에 파일 입출력 시 이 구조체 형태 그대로 저장됩니다.
*/

struct Node {
    bool isLeaf;             // true면 리프 노드(데이터 O), false면 내부 노드(데이터 X)
    int keyCount;            // 현재 이 노드에 들어있는 키의 개수
    Node* parent;            // 부모 노드를 가리키는 포인터 (Split 할 때 위로 올라가기 위함)
    Node* nextLeaf;          // [Leaf Only] 다음 리프 노드를 가리키는 포인터 (Linked List 구현용)

    // [데이터 저장 공간] (고정 길이 배열 사용)
    int keys[ORDER];         // 기준값(Key)들을 저장하는 배열

    // 내부 노드용: 자식 노드들의 주소 (포인터)
    // 자식은 키보다 항상 1개 더 많으므로 ORDER + 1
    Node* children[ORDER + 1];

    // 리프 노드용: 실제 데이터(Value)
    // 계획서상 Value가 문자열이므로 string 사용
    string values[ORDER];

    // [생성자] 노드 초기화
    Node(bool leaf = false) {
        isLeaf = leaf;
        keyCount = 0;
        parent = nullptr;
        nextLeaf = nullptr;

        // 포인터 배열 초기화 (쓰레기값 방지)
        for (int i = 0; i < ORDER + 1; i++) {
            children[i] = nullptr;
        }
    }
};

/* [클래스] BPlusTree
 트리를 관리하고 삽입/검색 기능을 제공하는 관리자입니다.
 */

class BPlusTree {
private:
    Node* root; // 트리의 시작점 (뿌리)

    // [내부 로직] - 외부에서는 호출할 필요 없는 도우미 함수들
    // 1. 리프 노드가 꽉 찼을 때 쪼개는 함수
    void splitLeaf(Node* node);

    // 2. 내부 노드(인덱스 노드)가 꽉 찼을 때 쪼개는 함수
    void splitInternal(Node* node);

    // 3. 쪼개진 후 중간 키(Key)를 부모에게 올려보내는 함수
    void insertIntoParent(Node* oldChild, int key, Node* newChild);

public:
    // 생성자
    BPlusTree();

    // 1. 데이터 검색 (Key로 Value 찾기)
    string Search(int key);

    // 2. 데이터 삽입
    void Insert(int key, string value);

    // 3. 트리 전체 출력 (디버깅용)
    void PrintTree();
};