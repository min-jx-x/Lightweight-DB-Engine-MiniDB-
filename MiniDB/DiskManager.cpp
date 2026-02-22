#pragma once
#include "DiskManager.h"
#include <stdexcept>

/**
 * @brief 생성자: DB 파일을 엽니다.
 * @details 파일이 존재하지 않으면 새로 생성하고,
 * 바이너리 읽기/쓰기 모드로 열어 입출력을 준비합니다.
 * @param dbFileName 열거나 생성할 데이터베이스 파일의 이름
 */
DiskManager::DiskManager(const std::string& dbFileName) : fileName(dbFileName) {
    nextPageId = 0;
    // 1. 읽기/쓰기/바이너리 모드로 파일 열기 시도
    // std::ios::in (읽기), std::ios::out (쓰기), std::ios::binary (바이너리 모드)
    dbFile.open(fileName, std::ios::in | std::ios::out | std::ios::binary);

    // 2. 파일이 없으면 새로 생성 (기존에 없던 파일이면 open이 실패하므로)
    if (!dbFile.is_open()) {
        // 출력 모드(out)로만 열면 파일이 없는 경우 새로 생성됨
        dbFile.open(fileName, std::ios::out | std::ios::binary);
        dbFile.close(); // 생성만 하고 일단 닫음

        // 다시 읽기/쓰기 모드로 열기
        dbFile.open(fileName, std::ios::in | std::ios::out | std::ios::binary);

        // 그래도 열리지 않으면 치명적 오류
        if (!dbFile.is_open()) {
            throw std::runtime_error("Failed to open DB File.");
        }
    }
}

/**
 * @brief 소멸자: 파일을 안전하게 닫습니다.
 * 객체가 소멸될 때 열려있는 파일 리소스를 반환합니다.
 */
DiskManager::~DiskManager() {
    if (dbFile.is_open()) {
        dbFile.close();
    }
}

/**
 * @brief 페이지 읽기: 특정 페이지 ID의 데이터를 파일에서 읽어옵니다.
 * @param pageId 읽을 페이지 번호
 * @param page 데이터를 저장할 페이지 객체 (참조)
 */
void DiskManager::ReadPage(int pageId, Page& page) {
    // 1. 파일 내 오프셋(위치) 계산: PageID * 4KB
    int offset = pageId * PAGE_SIZE;

    // 2. 파일 범위를 벗어나는지 확인 (유효성 검사)
    if (offset >= GetNumPages() * PAGE_SIZE) {
        std::cerr << "Error: Reading past end of file" << std::endl;
        return;
    }

    // 3. 파일 포인터 이동 (Seek) 및 데이터 읽기
    dbFile.seekg(offset, std::ios::beg); // 파일 시작지점부터 offset만큼 이동
    dbFile.read(page.data, PAGE_SIZE);   // 4KB만큼 읽어서 page.data에 저장

    // 4. 읽기 중 에러 체크
    if (dbFile.fail()) {
        std::cerr << "Error: Failed to read page " << pageId << std::endl;
        dbFile.clear(); // 에러 플래그 초기화 (다음 작업을 위해)
    }
}

/**
 * @brief 페이지 쓰기: 특정 페이지 ID 위치에 데이터를 씁니다.
 * @param pageId 데이터를 쓸 페이지 번호
 * @param page 디스크에 기록할 페이지 객체
 */
void DiskManager::WritePage(int pageId, const Page& page) {
    // 1. 파일 내 오프셋 계산
    int offset = pageId * PAGE_SIZE;

    // 2. 쓰기 위치로 이동 (seekp: put pointer)
    dbFile.seekp(offset, std::ios::beg);

    // 3. 데이터 쓰기
    dbFile.write(page.data, PAGE_SIZE);

    // 4. 버퍼 비우기 (Flush)
    // 데이터가 메모리 버퍼에만 남아있지 않고 실제 디스크에 기록되도록 강제함
    dbFile.flush();

    // 5. 쓰기 실패 확인
    if (dbFile.fail()) {
        std::cerr << "Error: Failed to write page " << pageId << std::endl;
        dbFile.clear();
    }
}

/**
 * @brief 페이지 할당: 파일 끝에 빈 페이지를 추가하여 공간을 확보합니다.
 * @return 새로 할당된 페이지의 ID
 */
int DiskManager::AllocatePage() {
    // 현재 파일의 페이지 개수가 곧 새로운 페이지의 ID가 됨 (0부터 시작하므로)
    int newPageId = GetNumPages();

    // 빈 페이지 생성 및 쓰기 (실제 파일 크기를 늘림)
    Page emptyPage;
    WritePage(newPageId, emptyPage);

    return newPageId;
}

/**
 * @brief 파일 크기 확인: 현재 DB 파일의 총 페이지 개수를 반환합니다.
 * @return 전체 페이지 개수
 */
int DiskManager::GetNumPages() {
    // 현재 읽기 포인터 위치 저장 (작업 후 복구를 위해)
    std::streampos currentPos = dbFile.tellg();

    // 파일의 끝(End)으로 이동하여 전체 크기 확인
    dbFile.seekg(0, std::ios::end);
    std::streampos fileSize = dbFile.tellg();

    // 원래 위치로 포인터 복구
    dbFile.seekg(currentPos);

    // 전체 바이트 크기 / 페이지 크기(4KB) = 페이지 개수
    return (int)(fileSize / PAGE_SIZE);
}

DiskManager::DiskManager(const std::string& dbFileName) : fileName(dbFileName) {
    dbFile.open(fileName, std::ios::in | std::ios::out | std::ios::binary);

    if (!dbFile.is_open()) {
        dbFile.open(fileName, std::ios::out | std::ios::binary);
        dbFile.close();
        dbFile.open(fileName, std::ios::in | std::ios::out | std::ios::binary);

        Page p;
        MetaPage meta;
        std::memcpy(p.data, &meta, sizeof(MetaPage));
        WritePage(0, p);
        nextPageId = 1; // 다음 데이터는 1번 페이지부터
    }
    else {
        // 기존 파일이 있다면 파일 크기를 보고 nextPageId 복구
        nextPageId = GetNumPages();
    }
}