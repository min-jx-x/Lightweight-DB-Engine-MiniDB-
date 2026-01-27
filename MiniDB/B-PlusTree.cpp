#pragma once
#include "B-PlusTree.h"
#include <iostream>
#include <queue>
#include <string>

/**
 * @brief 생성자: DiskManager를 연결하고 초기화
 */
BPlusTree::BPlusTree(DiskManager* dm) : diskManager(dm) {
    // 일단 새 트리라고 가정하고 루트 페이지가 없다고(-1) 설정
    rootPageID = -1;
}

/**
 * @brief B+ 트리에 데이터(Key, Value)를 삽입하는 메인 함수
 * @details
 * 1. 트리가 비어있다면 루트 페이지를 새로 할당합니다.
 * 2. 루트부터 리프 노드(Leaf Node)까지 페이지 ID를 타고 내려가며 탐색합니다.
 * 3. 도착한 리프 노드가 가득 찼다면 분할(Split)을 수행하고, 트리를 재정비한 뒤 재귀적으로 삽입을 시도합니다.
 * 4. 리프 노드 내 정렬된 위치에 데이터를 삽입하고 디스크에 저장(WritePage)합니다.
 * @param key 삽입할 키 (Integer)
 * @param value 삽입할 값 (String)
 */
void BPlusTree::Insert(int key, string value) {
    Page page;

    // 1. 빈 트리 처리: 루트 페이지 생성
    if (rootPageID == -1) {
        rootPageID = diskManager->AllocatePage();

        TreePage* root = reinterpret_cast<TreePage*>(page.data);
        root->Clear();
        root->pageID = rootPageID;
        root->isLeaf = 1;       // 첫 노드는 루트이자 리프입니다.
        root->keyCount = 1;
        root->parentPageID = -1;

        root->keys[0] = key;
        strncpy(root->values[0], value.c_str(), 127);
        root->values[0][127] = '\0'; // NULL 종료 문자 보장

        diskManager->WritePage(rootPageID, page);
        return;
    }

    // 2. 탐색(Traversal): 데이터가 들어갈 리프 노드 찾기
    int currentPageID = rootPageID;

    diskManager->ReadPage(currentPageID, page);
    TreePage* node = reinterpret_cast<TreePage*>(page.data);

    // 리프 노드에 도달할 때까지 자식으로 이동
    while (node->isLeaf == 0) {
        int i = 0;
        // 현재 키보다 큰 첫 번째 키를 찾습니다 (오른쪽 자식 탐색)
        while (i < node->keyCount && node->keys[i] <= key) {
            i++;
        }

        int nextPageID = node->childrenPageIDs[i];

        currentPageID = nextPageID;
        diskManager->ReadPage(currentPageID, page);
        node = reinterpret_cast<TreePage*>(page.data);
    }

    // 3. 노드가 가득 찬 경우: 분할(Split) 후 재시도
    if (node->keyCount == ORDER) {
        // 현재 리프 노드를 분할합니다. (인자로 *node 참조 전달)
        splitLeaf(currentPageID, *node);

        // 트리 구조가 변경되었으므로, 루트부터 다시 탐색하여 삽입을 시도합니다. (재귀 호출)
        Insert(key, value);
        return;
    }

    // 4. 데이터 삽입: 리프 노드 내 정렬된 위치에 삽입
    // 새로운 키가 들어갈 공간을 만들기 위해 기존 요소들을 뒤로 이동(Shift)
    int i = node->keyCount - 1;
    while (i >= 0 && node->keys[i] > key) {
        node->keys[i + 1] = node->keys[i];
        strncpy(node->values[i + 1], node->values[i], 128);
        i--;
    }

    // 키와 값 삽입
    node->keys[i + 1] = key;
    strncpy(node->values[i + 1], value.c_str(), 127);
    node->values[i + 1][127] = '\0';

    node->keyCount++;

    // 5. 저장(Persist): 변경 사항을 디스크에 기록
    diskManager->WritePage(currentPageID, page);
}


/*
* @brief Disk-version:B+ Tree에서 특정 Key에 해당하는 value 검색
* @details 루트 페이지부터 리프 페이지까지 PageID를 타고 내려가며 탐색합니다.
*/
string BPlusTree::Search(int key) {
    // 1. 예외 처리: 트리가 비어있는 경우 (루트가 존재하지 않음)
    if (rootPageID == -1) return "";

    int cursorPageID = rootPageID;  ///< 현재 탐색 중인 페이지의 번호 (책 쪽수)
    TreePage cursorPage;            ///< 디스크에서 읽어온 페이지 데이터를 담을 버퍼 (책 내용)

    // 2. [Index Layer 탐색] 리프 노드에 도달할 때까지 아래로 내려감
    while (true) {
        // [Disk I/O] TreePage를 Page&로 강제 형변환하여 데이터를 읽어옵니다.
        diskManager->ReadPage(cursorPageID, reinterpret_cast<Page&>(cursorPage));

        // 도착한 페이지가 리프 노드라면 탐색 종료 (데이터는 여기에 있음)
        if (cursorPage.isLeaf) {
            break;
        }

        // [Internal Node] 어느 자식 페이지로 내려갈지 결정
        int i = 0;
        // 현재 노드의 키들을 순회하며, 내가 찾는 key가 들어갈 위치 탐색
        while (i < cursorPage.keyCount && key >= cursorPage.keys[i]) {
            i++;
        }

        // 찾은 위치(index)에 해당하는 자식 페이지 ID로 커서 이동
        cursorPageID = cursorPage.childrenPageIDs[i];
    }

    // 3. [Data Layer 탐색] 리프 노드 내부에서 실제 데이터(Value) 찾기
    for (int i = 0; i < cursorPage.keyCount; i++) {
        // 키값이 일치하는지 확인
        if (cursorPage.keys[i] == key) {
            // char 배열(C-String)을 std::string으로 변환하여 반환
            return string(cursorPage.values[i]);
        }
    }

    return ""; ///< 리프 노드를 다 뒤졌으나 해당 키가 없음
}

/**
 * @brief 리프 노드가 꽉 찼을 때 반으로 분할(Split)하는 함수 (디스크 버전)
 * @param leafPageID 기존 꽉 찬 페이지 번호
 * @param leafPage 기존 꽉 찬 페이지의 데이터 (메모리 상의 객체)
 */
void BPlusTree::splitLeaf(int leafPageID, TreePage& leafPage) {
    // 1. 새 페이지 할당 및 초기화 
    int newPageID = diskManager->AllocatePage();
    TreePage newLeafPage;

    newLeafPage.pageID = newPageID;
    newLeafPage.isLeaf = 1;              // 리프 노드
    newLeafPage.keyCount = 0;            // 아직 비어있음
    newLeafPage.parentPageID = leafPage.parentPageID; // 형제니까 부모가 같음

    // 2. 데이터 분할 및 이동 (오른쪽 절반 -> 새 페이지)
    int splitIndex = (ORDER + 1) / 2;
    int j = 0;

    // 중간(splitIndex)부터 끝까지의 데이터를 새 페이지로 복사
    for (int i = splitIndex; i < ORDER; i++) {
        // 키 복사
        newLeafPage.keys[j] = leafPage.keys[i];

        // 값 복사 (문자열이므로 strcpy 사용 필수)
        std::strcpy(newLeafPage.values[j], leafPage.values[i]);

        newLeafPage.keyCount++;
        j++;
    }

    // 기존 페이지는 절반을 보냈으므로 개수를 줄임
    leafPage.keyCount = splitIndex;

    // 3. 리프 노드 연결 리스트연결

    newLeafPage.nextLeafPageID = leafPage.nextLeafPageID; // 1. 새 페이지가 '원래 다음'을 가리킴
    leafPage.nextLeafPageID = newPageID;                  // 2. 기존 페이지가 '새 페이지'를 가리킴

    // 4. 변경 사항 디스크에 저장 
    // 메모리에서 수정한 TreePage 구조체를 실제 파일에 덮어씁니다.
    diskManager->WritePage(leafPageID, reinterpret_cast<Page&>(leafPage));
    diskManager->WritePage(newPageID, reinterpret_cast<Page&>(newLeafPage));

    // 5. 부모 노드 갱신 (Promote)
    // 부모에게 "새 자식(newPageID)이 생겼고, 기준 키는 newLeafPage의 첫 번째 키야"라고 알림
    insertIntoParent(leafPageID, newLeafPage.keys[0], newPageID);
}

/**
 * @brief 자식 노드가 분할(Split)된 후, 부모 노드에 키와 포인터를 갱신하는 함수
 * @details 분할된 두 자식을 연결하기 위해 다음 두 가지 경우를 처리합니다:
 * 1. 루트 분할(Root Split): 부모가 없는 경우, 새로운 루트 노드를 생성하여 트리 높이를 높입니다.
 * 2. 일반 삽입(Normal Insert): 기존 부모 노드에 (Key, NewChildID)를 추가합니다. 부모도 가득 찬 경우 재귀적으로 분할합니다.
 * @param oldChildID 기존 자식 노드의 페이지 번호
 * @param key 부모 노드로 올려보낼 구분자 키(Separator Key)
 * @param newChildID 새로 생성된 자식 노드의 페이지 번호
 */
void BPlusTree::insertIntoParent(int oldChildID, int key, int newChildID) {
    Page oldPage;
    diskManager->ReadPage(oldChildID, oldPage);
    TreePage* oldNode = reinterpret_cast<TreePage*>(oldPage.data);

    int parentID = oldNode->parentPageID;

    // CASE 1: 루트 분할 (새로운 루트 생성)
    if (parentID == -1) {
        int newRootID = diskManager->AllocatePage();
        Page newRootPage;
        TreePage* newRoot = reinterpret_cast<TreePage*>(newRootPage.data);

        // 새 루트를 내부 노드(Internal Node)로 초기화
        newRoot->pageID = newRootID;
        newRoot->isLeaf = 0;
        newRoot->keyCount = 1;
        newRoot->parentPageID = -1;

        // 자식 연결: [OldChild] <Key> [NewChild]
        newRoot->keys[0] = key;
        newRoot->childrenPageIDs[0] = oldChildID;
        newRoot->childrenPageIDs[1] = newChildID;

        // 기존 자식의 부모 포인터 갱신
        oldNode->parentPageID = newRootID;
        diskManager->WritePage(oldChildID, oldPage);

        // 새 자식의 부모 포인터 갱신
        Page newPage;
        diskManager->ReadPage(newChildID, newPage);
        TreePage* newNode = reinterpret_cast<TreePage*>(newPage.data);

        newNode->parentPageID = newRootID;
        diskManager->WritePage(newChildID, newPage);

        // 트리 메타데이터(Root Page ID) 갱신
        this->rootPageID = newRootID;
        diskManager->WritePage(newRootID, newRootPage);
        return;
    }

    // CASE 2: 기존 부모 노드에 삽입
    Page parentPage;
    diskManager->ReadPage(parentID, parentPage);
    TreePage* parentNode = reinterpret_cast<TreePage*>(parentPage.data);

    // 부모 노드가 가득 찼는지 확인 (재귀적 내부 노드 분할 필요)
    if (parentNode->keyCount == ORDER) {
        splitInternal(parentID, *parentNode);
        // 부모가 쪼개진 후, 원래 넣으려던 데이터 처리를 위해 재귀 호출
        insertIntoParent(oldChildID, key, newChildID);
        return;
    }

    // 삽입 위치 탐색 및 요소 이동 (Shift)
    int i = parentNode->keyCount - 1;
    while (i >= 0 && parentNode->keys[i] > key) {
        parentNode->keys[i + 1] = parentNode->keys[i];
        // 자식 포인터 이동 (자식 인덱스는 키 인덱스 + 1)
        parentNode->childrenPageIDs[i + 2] = parentNode->childrenPageIDs[i + 1];
        i--;
    }

    // 키 삽입 및 새 자식 연결
    parentNode->keys[i + 1] = key;
    parentNode->childrenPageIDs[i + 2] = newChildID;
    parentNode->keyCount++;

    diskManager->WritePage(parentID, parentPage);

    // 새 자식 노드의 부모 포인터 갱신
    Page newPage;
    diskManager->ReadPage(newChildID, newPage);
    TreePage* newNode = reinterpret_cast<TreePage*>(newPage.data);

    newNode->parentPageID = parentID;

    diskManager->WritePage(newChildID, newPage);
}

void BPlusTree::PrintTree() {
   // 트리가 비어있는지 확인
   if (root == nullptr) {
      std::cout << "Tree is empty." << std::endl;
      return;
   }

   // BFS를 위한 큐 선언 (std::queue 사용)
   std::queue<Node*> q;
   q.push(root);

   int level = 0;
   std::cout << "=== B+ Tree Structure (BFS) ===" << std::endl;

   while (!q.empty()) {
      int levelSize = q.size(); // 현재 레벨에 있는 노드의 개수

      std::cout << "Level " << level << ": ";

      // 현재 레벨의 모든 노드를 순회
      for (int i = 0; i < levelSize; i++) {
         Node* curr = q.front();
         q.pop();

         std::cout << "[ ";
         for (int j = 0; j < curr->keyCount; j++) {
            // 1. 키(Key) 출력
            std::cout << curr->keys[j];

            // 2. 리프 노드라면 데이터(Value)도 함께 출력
            if (curr->isLeaf) {
               std::cout << "(" << curr->values[j] << ")";
            }

            // 마지막 키가 아니라면 공백 추가
            if (j < curr->keyCount - 1) {
               std::cout << " ";
            }
         }
         std::cout << " ] ";

         // 내부 노드라면 자식들을 큐에 추가
         if (!curr->isLeaf) {
            // 자식 포인터 개수는 키 개수 + 1
            for (int j = 0; j <= curr->keyCount; j++) {
               if (curr->children[j] != nullptr) {
                  q.push(curr->children[j]);
               }
            }
         }
      }
      // 레벨 변경 시 줄바꿈
      std::cout << std::endl;
      level++;
   }
   std::cout << "===============================" << std::endl;
}

/**
 * @brief 범위 검색 (Start ~ End)
 * @details 리프 노드 탐색 후 연결 리스트 순회
 */
void BPlusTree::RangeSearch(int startKey, int endKey) {
   // 1. 예외 처리: 트리가 비어있는 경우
   if (root == nullptr) return;

   Node* cursor = root;

   // 2. 시작점 탐색
   while (cursor->isLeaf == false) {
      int i = 0;
      // startKey가 들어갈 수 있는 위치를 탐색
      while (i < cursor->keyCount && startKey >= cursor->keys[i]) {
         i++;
      }
      cursor = cursor->children[i];
   }

   cout << "Range result: ";
   bool finish = false;   // 탐색 종료 플래그

   // 3. 리프 노드 연결 리스트를 따라 횡단 탐색
   while (cursor != nullptr && finish == false) {
      for (int i = 0; i < cursor->keyCount; i++) {      // 현재 노드 내의 모든 키를 순회
         // 조건 1: 키가 범위 내에 포함되는 경우 -> 출력
         if (cursor->keys[i] >= startKey && cursor->keys[i] <= endKey) {
            cout << "(" << cursor->keys[i] << ":" << cursor->values[i] << ") ";
         }
         // 조건 2: 키가 종료 키(endKey)를 넘어선 경우 -> 종료
         else if (cursor->keys[i] > endKey) {
            finish = true;
            break;
         }
      }
      // 현재 노드 탐색이 끝나면 다음 형제 노드(Sibling)로 이동
      cursor = cursor->nextLeaf;
   }
   cout << endl;
}