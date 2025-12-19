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