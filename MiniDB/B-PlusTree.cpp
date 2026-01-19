#pragma once
#include "B-PlusTree.h"
#include <iostream>
#include <queue>
#include <string>

/**
 * @brief B+ 트리에 키-값 쌍을 삽입합니다.
 * @details 루트부터 탐색하여 적절한 리프 노드를 찾고, 데이터를 삽입합니다.
 * 노드가 가득 찬 경우 분할(Split)을 수행합니다.
 * @param key 삽입할 키
 * @param value 키에 대응하는 값
 */
void BPlusTree::Insert(int key, string value) {
	// 1. 트리가 비어있다면 루트 노드 생성
	if (root == nullptr) {
		root = new Node(true);
		root->keys[0] = key;
		root->values[0] = value;
		root->keyCount = 1;
		return;
	}

	// 2. 데이터가 들어갈 리프 노드 탐색
	Node* leaf = root;
	// 루트가 내부 노드(Internal Node)라면 리프까지 내려감
	while (!leaf->isLeaf) {
		int i = 0;
		// 현재 노드의 키값들을 비교하여 적절한 자식 노드 인덱스를 찾음
		while (i < leaf->keyCount && leaf->keys[i] <= key)
			i++;
		leaf = leaf->children[i];	// 해당 자식 노드로 포인터 이동
	}

	// 3. 예외 처리: 노드가 가득 찼을 때
	if (leaf->keyCount == ORDER) {
		splitLeaf(leaf);

		// 쪼개진 후에는 새로운 루트에서 다시 자리를 찾기
		// 일단 지금은 단순하게 쪼개는 것까지만 테스트
		Insert(key, value);
		return;
	}

	// 4. 정렬된 위치를 찾아 삽입 (Insertion Sort)
	int i = leaf->keyCount - 1;
	while (i >= 0 && leaf->keys[i] > key) {
		leaf->keys[i + 1] = leaf->keys[i];
		leaf->values[i + 1] = leaf->values[i];
		i--;
	}

	// 빈 자리에 키와 값 할당 및 개수 증가
	leaf->keys[i + 1] = key;
	leaf->values[i + 1] = value;
	leaf->keyCount++;

	cout << "Inserted:" << key << endl;
}

/*
* @brief B+ Tree에서 특정 Key에 해당하는 value 검색
*/
string BPlusTree::Search(int key) {                         ///< Key로 Value 검색 (없으면 빈 문자열)
	if (root == nullptr) return "";                         ///< 예외처리
	Node* cursor = root;

	///< [Index Layer 탐색] 리프노드에 도달할 때까지 내려감
	while (!cursor->isLeaf) {
		int i = 0;
		while (i < cursor->keyCount && key >= cursor->keys[i]) { ///< 어느 자식으로 내려갈지 결정
			i++;
		}
		cursor = cursor->children[i];                           ///< 결정된 자식 노드로 포인터 이동
	}
	///< [Data Layer 탐색] 실제 데이터가 있는 리프 노드 도착
	for (int i = 0; i < cursor->keyCount; i++) {                ///< 노드 내의 키들을  순회하며 일치하는 값이 있는지 확인.
		if (cursor->keys[i] == key)
			return cursor->values[i];
	}
	return "";
}

/*
* @brief 리프 노드가 꽉 찼을 때 반으로 분할하고 부모에게 승격(Promote) 요청
*/
void BPlusTree::splitLeaf(Node* leaf) {
	// 1. 새로운 형제 노드 생성
	Node* newLeaf = new Node(true);

	// 2. 분할 기준점 설정 (중간 지점)
	int splitIndex = (ORDER + 1) / 2;

	// 3. 기존 노드의 오른쪽 절반을 새 노드로 복사
	int j = 0;
	for (int i = splitIndex; i < ORDER; i++) {
		newLeaf->keys[j] = leaf->keys[i];
		newLeaf->values[j] = leaf->values[i];
		j++;
	}

	// 4. 각 노드의 키 개수(KeyCount) 갱신
	leaf->keyCount = splitIndex;            // 기존 노드는 절반으로 줄어듦
	newLeaf->keyCount = ORDER - splitIndex; // 나머지는 새 노드가 가짐

	// 5. 리프 노드 연결 리스트 구조 유지
	newLeaf->nextLeaf = leaf->nextLeaf;     // 새 노드가 기존 노드의 뒷부분을 가리킴
	leaf->nextLeaf = newLeaf;               // 기존 노드가 새 노드를 가리킴

	// 6. 부모 노드에 새 키 등록 (승진)
	insertIntoParent(leaf, newLeaf->keys[0], newLeaf);
}

/*
* @brief B+ Tree에서 트리의 루트가 분할(Split)되어 새로운 루트를 생성하는 로직
*/
void BPlusTree::insertIntoParent(Node* left, int key, Node* right) {
	// 1. [Root Split] 부모가 없는 경우 (Left가 곧 Root인 경우)
	// 트리의 높이(Height)가 1 증가하며 새로운 루트가 생성됨
	if (root == left) {
		Node* newRoot = new Node(false);
		newRoot->keys[0] = key;             ///< 새 루트의 첫 번째 키 설정
		newRoot->children[0] = left;        ///< 왼쪽 자식 연결
		newRoot->children[1] = right;       ///< 오른쪽 자식 연결
		newRoot->keyCount = 1;

		left->parent = newRoot;             ///< 자식들의 부모 포인터 갱신
		right->parent = newRoot;
		root = newRoot;                     ///< 전역 루트 포인터 갱신
		return;
	}

	// 2. [General Insert] 이미 부모가 존재하는 경우
	// 부모 노드에 자리가 있는지 확인하고 정렬하여 삽입
	else {
		Node* parent = left->parent;

		// 들어갈 위치 찾기 및 자리 만들기 (Shift Right)
		// 키와 자식 포인터를 한 칸씩 뒤로 밀어 공간 확보
		int i = parent->keyCount - 1;
		while (i >= 0 && parent->keys[i] > key) {
			parent->keys[i + 1] = parent->keys[i];
			parent->children[i + 2] = parent->children[i + 1]; // 포인터도 같이 이동
			i--;
		}

		// 빈 자리에 키와 포인터 삽입
		parent->keys[i + 1] = key;
		parent->children[i + 2] = right;
		parent->keyCount++;

		right->parent = parent; // 새 자식의 부모 연결

		// 3. [Overflow Handling] 부모 노드도 꽉 찼을 경우
		// 내부 노드 분할(Split Internal)을 재귀적으로 호출
		if (parent->keyCount == ORDER) {
			splitInternal(parent);
		}
	}
}

/*
* @brief 내부 노드(Internal Node)가 꽉 찼을 때 분할하고 중간 키를 부모로 올림
*/
void BPlusTree::splitInternal(Node* parent) {
	// 1. 새로운 내부 노드 생성 (Leaf 아님)
	Node* newInternal = new Node(false);

	// 2. 분할 기준점 설정
	int splitIndex = (ORDER + 1) / 2;

	// 3. 부모로 올라갈 중간 키 백업 (내부 노드에서는 이 값이 사라짐)
	int backupkey = parent->keys[splitIndex];

	// 4. 기존 노드의 오른쪽 절반을 새 노드로 이사
	int j = 0;
	for (int i = splitIndex + 1; i < ORDER; i++) {
		newInternal->keys[j] = parent->keys[i];
		newInternal->children[j] = parent->children[i]; // 키와 왼쪽 자식 포인터를 쌍으로 이동
		j++;
	}
	// 루프에서 처리되지 않은 마지막 자식 포인터 이동
	newInternal->children[j] = parent->children[ORDER];

	// 5. 키 개수(KeyCount) 갱신
	parent->keyCount = splitIndex;
	newInternal->keyCount = ORDER - splitIndex - 1;

	// 6. 이사 간 자식 노드들의 부모 포인터를 새 노드로 변경
	for (int i = 0; i <= newInternal->keyCount; i++) {
		Node* child = newInternal->children[i];
		child->parent = newInternal;
	}

	// 7. 부모 노드에 중간 키 등록 (재귀 호출)
	insertIntoParent(parent, backupkey, newInternal);
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
 * @brief 키에 해당하는 값 검색
 * @return 찾은 값 (실패 시 빈 문자열)
 */
string BPlusTree::Search(int key) {
	// 1. 예외 처리: 트리가 비어있는 경우
	if (root == nullptr) return "";

	Node* cursor = root;

	// 2. 리프 노드까지 하향 탐색
	while (cursor->isLeaf == false) {
		int i = 0;
		// 현재 노드의 키들과 비교하여, 찾으려는 키가 포함된 자식 노드
		while (i < cursor->keyCount && key >= cursor->keys[i]) {
			i++;
		}
		cursor = cursor->children[i];	// 해당 자식 노드로 포인터 이동
	}

	// 3. 키 존재 여부 확인 및 반환
	for (int i = 0; i < cursor->keyCount; i++) {
		// 키를 찾았다면 대응하는 값을 반환
		if (cursor->keys[i] == key) {
			return cursor->values[i];
		}
	}
	// 4. 리프 노드를 다 찾아봤지만 키가 없는 경우
	return "";
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
	bool finish = false;	// 탐색 종료 플래그

	// 3. 리프 노드 연결 리스트를 따라 횡단 탐색
	while (cursor != nullptr && finish == false) {
		for (int i = 0; i < cursor->keyCount; i++) {		// 현재 노드 내의 모든 키를 순회
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
