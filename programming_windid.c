#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define FLOORS 3           // 주차장 층수
#define PER_FLOOR 40 // 층별 주차 공간 (A:10, B:10, C:10, D:10)

// 주차 공간 하나를 나타내는 구조체
typedef struct {
    int isOccupied;         // 0: 비어있음, 1: 차있음
    char Number[10]; // 차량 번호 (예: "12가3456")
    time_t entryTime;       // 입차 시간 (time_t 자료형)
} ParkingSpot;

// parkingLot[층][칸]
ParkingSpot parkingLot[FLOORS][PER_FLOOR];

int check(char* plate);                           // 1. 차량 번호 중복 검사
int Spot(char* spotStr, int* floor, int* spot);     // 2. 위치 문자열("B1-A5")을 인덱스로 변환
long long Fee(double parkedSeconds);              // 3. 요금 계산
void SpotID(int floor, int spot, char* spotID);         // 4. 인덱스를 위치 문자열("B1-A5")로 변환

void initializeParking();   // 주차장 초기화 함수
void displayMenu();         // 메인 메뉴 표시 함수
void Entry();        // 차량 입차 함수
void Exit();         // 차량 출차 함수
void ParkingStatus();   // 주차 현황 조회 함수

int main() {
    int choice;

    // 프로그램 시작 시 주차장을 모두 비어있는 상태로 초기화
    initializeParking();

    // 메인 루프 (프로그램이 종료되지 않게)
    while (1) {
        displayMenu();
        printf("\n메뉴를 선택하세요 (1-4): ");
        scanf_s("%d", &choice);

        // 입력 버퍼에 남아있는 \n(엔터) 제거
        while (getchar() != '\n');
        switch (choice) {
        case 1:
            Entry();
            break;
        case 2:
            Exit();
            break;
        case 3:
            ParkingStatus();
            break;
        case 4:
            printf("프로그램을 종료합니다. 감사합니다.\n");
            return 0; // 프로그램 종료
        default:
            printf("! 경고: 1~4 사이의 숫자만 입력하세요.\n");
        }

        printf("\n(계속하려면 Enter 키를 누르세요...)");
        getchar(); // 사용자가 Enter를 누를 때까지 대기
    }
    return 0;
}

// --- 3. 핵심 기능 함수 구현 ---
/*
주차장 배열을 모두 빈 상태(0)로 초기화합니다.
 */

void initializeParking() {
    for (int i = 0; i < FLOORS; i++) {
        for (int j = 0; j < PER_FLOOR; j++) {
            parkingLot[i][j].isOccupied = 0;
            strcpy_s(parkingLot[i][j].Number, sizeof(parkingLot[i][j].Number), "");
            parkingLot[i][j].entryTime = 0;
        }
    }
}

/*
사용자에게 메인 메뉴를 보여줍니다.
 */

void displayMenu() {
    printf("===================================\n");
    printf("   C언어 주차 관리 프로그램 \n");
    printf("===================================\n");
    printf("  1. 차량 입차\n");
    printf("  2. 차량 출차\n");
    printf("  3. 주차 현황 조회\n");
    printf("  4. 프로그램 종료\n");
    printf("===================================\n");
}

/*
전체 주차장을 순회하며 'O'(주차됨) 또는 'X'(비어있음)로 표시합니다.
 */

void ParkingStatus() {
    printf("\n--- [ 3. 주차 현황 조회 ] ---\n");
    printf("( O: 주차 중 / X: 비어있음 )\n");

    for (int i = 0; i < FLOORS; i++) {
        printf("\n--- 지하 %d층 ---\n", i + 1);

        // A 구역 (인덱스 0 ~ 9)
        printf("  A 구역 (1-10): ");
        for (int j = 0; j < 10; j++) {
            printf("[%c] ", parkingLot[i][j].isOccupied ? 'O' : 'X');
        }

        // B 구역 (인덱스 10 ~ 19)
        printf("\n  B 구역 (1-10): ");
        for (int j = 10; j < 20; j++) {
            printf("[%c] ", parkingLot[i][j].isOccupied ? 'O' : 'X');
        }

        // C 구역 (인덱스 20 ~ 29)
        printf("\n  C 구역 (1-10): ");
        for (int j = 20; j < 30; j++) {
            printf("[%c] ", parkingLot[i][j].isOccupied ? 'O' : 'X');
        }

        // D 구역 (인덱스 30 ~ 39)
        printf("\n  D 구역 (1-10): ");
        for (int j = 30; j < 40; j++) {
            printf("[%c] ", parkingLot[i][j].isOccupied ? 'O' : 'X');
        }
        printf("\n---------------------------------\n");
    }
}

/*
1. 차량 입차 기능
 */

void Entry() {
    char plate[10];
    char spotStr[10]; // "B1-A5" 입력을 받을 문자열
    int floor, spot;

    printf("\n--- [ 1. 차량 입차 ] ---\n");
    printf("입차할 차량 번호를 입력하세요 (예: 12가3456): ");

    if (scanf_s("%s", plate, (unsigned int)sizeof(plate)) != 1) {
        printf("입력 오류입니다.\n"); while (getchar() != '\n'); return;
    }
    while (getchar() != '\n'); // 버퍼 비우기

    // [2주차 기능 1] 차량 번호 중복 검사
    if (check(plate) == 1) {
        printf("\n [입차 실패] 이미 주차장에 등록된 차량(%s)입니다.\n", plate);
        return;
    }

    // [2주차 기능 2] 위치 지정 입차
    printf("주차할 위치를 입력하세요 (예: B1-A5, B2-C3 등): ");
    if (scanf_s("%s", spotStr, (unsigned int)sizeof(spotStr)) != 1) {
        printf("입력 오류입니다.\n"); while (getchar() != '\n'); return;
    }
    while (getchar() != '\n'); // 버퍼 비우기

    // [2주차 기능 3] 위치 유효성 검사 (Spot 함수 호출)
    if (Spot(spotStr, &floor, &spot) == 0) {
        // 0이 반환되면 유효하지 않은 위치
        printf("\n [입차 실패] '%s'는 유효하지 않은 위치입니다.\n", spotStr);
        printf("             (입력 예: B1-A1 ~ B3-D10)\n");
        return;
    }

    // [2주차 기능 4] 해당 자리가 비어있는지 검사
    if (parkingLot[floor][spot].isOccupied == 1) {
        printf("\n [입차 실패] 해당 위치(%s)는 이미 사용 중입니다.\n", spotStr);
        printf("             차량 번호: %s\n", parkingLot[floor][spot].Number);
        return;
    }

    // 모든 검사 통과: 주차 실행
    parkingLot[floor][spot].isOccupied = 1;
    strcpy_s(parkingLot[floor][spot].Number, sizeof(parkingLot[floor][spot].Number), plate);
    parkingLot[floor][spot].entryTime = time(NULL);

    printf("\n [주차 완료]\n");
    printf("    차량 번호: %s\n", plate);
    printf("    주차 위치: %s\n", spotStr);
}

/*
2. 차량 출차 기능
 */

void Exit() {
    char plate[20];
    printf("\n--- [ 2. 차량 출차 ] ---\n");
    printf("출차할 차량 번호를 입력하세요: ");

    if (scanf_s("%s", plate, (unsigned int)sizeof(plate)) != 1) {
        printf("입력 오류입니다.\n"); while (getchar() != '\n'); return;
    }
	while (getchar() != '\n'); // 버퍼 비우기

    int found = 0;

    for (int i = 0; i < FLOORS; i++) {
        for (int j = 0; j < PER_FLOOR; j++) {

            // isOccupied가 1이고(차량이 있고), 차량 번호가 일치하면
            if (parkingLot[i][j].isOccupied == 1 && strcmp(parkingLot[i][j].Number, plate) == 0) {
                // 1. 차량 발견
                found = 1;

                // 2. 시간 계산 (1주차: 초 단위로만 계산)
                time_t exitTime = time(NULL);
                time_t entryTime = parkingLot[i][j].entryTime;
                double parkedSeconds = difftime(exitTime, entryTime);

                long long fee = Fee(parkedSeconds);

                char spotID[20];
                SpotID(i, j, spotID);

                printf("\n [정산 내역]\n");
                printf("    차량 번호: %s\n", parkingLot[i][j].Number);
                printf("    주차 위치: %s\n", spotID);
                printf("    총 주차 시간: %.0f 초 (약 %.1f 시간)\n", parkedSeconds, parkedSeconds / 3600.0);
                printf("    [최종 요금: %lld 원]\n", fee); // long long 타입은 %lld로 출력

                // 4. 데이터 초기화 (차량 빼기)
                parkingLot[i][j].isOccupied = 0;
                strcpy_s(parkingLot[i][j].Number, sizeof(parkingLot[i][j].Number), plate);
                parkingLot[i][j].entryTime = 0;

                printf("\n>> 출차가 완료되었습니다.\n");
                goto exit_success;
            }
        }
    }

exit_success:
    if (found == 0) {
        printf("\n [오류] 해당 차량(%s)이 주차장에 없습니다.\n", plate);
    }
}

// --- 헬퍼(Helper) 함수 구현부 ---

/*
 1. 차량 번호 중복 검사
*/
    int check(char* plate) {
    for (int i = 0; i < FLOORS; i++) {
        for (int j = 0; j < PER_FLOOR; j++) {
            if (parkingLot[i][j].isOccupied == 1 && strcmp(parkingLot[i][j].Number, plate) == 0) {
                return 1; // 중복 발견
            }
        }
    }
    return 0; // 중복 없음
}

/*
 위치 문자열("B1-A5")을 인덱스로 변환
*/
int Spot(char* spotStr, int* floor, int* spot) {
    int floorNum, spotNum;
    char area; // 'A', 'B', 'C', 'D'
    int offset = 0;

    if (sscanf_s(spotStr, "B%d-%c%d", &floorNum, &area, 1, &spotNum) != 3) {
        return 0;
    }

    // 1. 층 번호 검사 (B1, B2, B3)
    if (floorNum < 1 || floorNum > 3) return 0;
    *floor = floorNum - 1;

    // 2. 구역(A-D) -> 인덱스 오프셋(0, 10, 20, 30) 변환
    if (area == 'A' || area == 'a') offset = 0;
    else if (area == 'B' || area == 'b') offset = 10;
    else if (area == 'C' || area == 'c') offset = 20;
    else if (area == 'D' || area == 'd') offset = 30;
    else return 0; // A,B,C,D가 아님

    // 3. 칸 번호(1~10) 검사
    if (spotNum < 1 || spotNum > 10) return 0;

    // 4. 최종 인덱스 계산
    *spot = offset + (spotNum - 1); // 예: C3 -> 20 + (3-1) = 22

    return 1; // 변환 성공
}

/*
 3. 요금 계산
*/
long long Fee(double parkedSeconds) {
    const int freeHours = 2;
    const int hourlyRate = 1000;

    if (parkedSeconds <= 0) return 0;

    // 7201초 (2시간 1초) -> 7201 / 3600 = 2.0002... -> ceil() -> 3시간
    int totalHours = (int)ceil(parkedSeconds / 3600.0);

    if (totalHours <= freeHours) {
		return 0; // 2시간 이내 무료
    }
    else {
        // 3시간 주차 -> (3 - 2) * 1000 = 1000원
        return (long long)(totalHours - freeHours) * hourlyRate;
    }
}

/*
 4. 인덱스를 위치 문자열("B1-A5")로 변환
*/
void SpotID(int floor, int spot, char* spotID) {
    char area;
	int spotNum = (spot % 10) + 1; // 0~9 -> 1~10

    if (spot < 10) area = 'A';
    else if (spot < 20) area = 'B';
    else if (spot < 30) area = 'C';
    else area = 'D';

	//  최종 문자열 생성
    sprintf_s(spotID, 20, "B%d-%c%d", floor + 1, area, spotNum);
}