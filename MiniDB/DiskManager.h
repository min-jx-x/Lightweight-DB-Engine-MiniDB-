#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

// 페이지 크기는 보통 OS 페이지 크기와 맞춘 4KB (4096 bytes)를 사용
const int PAGE_SIZE = 4096;

/**
 * @brief 페이지 구조체
 * 디스크에 저장되는 데이터의 최소 단위입니다.
 * 단순히 4096 바이트짜리 char 배열을 감싸고 있습니다.
 */
struct Page {
	char data[PAGE_SIZE];

	// 생성자: 페이지를 생성할 때 데이터를 0으로 초기화 (쓰레기 값 방지)
	Page() {
		std::memset(data, 0, PAGE_SIZE);
	}
};

/**
 * @brief 디스크 매니저 클래스
 * 파일을 열어서 페이지 단위로 읽고 쓰는 역할을 담당합니다.
 */
class DiskManager {
private:
    std::fstream dbFile;    // 파일 입출력 객체
    std::string fileName;   // DB 파일 이름
    int nextPageId;         // 다음에 할당될 페이지 ID
public:
    // 생성자: 파일 이름을 받아 파일을 열기
    DiskManager(const std::string& dbFileName);
    ~DiskManager();

    // 특정 페이지 ID의 데이터를 파일에서 읽기
    void ReadPage(int pageId, Page& page);

    // 특정 페이지 ID 위치에 데이터를 쓰기
    void WritePage(int pageId, const Page& page);

    // 새로운 페이지를 할당하고 그 페이지 ID를 반환 (파일 크기가 늘어남)
    int AllocatePage();

    // 현재 파일에 총 몇 개의 페이지가 있는지 확인
    int GetNumPages();
};