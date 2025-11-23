#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define FLOORS 3           // 주차장 층수
#define PER_FLOOR 40       // 층별 주차 공간
#define TOTAL_SPOTS (FLOORS * PER_FLOOR) // 전체 주차 공간 수 (120)
#define DATA_FILE "parking_data.dat"     // 데이터 저장 파일명

// 주차 공간 하나를 나타내는 구조체
typedef struct {
    int isOccupied;         // 0: 비어있음, 1: 차있음
    char Number[10];      // 차량 번호 (예: "12가3456")
    time_t entryTime;       // 입차 시간
} ParkingSpot;

// 전역 변수
ParkingSpot parkingLot[FLOORS][PER_FLOOR];
long long totalRevenue = 0; // 총 매출액 (화면엔 안 보이지만 저장은 함)
int carCount = 0;           // 현재 주차된 차량 수

// --- 헬퍼 함수 프로토타입 ---
int check(char* plate);                           // 차량 번호 중복 검사
int Spot(char* spotStr, int* floor, int* spot);     // 위치 문자열 -> 인덱스 변환
long long Fee(double parkedSeconds);              // 요금 계산
void SpotID(int floor, int spot, char* spotID);         // 인덱스 -> 위치 문자열 변환

// --- 파일 입출력 함수 ---
void loadData(); // 시작 시 데이터 불러오기
void saveData(); // 종료 시 데이터 저장하기

// --- 메인 기능 함수 프로토타입 ---
void initializeParking();
void displayMenu();
void Entry();
void Exit();
void ParkingStatus();
void Search(); // [3주차 추가] 차량 위치 검색

int main() {
    int choice;

    // 프로그램 시작 시 파일에서 데이터 불러오기
    loadData();

    while (1) {
        system("cls"); // Windows 화면 지우기
        displayMenu();
        printf("\n메뉴를 선택하세요 (1-5): ");

        if (scanf_s("%d", &choice) != 1) {
            choice = 0;
        }

        while (getchar() != '\n'); // 버퍼 비우기

        switch (choice) {
        case 1: Entry(); break;
        case 2: Exit(); break;
        case 3: ParkingStatus(); break;
        case 4: Search(); break; // [3주차] 검색 기능
        case 5:
            printf("데이터를 저장하고 종료합니다...\n");
            saveData(); // 종료 전 저장
            printf("프로그램을 종료합니다. 감사합니다.\n");
            return 0;
        default:
            printf("! 경고: 1~5 사이의 숫자만 입력하세요.\n");
        }

        printf("\n(계속하려면 Enter 키를 누르세요...)");
        (void)getchar();
    }
    return 0;
}

// --- 구현부 ---

void initializeParking() {
    for (int i = 0; i < FLOORS; i++) {
        for (int j = 0; j < PER_FLOOR; j++) {
            parkingLot[i][j].isOccupied = 0;
            strcpy_s(parkingLot[i][j].Number, sizeof(parkingLot[i][j].Number), "");
            parkingLot[i][j].entryTime = 0;
        }
    }
    carCount = 0;
}

// 데이터 불러오기
void loadData() {
    FILE* fp = NULL;
    if (fopen_s(&fp, DATA_FILE, "rb") == 0) {
        fread(parkingLot, sizeof(ParkingSpot), FLOORS * PER_FLOOR, fp);
        fread(&totalRevenue, sizeof(long long), 1, fp);
        fclose(fp);

        // 불러온 데이터를 바탕으로 현재 차량 수(carCount) 재계산
        carCount = 0;
        for (int i = 0; i < FLOORS; i++)
            for (int j = 0; j < PER_FLOOR; j++)
                if (parkingLot[i][j].isOccupied) carCount++;

        printf("[시스템] 데이터 불러오기 성공. (현재 주차 차량: %d대)\n", carCount);
        // [수정] 매출액 출력 삭제

        // 잠시 대기
        time_t start = time(NULL);
        while (time(NULL) - start < 1);
    }
    else {
        printf("[시스템] 저장된 데이터가 없어 새로 시작합니다.\n");
        initializeParking();
    }
}

// 데이터 저장하기
void saveData() {
    FILE* fp = NULL;
    if (fopen_s(&fp, DATA_FILE, "wb") == 0) {
        fwrite(parkingLot, sizeof(ParkingSpot), FLOORS * PER_FLOOR, fp);
        fwrite(&totalRevenue, sizeof(long long), 1, fp);
        fclose(fp);
        printf("[시스템] 데이터 저장이 완료되었습니다.\n");
    }
    else {
        printf("[오류] 파일 저장에 실패했습니다.\n");
    }
}

void displayMenu() {
    printf("===================================\n");
    printf("    C언어 주차 관리 프로그램\n"); // [수정] v3.1 제거
    printf("    [ 현재 주차 차량: %d / %d ]\n", carCount, TOTAL_SPOTS); // [수정] 매출 제거
    printf("===================================\n");
    printf("  1. 차량 입차\n");
    printf("  2. 차량 출차\n");
    printf("  3. 주차 현황 조회\n");
    printf("  4. 차량 위치 검색\n");
    printf("  5. 저장 및 종료\n");
    printf("===================================\n");
}

void ParkingStatus() {
    printf("\n--- [ 3. 주차 현황 조회 ] ---\n");
    printf("( O: 주차 중 / X: 비어있음 )\n");
    for (int i = 0; i < FLOORS; i++) {
        printf("\n--- 지하 %d층 ---\n", i + 1);
        printf("  A 구역: ");
        for (int j = 0; j < 10; j++) printf("[%c] ", parkingLot[i][j].isOccupied ? 'O' : 'X');
        printf("\n  B 구역: ");
        for (int j = 10; j < 20; j++) printf("[%c] ", parkingLot[i][j].isOccupied ? 'O' : 'X');
        printf("\n  C 구역: ");
        for (int j = 20; j < 30; j++) printf("[%c] ", parkingLot[i][j].isOccupied ? 'O' : 'X');
        printf("\n  D 구역: ");
        for (int j = 30; j < 40; j++) printf("[%c] ", parkingLot[i][j].isOccupied ? 'O' : 'X');
        printf("\n---------------------------------\n");
    }
}

// [3주차 추가] 차량 위치 검색
void Search() {
    char plate[10] = { 0 };
    printf("\n--- [ 4. 차량 위치 검색 ] ---\n");
    printf("찾으시는 차량 번호: ");

    if (scanf_s("%s", plate, (unsigned int)sizeof(plate)) != 1) {
        printf("입력 오류입니다.\n");
        while (getchar() != '\n'); return;
    }
    while (getchar() != '\n');

    for (int i = 0; i < FLOORS; i++) {
        for (int j = 0; j < PER_FLOOR; j++) {
            if (parkingLot[i][j].isOccupied && strcmp(parkingLot[i][j].Number, plate) == 0) {

                char spotID[20] = { 0 };
                SpotID(i, j, spotID);

                double duration = difftime(time(NULL), parkingLot[i][j].entryTime);
                int hours = (int)(duration / 3600);
                int mins = ((int)duration % 3600) / 60;

                printf("\n [검색 성공]\n");
                printf("    차량 번호: %s\n", plate);
                printf("    주차 위치: %s\n", spotID);
                printf("    주차 시간: %d시간 %d분 경과 중\n", hours, mins);
                return;
            }
        }
    }
    printf("\n [검색 실패] 주차장에 없는 차량입니다.\n");
}

void Entry() {
    char plate[10] = { 0 };
    char spotStr[10] = { 0 };
    int floor, spot;

    // 만차 확인
    if (carCount >= TOTAL_SPOTS) {
        printf("\n [오류] 주차장이 꽉 찼습니다! (만차)\n");
        return;
    }

    printf("\n--- [ 1. 차량 입차 ] ---\n");
    printf("입차할 차량 번호를 입력하세요 (예: 12가3456): ");
    if (scanf_s("%s", plate, (unsigned int)sizeof(plate)) != 1) {
        while (getchar() != '\n'); return;
    }
    while (getchar() != '\n');

    if (check(plate) == 1) {
        printf("\n [실패] 이미 주차장에 등록된 차량(%s)입니다.\n", plate);
        return;
    }

    printf("주차할 위치를 입력하세요 (예: B1-A5, B2-C3 등): ");
    if (scanf_s("%s", spotStr, (unsigned int)sizeof(spotStr)) != 1) {
        while (getchar() != '\n'); return;
    }
    while (getchar() != '\n');

    if (Spot(spotStr, &floor, &spot) == 0) {
        printf("\n [실패] 유효하지 않은 위치입니다.\n");
        return;
    }

    if (parkingLot[floor][spot].isOccupied == 1) {
        printf("\n [실패] 해당 위치는 이미 사용 중입니다.\n");
        return;
    }

    parkingLot[floor][spot].isOccupied = 1;
    strcpy_s(parkingLot[floor][spot].Number, sizeof(parkingLot[floor][spot].Number), plate);
    parkingLot[floor][spot].entryTime = time(NULL);

    carCount++; // 차량 수 증가
    printf("\n [완료] 주차가 완료되었습니다. (현재 %d대)\n", carCount);
}

void Exit() {
    char plate[10] = { 0 };
    printf("\n--- [ 2. 차량 출차 ] ---\n");
    printf("출차할 차량 번호를 입력하세요: ");
    if (scanf_s("%s", plate, (unsigned int)sizeof(plate)) != 1) {
        while (getchar() != '\n'); return;
    }
    while (getchar() != '\n');

    int found = 0;
    for (int i = 0; i < FLOORS; i++) {
        for (int j = 0; j < PER_FLOOR; j++) {
            if (parkingLot[i][j].isOccupied == 1 && strcmp(parkingLot[i][j].Number, plate) == 0) {
                found = 1;
                time_t exitTime = time(NULL);
                double parkedSeconds = difftime(exitTime, parkingLot[i][j].entryTime);
                long long fee = Fee(parkedSeconds);

                totalRevenue += fee; // 매출 누적
                carCount--;          // 차량 수 감소

                char spotID[20] = { 0 };
                SpotID(i, j, spotID);

                printf("\n [정산 내역]\n");
                printf("    위치: %s / 번호: %s\n", spotID, parkingLot[i][j].Number);
                printf("    시간: %.0f초 (약 %.1f시간)\n", parkedSeconds, parkedSeconds / 3600.0);
                printf("    [최종 요금: %lld 원]\n", fee);

                parkingLot[i][j].isOccupied = 0;
                strcpy_s(parkingLot[i][j].Number, sizeof(parkingLot[i][j].Number), "");
                parkingLot[i][j].entryTime = 0;

                printf("\n>> 출차가 완료되었습니다. (현재 %d대)\n", carCount);
                goto exit_success;
            }
        }
    }
exit_success:
    if (found == 0) {
        printf("\n [오류] 해당 차량(%s)이 주차장에 없습니다.\n", plate);
    }
}

// 헬퍼 함수들
int check(char* plate) {
    for (int i = 0; i < FLOORS; i++)
        for (int j = 0; j < PER_FLOOR; j++)
            if (parkingLot[i][j].isOccupied && strcmp(parkingLot[i][j].Number, plate) == 0) return 1;
    return 0;
}

int Spot(char* spotStr, int* floor, int* spot) {
    int f, s; char a;
    if (sscanf_s(spotStr, "B%d-%c%d", &f, &a, (unsigned int)sizeof(a), &s) != 3) return 0;
    if (f < 1 || f > 3 || s < 1 || s > 10) return 0;
    *floor = f - 1;
    if (a == 'A' || a == 'a') *spot = s - 1;
    else if (a == 'B' || a == 'b') *spot = 10 + s - 1;
    else if (a == 'C' || a == 'c') *spot = 20 + s - 1;
    else if (a == 'D' || a == 'd') *spot = 30 + s - 1;
    else return 0;
    return 1;
}

long long Fee(double ps) {
    if (ps <= 0) return 0;
    int h = (int)ceil(ps / 3600.0);
    return (h <= 2) ? 0 : (long long)(h - 2) * 1000;
}

void SpotID(int floor, int spot, char* spotID) {
    char area = (spot < 10) ? 'A' : (spot < 20) ? 'B' : (spot < 30) ? 'C' : 'D';
    sprintf_s(spotID, 20, "B%d-%c%d", floor + 1, area, (spot % 10) + 1);
}