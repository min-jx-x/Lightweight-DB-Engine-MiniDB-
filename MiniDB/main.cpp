#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <cstdio> // remove() 함수 사용을 위해

#include "DiskManager.h"
#include "B-PlusTree.h"
#include "Common.h"

using namespace std;

int main() {
    string dbFileName = "test_bptree.db";

    // 1. 테스트를 위해 기존 DB 파일 삭제 (항상 새로운 트리에서 시작)
    // 주의: 실제 서비스에서는 함부로 삭제하면 안 됩니다.
    std::remove(dbFileName.c_str());
    cout << "Existing DB file removed. Starting fresh..." << endl;

    try {
        // 2. 매니저 및 트리 초기화
        DiskManager dm(dbFileName);
        BPlusTree tree(&dm);

        cout << "\n=== [TEST 1] Insert Data (1 ~ 20) ===" << endl;
        // ORDER가 3이므로, 키가 3개 이상 찰 때마다 분할이 일어납니다.
        // 1~20까지 넣으면 트리의 높이가 높아지고(Level 증가) 내부 노드 분할이 발생합니다.
        for (int i = 1; i <= 20; i++) {
            string value = "Val_" + to_string(i);
            tree.Insert(i, value);

            // 중간 과정 확인용 (너무 길면 주석 처리하세요)
            // cout << "Inserted Key: " << i << endl;
        }
        cout << "-> Insertion Complete." << endl;

        // 3. 트리 구조 시각화 (BFS)
        // 레벨별로 노드가 잘 나뉘었는지 확인합니다.
        tree.PrintTree();

        cout << "\n=== [TEST 2] Range Search Verification ===" << endl;

        // Case A: 작은 범위 (단일 페이지 혹은 두 페이지)
        cout << "Search (5 ~ 8): ";
        tree.RangeSearch(5, 8);

        // Case B: 넓은 범위 (여러 리프 노드를 타고 이동해야 함)
        cout << "Search (3 ~ 17): ";
        tree.RangeSearch(3, 17);

        // Case C: 없는 범위 포함
        cout << "Search (18 ~ 30): ";
        tree.RangeSearch(18, 30);

        // Case D: 전체 범위 스캔
        cout << "Search (0 ~ 100): ";
        tree.RangeSearch(0, 100);

        cout << "\n=== [TEST 3] Single Search Verification ===" << endl;
        int searchTarget = 15;
        string result = tree.Search(searchTarget);
        if (!result.empty()) {
            cout << "Found Key " << searchTarget << ": " << result << endl;
        }
        else {
            cout << "Key " << searchTarget << " not found." << endl;
        }

    }
    catch (const std::exception& e) {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}