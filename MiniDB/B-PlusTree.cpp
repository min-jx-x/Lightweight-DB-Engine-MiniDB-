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

	// 예외 처리: 노드가 가득 찼을 때(Split 로직 추가 예정)
	if (leaf->keyCount == ORDER) {
		cout << "Node is full" << endl;
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
* @brief B+ Tree에서 트리의 루트가 분할(Split)되어 새로운 루트를 생성하는 로직
*/
void BPlusTree::insert_into_new_root(Node* left, int key, Node* right) {
	Node* newRoot = new Node(false);
	newRoot->keys[0] = key;										///< newRoot의 첫 번째 키(keys[0])에 key 저장
	newRoot->children[0] = left;								///< newRoot의 첫 번째 자식(children[0])은 left
	newRoot->children[1] = right;								///< 두 번째 자식(children[1])은 right로 연결
	left->parent = newRoot;										///< left와 right의 부모로 newRoot를 가리키게 변경
	right->parent = newRoot;
	root = newRoot;												///< 트리의 root를 newRoot로 변경
}