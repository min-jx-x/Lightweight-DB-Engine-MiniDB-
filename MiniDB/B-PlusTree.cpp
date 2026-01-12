#pragma once
#include "B-PlusTree.h"
#include <iostream>

/**
 * @brief B+ 트리에 키-값 쌍을 삽입합니다.
 * @note 현재 노드 분할(Split) 로직은 미구현 상태로, 노드가 꽉 차면 삽입이 거부됩니다.
 * @param key 삽입할 키
 * @param value 키에 대응하는 값
 */
void BPlusTree::Insert(int key, string value) {
	if (root == nullptr) {
		root = new Node(true);
		root->keys[0] = key;
		root->values[0] = value;
		root->keyCount = 1;
		return;
	}

	// TODO: 루트가 리프가 아닐 경우 탐색 로직 추가 필요
	Node* leaf = root;

	// 예외 처리: 노드가 가득 찼을 때
	if (leaf->keyCount == ORDER) {
		splitLeaf(leaf);
		// 쪼개진 후에는 새로운 루트에서 다시 자리를 찾기
		// 일단 지금은 단순하게 쪼개는 것까지만 테스트
		Insert(key, value);
		return;
	}

	// 정렬된 위치를 찾아 삽입 (Insertion Sort)
	int i = leaf->keyCount - 1;
	while (i >= 0 && leaf->keys[i] > key) {
		leaf->keys[i + 1] = leaf->keys[i];
		leaf->values[i + 1] = leaf->values[i];
		i--;
	}

	leaf->keys[i + 1] = key;
	leaf->values[i + 1] = value;
	leaf->keyCount++;

	cout << "Inserted:" << key << endl;
}

/*
* @brief B+ Tree에서 특정 Key에 해당하는 value 검색
*/
string BPlusTree::Search(int key) {							  ///< Key로 Value 검색 (없으면 빈 문자열)
	if (root == nullptr) return "";							  ///< 예외처리
	Node* cursor = root;

	///< [Index Layer 탐색] 리프노드에 도달할 때까지 내려감
	while (!cursor->isLeaf) {
		int i = 0;
		while (i < cursor->keyCount && key >= cursor->keys[i]) {  ///< 어느 자식으로 내려갈지 결정
			i++;
		}
		cursor = cursor->children[i];							  ///< 결정된 자식 노드로 포인터 이동
	}
	///< [Data Layer 탐색] 실제 데이터가 있는 리프 노드 도착
	for (int i = 0; i < cursor->keyCount; i++) {				  ///< 노드 내의 키들을  순회하며 일치하는 값이 있는지 확인.
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