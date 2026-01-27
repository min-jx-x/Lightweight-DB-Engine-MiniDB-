#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <cstring>
#include "DiskManager.h"
#include "B-PlusTree.h"
#include "Common.h"

// 디스크에 가상의 트리 데이터를 수동으로 구축하는 함수
void BuildMockTree(DiskManager& disk) {
    // 상황 설정:
    // Root(Page 0) -> [ Key: 10 ]
    //                 /       \
    //      Leaf(Page 1)       Leaf(Page 2)
    //      [1, 5]             [10, 20]

    // 1. Leaf Node 1 생성 (PageID: 1)
    TreePage leaf1;
    leaf1.pageID = 1;
    leaf1.isLeaf = 1;
    leaf1.keyCount = 2;
    leaf1.keys[0] = 1; strcpy(leaf1.values[0], "Value_1");
    leaf1.keys[1] = 5; strcpy(leaf1.values[1], "Value_5");

    // 2. Leaf Node 2 생성 (PageID: 2)
    TreePage leaf2;
    leaf2.pageID = 2;
    leaf2.isLeaf = 1;
    leaf2.keyCount = 2;
    leaf2.keys[0] = 10; strcpy(leaf2.values[0], "Value_10");
    leaf2.keys[1] = 20; strcpy(leaf2.values[1], "Value_20");

    // 3. Root Node 생성 (PageID: 0, Internal)
    TreePage root;
    root.pageID = 0;
    root.isLeaf = 0;
    root.keyCount = 1;
    root.keys[0] = 10; // 10보다 작으면 왼쪽, 크거나 같으면 오른쪽
    root.childrenPageIDs[0] = 1; // 왼쪽 자식 (Page 1)
    root.childrenPageIDs[1] = 2; // 오른쪽 자식 (Page 2)

    // 4. 디스크에 쓰기 (TreePage를 Page로 캐스팅)
    // 주의: 실제 DB라면 AllocatePage()로 ID를 받아야 하지만, 테스트라 강제 지정
    if (disk.GetNumPages() == 0) {
        disk.AllocatePage(); // 0
        disk.AllocatePage(); // 1
        disk.AllocatePage(); // 2
    }

    disk.WritePage(0, reinterpret_cast<Page&>(root));
    disk.WritePage(1, reinterpret_cast<Page&>(leaf1));
    disk.WritePage(2, reinterpret_cast<Page&>(leaf2));

    std::cout << ">> 가상의 트리 데이터가 디스크에 생성되었습니다.\n";
}

int main() {
    try {
        std::string dbName = "search_test.db";
        DiskManager disk(dbName);

        // 1. 테스트용 데이터 구축 (Insert 함수 대신 수동으로 작성)
        BuildMockTree(disk);

        // 2. B+ Tree 초기화
        BPlusTree tree(&disk);
        tree.SetRootPageID(0); // 루트가 0번 페이지라고 알려줌

        std::cout << "\n=== B+ Tree Search Test ===\n";

        // 3. 검색 테스트
        int targets[] = { 1, 5, 10, 20, 99 };

        for (int key : targets) {
            std::cout << "Search Key " << key << ": ";
            string result = tree.Search(key);

            if (result != "") {
                std::cout << "Found Value = " << result << std::endl;
            }
            else {
                std::cout << "Not Found." << std::endl;
            }
        }

    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}