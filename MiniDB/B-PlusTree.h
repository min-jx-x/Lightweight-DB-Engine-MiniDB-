#pragma once
#include "Common.h"      // 여기에 struct Page (또는 TreePage)가 정의되어 있어야 함
#include "DiskManager.h" // 디스크 입출력 관리자
#include <string>
#include <vector>
#include <iostream>

using namespace std;

/**
 * @class BPlusTree
 * @brief 디스크 기반 B+ Tree (DiskManager와 연동)
 */
class BPlusTree {
private:
    DiskManager* diskManager; // 파일 입출력 담당 객체
    int rootPageID;           // 루트 노드의 페이지 번호 (Node* root 대신 사용)

    // 내부 로직 함수들: 포인터 대신 페이지 번호(int)를 사용하도록 변경
    void splitLeaf(int pageID);
    void splitInternal(int pageID);
    void insertIntoParent(int oldChildID, int key, int newChildID);

public:
    // 생성자: DiskManager 포인터를 받아서 저장함
    BPlusTree(DiskManager* dm);

    void SetRootPageID(int id) { rootPageID = id; }
    // 메인 기능 함수들 (인자는 그대로지만 내부 구현이 바뀔 예정)
    string Search(int key);
    void RangeSearch(int startKey, int endKey);
    void Insert(int key, string value);
    void PrintTree();
};