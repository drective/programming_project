#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>

#define FLOORS 3           // 주차장 층수
#define PER_FLOOR 40       // 층별 주차 공간
#define TOTAL_SPOTS (FLOORS * PER_FLOOR) 

typedef struct {
    int isOccupied;         // 0: 비어있음, 1: 차있음
    char Number[10];      // 차량 번호
    time_t entryTime;       // 입차 시간
} ParkingSpot;

ParkingSpot parkingLot[FLOORS][PER_FLOOR];
int carCount = 0;

// 헬퍼 함수 프로토타입
int check(char* plate);
int isValidFormat(char* plate); // [추가] 형식 검사 함수
int Spot(char* spotStr, int* floor, int* spot);
long long Fee(double parkedSeconds);
void SpotID(int floor, int spot, char* spotID);

// 메인 기능 함수 프로토타입
void initializeParking();
void displayMenu();
void Entry();
void Exit();
void ParkingStatus();
void Search();

int main() {
    int choice;
    initializeParking();

    while (1) {
        system("cls");
        displayMenu();
        printf("\n메뉴를 선택하세요 (1-5): ");

        if (scanf_s("%d", &choice) != 1) {
            choice = 0;
        }
        while (getchar() != '\n');

        switch (choice) {
        case 1: Entry(); break;
        case 2: Exit(); break;
        case 3: ParkingStatus(); break;
        case 4: Search(); break;
        case 5:
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

void displayMenu() {
    printf("===================================\n");
    printf("    C언어 주차 관리 프로그램\n");
    printf("    [ 현재 주차 차량: %d / %d ]\n", carCount, TOTAL_SPOTS);
    printf("===================================\n");
    printf("  1. 차량 입차\n");
    printf("  2. 차량 출차\n");
    printf("  3. 주차 현황 조회\n");
    printf("  4. 차량 위치 검색\n");
    printf("  5. 프로그램 종료\n");
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

    // [추가된 부분] 차량 번호 형식 검사
    if (isValidFormat(plate) == 0) {
        printf("\n [실패] 잘못된 차량 번호 형식입니다.\n");
        printf("         (예: 12가3456 - 숫자2+한글1+숫자4)\n");
        return;
    }

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

    carCount++;
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

                carCount--;
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

// 차량 번호 형식 검사 (12가3456)
int isValidFormat(char* plate) {
    if (strlen(plate) != 8) return 0;

    // 앞 2자리가 숫자인지 확인
    if (!isdigit(plate[0]) || !isdigit(plate[1])) return 0;

    // 뒤 4자리가 숫자인지 확인
    if (!isdigit(plate[4]) || !isdigit(plate[5]) ||
        !isdigit(plate[6]) || !isdigit(plate[7])) return 0;

    // 가운데 글자가 영어가 아닌지 확인 (한글 코드 체크)
    if (!((unsigned char)plate[2] & 0x80)) return 0;

    return 1; // 통과
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