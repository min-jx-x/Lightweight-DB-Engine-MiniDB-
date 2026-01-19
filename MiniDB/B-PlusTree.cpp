#pragma once
#include "B-PlusTree.h"
#include <iostream>
#include <queue>
#include <string>

/**
<<<<<<< HEAD
 * @brief B+ Æ®¸®¿¡ Å°-°ª ½ÖÀ» »ğÀÔÇÕ´Ï´Ù.
 * @details ·çÆ®ºÎÅÍ Å½»öÇÏ¿© ÀûÀıÇÑ ¸®ÇÁ ³ëµå¸¦ Ã£°í, µ¥ÀÌÅÍ¸¦ »ğÀÔÇÕ´Ï´Ù.
 * ³ëµå°¡ °¡µæ Âù °æ¿ì ºĞÇÒ(Split)À» ¼öÇàÇÕ´Ï´Ù.
 * @param key »ğÀÔÇÒ Å°
 * @param value Å°¿¡ ´ëÀÀÇÏ´Â °ª
 */
void BPlusTree::Insert(int key, string value) {
    // 1. Æ®¸®°¡ ºñ¾îÀÖ´Ù¸é ·çÆ® ³ëµå »ı¼º
    if (root == nullptr) {
        root = new Node(true);
        root->keys[0] = key;
        root->values[0] = value;
        root->keyCount = 1;
        return;
    }

    // 2. µ¥ÀÌÅÍ°¡ µé¾î°¥ ¸®ÇÁ ³ëµå Å½»ö
    Node* leaf = root;
    // ·çÆ®°¡ ³»ºÎ ³ëµå(Internal Node)¶ó¸é ¸®ÇÁ±îÁö ³»·Á°¨
    while (!leaf->isLeaf) {
        int i = 0;
        // ÇöÀç ³ëµåÀÇ Å°°ªµéÀ» ºñ±³ÇÏ¿© ÀûÀıÇÑ ÀÚ½Ä ³ëµå ÀÎµ¦½º¸¦ Ã£À½
        while (i < leaf->keyCount && leaf->keys[i] <= key)
            i++;
        leaf = leaf->children[i];   // ÇØ´ç ÀÚ½Ä ³ëµå·Î Æ÷ÀÎÅÍ ÀÌµ¿
    }

    // 3. ¿¹¿Ü Ã³¸®: ³ëµå°¡ °¡µæ Ã¡À» ¶§
    if (leaf->keyCount == ORDER) {
        splitLeaf(leaf);

        // ÂÉ°³Áø ÈÄ¿¡´Â »õ·Î¿î ·çÆ®¿¡¼­ ´Ù½Ã ÀÚ¸®¸¦ Ã£±â
        // ÀÏ´Ü Áö±İÀº ´Ü¼øÇÏ°Ô ÂÉ°³´Â °Í±îÁö¸¸ Å×½ºÆ®
        Insert(key, value);
        return;
    }

    // 4. Á¤·ÄµÈ À§Ä¡¸¦ Ã£¾Æ »ğÀÔ (Insertion Sort)
    int i = leaf->keyCount - 1;
    while (i >= 0 && leaf->keys[i] > key) {
        leaf->keys[i + 1] = leaf->keys[i];
        leaf->values[i + 1] = leaf->values[i];
        i--;
    }
=======
 * @brief B+ ?¸ë¦¬????ê°??ì„ ?½ì…?©ë‹ˆ??
 * @details ë£¨íŠ¸ë¶€???ìƒ‰?˜ì—¬ ?ì ˆ??ë¦¬í”„ ?¸ë“œë¥?ì°¾ê³ , ?°ì´?°ë? ?½ì…?©ë‹ˆ??
 * ?¸ë“œê°€ ê°€??ì°?ê²½ìš° ë¶„í• (Split)???˜í–‰?©ë‹ˆ??
 * @param key ?½ì…????
 * @param value ?¤ì— ?€?‘í•˜??ê°?
 */
void BPlusTree::Insert(int key, string value) {
	// 1. ?¸ë¦¬ê°€ ë¹„ì–´?ˆë‹¤ë©?ë£¨íŠ¸ ?¸ë“œ ?ì„±
	if (root == nullptr) {
		root = new Node(true);
		root->keys[0] = key;
		root->values[0] = value;
		root->keyCount = 1;
		return;
	}

	// 2. ?°ì´?°ê? ?¤ì–´ê°?ë¦¬í”„ ?¸ë“œ ?ìƒ‰
	Node* leaf = root;
	// ë£¨íŠ¸ê°€ ?´ë? ?¸ë“œ(Internal Node)?¼ë©´ ë¦¬í”„ê¹Œì? ?´ë ¤ê°?
	while (!leaf->isLeaf) {
		int i = 0;
		// ?„ì¬ ?¸ë“œ???¤ê°’?¤ì„ ë¹„êµ?˜ì—¬ ?ì ˆ???ì‹ ?¸ë“œ ?¸ë±?¤ë? ì°¾ìŒ
		while (i < leaf->keyCount && leaf->keys[i] <= key)
			i++;
		leaf = leaf->children[i];	// ?´ë‹¹ ?ì‹ ?¸ë“œë¡??¬ì¸???´ë™
	}

	// 3. ?ˆì™¸ ì²˜ë¦¬: ?¸ë“œê°€ ê°€??ì°¼ì„ ??
	if (leaf->keyCount == ORDER) {
		splitLeaf(leaf);

		// ìª¼ê°œì§??„ì—???ˆë¡œ??ë£¨íŠ¸?ì„œ ?¤ì‹œ ?ë¦¬ë¥?ì°¾ê¸°
		// ?¼ë‹¨ ì§€ê¸ˆì? ?¨ìˆœ?˜ê²Œ ìª¼ê°œ??ê²ƒê¹Œì§€ë§??ŒìŠ¤??
		Insert(key, value);
		return;
	}

	// 4. ?•ë ¬???„ì¹˜ë¥?ì°¾ì•„ ?½ì… (Insertion Sort)
	int i = leaf->keyCount - 1;
	while (i >= 0 && leaf->keys[i] > key) {
		leaf->keys[i + 1] = leaf->keys[i];
		leaf->values[i + 1] = leaf->values[i];
		i--;
	}

	// ë¹??ë¦¬???¤ì? ê°?? ë‹¹ ë°?ê°œìˆ˜ ì¦ê?
	leaf->keys[i + 1] = key;
	leaf->values[i + 1] = value;
	leaf->keyCount++;
>>>>>>> 6e99f7939e7c6f9048fa6b1924464d45b9692b0e

    // ºó ÀÚ¸®¿¡ Å°¿Í °ª ÇÒ´ç ¹× °³¼ö Áõ°¡
    leaf->keys[i + 1] = key;
    leaf->values[i + 1] = value;
    leaf->keyCount++;

    cout << "Inserted:" << key << endl;
}

/*
* @brief B+ Tree?ì„œ ?¹ì • Key???´ë‹¹?˜ëŠ” value ê²€??
*/
<<<<<<< HEAD
string BPlusTree::Search(int key) {                         ///< Key·Î Value °Ë»ö (¾øÀ¸¸é ºó ¹®ÀÚ¿­)
    if (root == nullptr) return "";                         ///< ¿¹¿ÜÃ³¸®
    Node* cursor = root;

    ///< [Index Layer Å½»ö] ¸®ÇÁ³ëµå¿¡ µµ´ŞÇÒ ¶§±îÁö ³»·Á°¨
    while (!cursor->isLeaf) {
        int i = 0;
        while (i < cursor->keyCount && key >= cursor->keys[i]) { ///< ¾î´À ÀÚ½ÄÀ¸·Î ³»·Á°¥Áö °áÁ¤
            i++;
        }
        cursor = cursor->children[i];                           ///< °áÁ¤µÈ ÀÚ½Ä ³ëµå·Î Æ÷ÀÎÅÍ ÀÌµ¿
    }
    ///< [Data Layer Å½»ö] ½ÇÁ¦ µ¥ÀÌÅÍ°¡ ÀÖ´Â ¸®ÇÁ ³ëµå µµÂø
    for (int i = 0; i < cursor->keyCount; i++) {                ///< ³ëµå ³»ÀÇ Å°µéÀ»  ¼øÈ¸ÇÏ¸ç ÀÏÄ¡ÇÏ´Â °ªÀÌ ÀÖ´ÂÁö È®ÀÎ.
        if (cursor->keys[i] == key)
            return cursor->values[i];
    }
    return "";
=======
string BPlusTree::Search(int key) {                         ///< Keyë¡?Value ê²€??(?†ìœ¼ë©?ë¹?ë¬¸ì??
	if (root == nullptr) return "";                         ///< ?ˆì™¸ì²˜ë¦¬
	Node* cursor = root;

	///< [Index Layer ?ìƒ‰] ë¦¬í”„?¸ë“œ???„ë‹¬???Œê¹Œì§€ ?´ë ¤ê°?
	while (!cursor->isLeaf) {
		int i = 0;
		while (i < cursor->keyCount && key >= cursor->keys[i]) { ///< ?´ëŠ ?ì‹?¼ë¡œ ?´ë ¤ê°ˆì? ê²°ì •
			i++;
		}
		cursor = cursor->children[i];                           ///< ê²°ì •???ì‹ ?¸ë“œë¡??¬ì¸???´ë™
	}
	///< [Data Layer ?ìƒ‰] ?¤ì œ ?°ì´?°ê? ?ˆëŠ” ë¦¬í”„ ?¸ë“œ ?„ì°©
	for (int i = 0; i < cursor->keyCount; i++) {                ///< ?¸ë“œ ?´ì˜ ?¤ë“¤?? ?œíšŒ?˜ë©° ?¼ì¹˜?˜ëŠ” ê°’ì´ ?ˆëŠ”ì§€ ?•ì¸.
		if (cursor->keys[i] == key)
			return cursor->values[i];
	}
	return "";
>>>>>>> 6e99f7939e7c6f9048fa6b1924464d45b9692b0e
}

/*
* @brief ë¦¬í”„ ?¸ë“œê°€ ê½?ì°¼ì„ ??ë°˜ìœ¼ë¡?ë¶„í• ?˜ê³  ë¶€ëª¨ì—ê²??¹ê²©(Promote) ?”ì²­
*/
void BPlusTree::splitLeaf(Node* leaf) {
<<<<<<< HEAD
    // 1. »õ·Î¿î ÇüÁ¦ ³ëµå »ı¼º
    Node* newLeaf = new Node(true);

    // 2. ºĞÇÒ ±âÁØÁ¡ ¼³Á¤ (Áß°£ ÁöÁ¡)
    int splitIndex = (ORDER + 1) / 2;

    // 3. ±âÁ¸ ³ëµåÀÇ ¿À¸¥ÂÊ Àı¹İÀ» »õ ³ëµå·Î º¹»ç
    int j = 0;
    for (int i = splitIndex; i < ORDER; i++) {
        newLeaf->keys[j] = leaf->keys[i];
        newLeaf->values[j] = leaf->values[i];
        j++;
    }

    // 4. °¢ ³ëµåÀÇ Å° °³¼ö(KeyCount) °»½Å
    leaf->keyCount = splitIndex;            // ±âÁ¸ ³ëµå´Â Àı¹İÀ¸·Î ÁÙ¾îµê
    newLeaf->keyCount = ORDER - splitIndex; // ³ª¸ÓÁö´Â »õ ³ëµå°¡ °¡Áü

    // 5. ¸®ÇÁ ³ëµå ¿¬°á ¸®½ºÆ® ±¸Á¶ À¯Áö
    newLeaf->nextLeaf = leaf->nextLeaf;     // »õ ³ëµå°¡ ±âÁ¸ ³ëµåÀÇ µŞºÎºĞÀ» °¡¸®Å´
    leaf->nextLeaf = newLeaf;               // ±âÁ¸ ³ëµå°¡ »õ ³ëµå¸¦ °¡¸®Å´

    // 6. ºÎ¸ğ ³ëµå¿¡ »õ Å° µî·Ï (½ÂÁø)
    insertIntoParent(leaf, newLeaf->keys[0], newLeaf);
=======
	// 1. ?ˆë¡œ???•ì œ ?¸ë“œ ?ì„±
	Node* newLeaf = new Node(true);

	// 2. ë¶„í•  ê¸°ì????¤ì • (ì¤‘ê°„ ì§€??
	int splitIndex = (ORDER + 1) / 2;

	// 3. ê¸°ì¡´ ?¸ë“œ???¤ë¥¸ìª??ˆë°˜?????¸ë“œë¡?ë³µì‚¬
	int j = 0;
	for (int i = splitIndex; i < ORDER; i++) {
		newLeaf->keys[j] = leaf->keys[i];
		newLeaf->values[j] = leaf->values[i];
		j++;
	}

	// 4. ê°??¸ë“œ????ê°œìˆ˜(KeyCount) ê°±ì‹ 
	leaf->keyCount = splitIndex;            // ê¸°ì¡´ ?¸ë“œ???ˆë°˜?¼ë¡œ ì¤„ì–´??
	newLeaf->keyCount = ORDER - splitIndex; // ?˜ë¨¸ì§€?????¸ë“œê°€ ê°€ì§?

	// 5. ë¦¬í”„ ?¸ë“œ ?°ê²° ë¦¬ìŠ¤??êµ¬ì¡° ? ì?
	newLeaf->nextLeaf = leaf->nextLeaf;     // ???¸ë“œê°€ ê¸°ì¡´ ?¸ë“œ???·ë?ë¶„ì„ ê°€ë¦¬í‚´
	leaf->nextLeaf = newLeaf;               // ê¸°ì¡´ ?¸ë“œê°€ ???¸ë“œë¥?ê°€ë¦¬í‚´

	// 6. ë¶€ëª??¸ë“œ???????±ë¡ (?¹ì§„)
	insertIntoParent(leaf, newLeaf->keys[0], newLeaf);
>>>>>>> 6e99f7939e7c6f9048fa6b1924464d45b9692b0e
}

/*
* @brief B+ Tree?ì„œ ?¸ë¦¬??ë£¨íŠ¸ê°€ ë¶„í• (Split)?˜ì–´ ?ˆë¡œ??ë£¨íŠ¸ë¥??ì„±?˜ëŠ” ë¡œì§
*/
void BPlusTree::insertIntoParent(Node* left, int key, Node* right) {
<<<<<<< HEAD
    // 1. [Root Split] ºÎ¸ğ°¡ ¾ø´Â °æ¿ì (Left°¡ °ğ RootÀÎ °æ¿ì)
    // Æ®¸®ÀÇ ³ôÀÌ(Height)°¡ 1 Áõ°¡ÇÏ¸ç »õ·Î¿î ·çÆ®°¡ »ı¼ºµÊ
    if (root == left) {
        Node* newRoot = new Node(false);
        newRoot->keys[0] = key;             ///< »õ ·çÆ®ÀÇ Ã¹ ¹øÂ° Å° ¼³Á¤
        newRoot->children[0] = left;        ///< ¿ŞÂÊ ÀÚ½Ä ¿¬°á
        newRoot->children[1] = right;       ///< ¿À¸¥ÂÊ ÀÚ½Ä ¿¬°á
        newRoot->keyCount = 1;

        left->parent = newRoot;             ///< ÀÚ½ÄµéÀÇ ºÎ¸ğ Æ÷ÀÎÅÍ °»½Å
        right->parent = newRoot;
        root = newRoot;                     ///< Àü¿ª ·çÆ® Æ÷ÀÎÅÍ °»½Å
        return;
    }

    // 2. [General Insert] ÀÌ¹Ì ºÎ¸ğ°¡ Á¸ÀçÇÏ´Â °æ¿ì
    // ºÎ¸ğ ³ëµå¿¡ ÀÚ¸®°¡ ÀÖ´ÂÁö È®ÀÎÇÏ°í Á¤·ÄÇÏ¿© »ğÀÔ
    else {
        Node* parent = left->parent;

        // µé¾î°¥ À§Ä¡ Ã£±â ¹× ÀÚ¸® ¸¸µé±â (Shift Right)
        // Å°¿Í ÀÚ½Ä Æ÷ÀÎÅÍ¸¦ ÇÑ Ä­¾¿ µÚ·Î ¹Ğ¾î °ø°£ È®º¸
        int i = parent->keyCount - 1;
        while (i >= 0 && parent->keys[i] > key) {
            parent->keys[i + 1] = parent->keys[i];
            parent->children[i + 2] = parent->children[i + 1]; // Æ÷ÀÎÅÍµµ °°ÀÌ ÀÌµ¿
            i--;
        }

        // ºó ÀÚ¸®¿¡ Å°¿Í Æ÷ÀÎÅÍ »ğÀÔ
        parent->keys[i + 1] = key;
        parent->children[i + 2] = right;
        parent->keyCount++;

        right->parent = parent; // »õ ÀÚ½ÄÀÇ ºÎ¸ğ ¿¬°á

        // 3. [Overflow Handling] ºÎ¸ğ ³ëµåµµ ²Ë Ã¡À» °æ¿ì
        // ³»ºÎ ³ëµå ºĞÇÒ(Split Internal)À» Àç±ÍÀûÀ¸·Î È£Ãâ
        if (parent->keyCount == ORDER) {
            splitInternal(parent);
        }
    }
}

/*
* @brief ³»ºÎ ³ëµå(Internal Node)°¡ ²Ë Ã¡À» ¶§ ºĞÇÒÇÏ°í Áß°£ Å°¸¦ ºÎ¸ğ·Î ¿Ã¸²
*/
void BPlusTree::splitInternal(Node* parent) {
    // 1. »õ·Î¿î ³»ºÎ ³ëµå »ı¼º (Leaf ¾Æ´Ô)
    Node* newInternal = new Node(false);

    // 2. ºĞÇÒ ±âÁØÁ¡ ¼³Á¤
    int splitIndex = (ORDER + 1) / 2;

    // 3. ºÎ¸ğ·Î ¿Ã¶ó°¥ Áß°£ Å° ¹é¾÷ (³»ºÎ ³ëµå¿¡¼­´Â ÀÌ °ªÀÌ »ç¶óÁü)
    int backupkey = parent->keys[splitIndex];

    // 4. ±âÁ¸ ³ëµåÀÇ ¿À¸¥ÂÊ Àı¹İÀ» »õ ³ëµå·Î ÀÌ»ç
    int j = 0;
    for (int i = splitIndex + 1; i < ORDER; i++) {
        newInternal->keys[j] = parent->keys[i];
        newInternal->children[j] = parent->children[i]; // Å°¿Í ¿ŞÂÊ ÀÚ½Ä Æ÷ÀÎÅÍ¸¦ ½ÖÀ¸·Î ÀÌµ¿
        j++;
    }
    // ·çÇÁ¿¡¼­ Ã³¸®µÇÁö ¾ÊÀº ¸¶Áö¸· ÀÚ½Ä Æ÷ÀÎÅÍ ÀÌµ¿
    newInternal->children[j] = parent->children[ORDER];

    // 5. Å° °³¼ö(KeyCount) °»½Å
    parent->keyCount = splitIndex;
    newInternal->keyCount = ORDER - splitIndex - 1;

    // 6. ÀÌ»ç °£ ÀÚ½Ä ³ëµåµéÀÇ ºÎ¸ğ Æ÷ÀÎÅÍ¸¦ »õ ³ëµå·Î º¯°æ
    for (int i = 0; i <= newInternal->keyCount; i++) {
        Node* child = newInternal->children[i];
        child->parent = newInternal;
    }

    // 7. ºÎ¸ğ ³ëµå¿¡ Áß°£ Å° µî·Ï (Àç±Í È£Ãâ)
    insertIntoParent(parent, backupkey, newInternal);
}

void BPlusTree::PrintTree() {
    // Æ®¸®°¡ ºñ¾îÀÖ´ÂÁö È®ÀÎ
    if (root == nullptr) {
        std::cout << "Tree is empty." << std::endl;
        return;
    }

    // BFS¸¦ À§ÇÑ Å¥ ¼±¾ğ (std::queue »ç¿ë)
    std::queue<Node*> q;
    q.push(root);
=======
	// 1. [Root Split] ë¶€ëª¨ê? ?†ëŠ” ê²½ìš° (Leftê°€ ê³?Root??ê²½ìš°)
	// ?¸ë¦¬???’ì´(Height)ê°€ 1 ì¦ê??˜ë©° ?ˆë¡œ??ë£¨íŠ¸ê°€ ?ì„±??
	if (root == left) {
		Node* newRoot = new Node(false);
		newRoot->keys[0] = key;             ///< ??ë£¨íŠ¸??ì²?ë²ˆì§¸ ???¤ì •
		newRoot->children[0] = left;        ///< ?¼ìª½ ?ì‹ ?°ê²°
		newRoot->children[1] = right;       ///< ?¤ë¥¸ìª??ì‹ ?°ê²°
		newRoot->keyCount = 1;

		left->parent = newRoot;             ///< ?ì‹?¤ì˜ ë¶€ëª??¬ì¸??ê°±ì‹ 
		right->parent = newRoot;
		root = newRoot;                     ///< ?„ì—­ ë£¨íŠ¸ ?¬ì¸??ê°±ì‹ 
		return;
	}

	// 2. [General Insert] ?´ë? ë¶€ëª¨ê? ì¡´ì¬?˜ëŠ” ê²½ìš°
	// ë¶€ëª??¸ë“œ???ë¦¬ê°€ ?ˆëŠ”ì§€ ?•ì¸?˜ê³  ?•ë ¬?˜ì—¬ ?½ì…
	else {
		Node* parent = left->parent;

		// ?¤ì–´ê°??„ì¹˜ ì°¾ê¸° ë°??ë¦¬ ë§Œë“¤ê¸?(Shift Right)
		// ?¤ì? ?ì‹ ?¬ì¸?°ë? ??ì¹¸ì”© ?¤ë¡œ ë°€??ê³µê°„ ?•ë³´
		int i = parent->keyCount - 1;
		while (i >= 0 && parent->keys[i] > key) {
			parent->keys[i + 1] = parent->keys[i];
			parent->children[i + 2] = parent->children[i + 1]; // ?¬ì¸?°ë„ ê°™ì´ ?´ë™
			i--;
		}

		// ë¹??ë¦¬???¤ì? ?¬ì¸???½ì…
		parent->keys[i + 1] = key;
		parent->children[i + 2] = right;
		parent->keyCount++;

		right->parent = parent; // ???ì‹??ë¶€ëª??°ê²°

		// 3. [Overflow Handling] ë¶€ëª??¸ë“œ??ê½?ì°¼ì„ ê²½ìš°
		// ?´ë? ?¸ë“œ ë¶„í• (Split Internal)???¬ê??ìœ¼ë¡??¸ì¶œ
		if (parent->keyCount == ORDER) {
			splitInternal(parent);
		}
	}
}

/*
* @brief ?´ë? ?¸ë“œ(Internal Node)ê°€ ê½?ì°¼ì„ ??ë¶„í• ?˜ê³  ì¤‘ê°„ ?¤ë? ë¶€ëª¨ë¡œ ?¬ë¦¼
*/
void BPlusTree::splitInternal(Node* parent) {
	// 1. ?ˆë¡œ???´ë? ?¸ë“œ ?ì„± (Leaf ?„ë‹˜)
	Node* newInternal = new Node(false);

	// 2. ë¶„í•  ê¸°ì????¤ì •
	int splitIndex = (ORDER + 1) / 2;

	// 3. ë¶€ëª¨ë¡œ ?¬ë¼ê°?ì¤‘ê°„ ??ë°±ì—… (?´ë? ?¸ë“œ?ì„œ????ê°’ì´ ?¬ë¼ì§?
	int backupkey = parent->keys[splitIndex];

	// 4. ê¸°ì¡´ ?¸ë“œ???¤ë¥¸ìª??ˆë°˜?????¸ë“œë¡??´ì‚¬
	int j = 0;
	for (int i = splitIndex + 1; i < ORDER; i++) {
		newInternal->keys[j] = parent->keys[i];
		newInternal->children[j] = parent->children[i]; // ?¤ì? ?¼ìª½ ?ì‹ ?¬ì¸?°ë? ?ìœ¼ë¡??´ë™
		j++;
	}
	// ë£¨í”„?ì„œ ì²˜ë¦¬?˜ì? ?Šì? ë§ˆì?ë§??ì‹ ?¬ì¸???´ë™
	newInternal->children[j] = parent->children[ORDER];

	// 5. ??ê°œìˆ˜(KeyCount) ê°±ì‹ 
	parent->keyCount = splitIndex;
	newInternal->keyCount = ORDER - splitIndex - 1;

	// 6. ?´ì‚¬ ê°??ì‹ ?¸ë“œ?¤ì˜ ë¶€ëª??¬ì¸?°ë? ???¸ë“œë¡?ë³€ê²?
	for (int i = 0; i <= newInternal->keyCount; i++) {
		Node* child = newInternal->children[i];
		child->parent = newInternal;
	}

	// 7. ë¶€ëª??¸ë“œ??ì¤‘ê°„ ???±ë¡ (?¬ê? ?¸ì¶œ)
	insertIntoParent(parent, backupkey, newInternal);
}

void BPlusTree::PrintTree() {
	// ?¸ë¦¬ê°€ ë¹„ì–´?ˆëŠ”ì§€ ?•ì¸
	if (root == nullptr) {
		std::cout << "Tree is empty." << std::endl;
		return;
	}

	// BFSë¥??„í•œ ??? ì–¸ (std::queue ?¬ìš©)
	std::queue<Node*> q;
	q.push(root);
>>>>>>> 6e99f7939e7c6f9048fa6b1924464d45b9692b0e

    int level = 0;
    std::cout << "=== B+ Tree Structure (BFS) ===" << std::endl;

<<<<<<< HEAD
    while (!q.empty()) {
        int levelSize = q.size(); // ÇöÀç ·¹º§¿¡ ÀÖ´Â ³ëµåÀÇ °³¼ö
=======
	while (!q.empty()) {
		int levelSize = q.size(); // ?„ì¬ ?ˆë²¨???ˆëŠ” ?¸ë“œ??ê°œìˆ˜
>>>>>>> 6e99f7939e7c6f9048fa6b1924464d45b9692b0e

        std::cout << "Level " << level << ": ";

<<<<<<< HEAD
        // ÇöÀç ·¹º§ÀÇ ¸ğµç ³ëµå¸¦ ¼øÈ¸
        for (int i = 0; i < levelSize; i++) {
            Node* curr = q.front();
            q.pop();

            std::cout << "[ ";
            for (int j = 0; j < curr->keyCount; j++) {
                // 1. Å°(Key) Ãâ·Â
                std::cout << curr->keys[j];

                // 2. ¸®ÇÁ ³ëµå¶ó¸é µ¥ÀÌÅÍ(Value)µµ ÇÔ²² Ãâ·Â
                if (curr->isLeaf) {
                    std::cout << "(" << curr->values[j] << ")";
                }

                // ¸¶Áö¸· Å°°¡ ¾Æ´Ï¶ó¸é °ø¹é Ãß°¡
                if (j < curr->keyCount - 1) {
                    std::cout << " ";
                }
            }
            std::cout << " ] ";

            // ³»ºÎ ³ëµå¶ó¸é ÀÚ½ÄµéÀ» Å¥¿¡ Ãß°¡
            if (!curr->isLeaf) {
                // ÀÚ½Ä Æ÷ÀÎÅÍ °³¼ö´Â Å° °³¼ö + 1
                for (int j = 0; j <= curr->keyCount; j++) {
                    if (curr->children[j] != nullptr) {
                        q.push(curr->children[j]);
                    }
                }
            }
        }
        // ·¹º§ º¯°æ ½Ã ÁÙ¹Ù²Ş
        std::cout << std::endl;
        level++;
    }
    std::cout << "===============================" << std::endl;
=======
		// ?„ì¬ ?ˆë²¨??ëª¨ë“  ?¸ë“œë¥??œíšŒ
		for (int i = 0; i < levelSize; i++) {
			Node* curr = q.front();
			q.pop();

			std::cout << "[ ";
			for (int j = 0; j < curr->keyCount; j++) {
				// 1. ??Key) ì¶œë ¥
				std::cout << curr->keys[j];

				// 2. ë¦¬í”„ ?¸ë“œ?¼ë©´ ?°ì´??Value)???¨ê»˜ ì¶œë ¥
				if (curr->isLeaf) {
					std::cout << "(" << curr->values[j] << ")";
				}

				// ë§ˆì?ë§??¤ê? ?„ë‹ˆ?¼ë©´ ê³µë°± ì¶”ê?
				if (j < curr->keyCount - 1) {
					std::cout << " ";
				}
			}
			std::cout << " ] ";

			// ?´ë? ?¸ë“œ?¼ë©´ ?ì‹?¤ì„ ?ì— ì¶”ê?
			if (!curr->isLeaf) {
				// ?ì‹ ?¬ì¸??ê°œìˆ˜????ê°œìˆ˜ + 1
				for (int j = 0; j <= curr->keyCount; j++) {
					if (curr->children[j] != nullptr) {
						q.push(curr->children[j]);
					}
				}
			}
		}
		// ?ˆë²¨ ë³€ê²???ì¤„ë°”ê¿?
		std::cout << std::endl;
		level++;
	}
	std::cout << "===============================" << std::endl;
>>>>>>> 6e99f7939e7c6f9048fa6b1924464d45b9692b0e
}