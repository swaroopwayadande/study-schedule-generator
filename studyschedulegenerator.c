#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SUBJECTS 10
#define MAX_NAME_LENGTH 50
#define MAX_FILENAME_LENGTH 50

struct Days {
    int d1, m1, y1;
    int d2, m2, y2;
    char name[MAX_FILENAME_LENGTH];
} ds;

struct Subject {
    char name[MAX_NAME_LENGTH];
    int difficulty;
    float studyHours;
};

int isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int daysInMonth(int month, int year) {
    switch (month) {
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            return isLeapYear(year) ? 29 : 28;
        default:
            return 31;
    }
}

int countDays(int d1, int m1, int y1, int d2, int m2, int y2) {
    int days = 0;

    if (y1 != y2) {
        // Count remaining days in first year
        while (m1 <= 12) {
            days += daysInMonth(m1, y1) - d1 + 1;
            m1++;
            d1 = 1;
        }
        y1++;
        // Count full years in between
        while (y1 < y2) {
            days += isLeapYear(y1) ? 366 : 365;
            y1++;
        }
        // Count months and days in last year
        m1 = 1;
        while (m1 < m2) {
            days += daysInMonth(m1, y2);
            m1++;
        }
        days += d2;
    } else {
        // Same year
        while (m1 < m2) {
            days += daysInMonth(m1, y1) - d1 + 1;
            m1++;
            d1 = 1;
        }
        days += d2 - d1;
    }

    return days;
}

int inputSubjects(struct Subject subjects[], int *numSubjects) {
    printf("Enter the number of subjects (max %d): ", MAX_SUBJECTS);
    if (scanf("%d", numSubjects) != 1 || *numSubjects <= 0 || *numSubjects > MAX_SUBJECTS) {
        printf("Invalid number of subjects.\n");
        return 1;
    }
    getchar(); // consume newline

    for (int i = 0; i < *numSubjects; i++) {
        printf("Enter name of subject %d: ", i + 1);
        if (fgets(subjects[i].name, MAX_NAME_LENGTH, stdin) == NULL) {
            printf("Error reading subject name.\n");
            return 1;
        }
        // Remove trailing newline if any
        subjects[i].name[strcspn(subjects[i].name, "\n")] = 0;

        do {
            printf("Enter difficulty level (1 - hard, 2 - medium, 3 - easy) for %s: ", subjects[i].name);
            if (scanf("%d", &subjects[i].difficulty) != 1) {
                // Clear invalid input
                while (getchar() != '\n');
                subjects[i].difficulty = 0;
            }
        } while (subjects[i].difficulty < 1 || subjects[i].difficulty > 3);
        getchar(); // consume newline
    }

    printf("\nSubjects and their difficulty:\n");
    for (int i = 0; i < *numSubjects; i++) {
        printf("%d. %s - Difficulty: %d\n", i + 1, subjects[i].name, subjects[i].difficulty);
    }
    return 0;
}

void generateStudySchedule(int nSubjects, int nDays, struct Subject subjects[], const char *fileName) {
    FILE *filePtr = fopen(fileName, "w");
    if (!filePtr) {
        printf("Unable to create file %s\n", fileName);
        return;
    }

    float freshUpTime, collegeTime, sleepTime;

    printf("\nEnter time needed to freshen up per day (hours): ");
    scanf("%f", &freshUpTime);
    printf("Enter hours spent in college per day: ");
    scanf("%f", &collegeTime);
    printf("Enter time spent sleeping per day (hours): ");
    scanf("%f", &sleepTime);

    float totalFreeTime = 24.0f - (freshUpTime + collegeTime + sleepTime);
    if (totalFreeTime <= 0) {
        printf("Error: Total free time per day is less than or equal to zero.\n");
        fclose(filePtr);
        return;
    }
    printf("Total average free time per day: %.2f hours\n", totalFreeTime);
    fprintf(filePtr, "Study Schedule for %s\n\n", fileName);
    fprintf(filePtr, "Total average free time per day: %.2f hours\n", totalFreeTime);

    float totalHardHours = 0, totalMediumHours = 0, totalEasyHours = 0;

    // Calculate total free hours over period
    float totalFreeHours = totalFreeTime * nDays;
    printf("Total free study hours in period: %.2f\n", totalFreeHours);
    fprintf(filePtr, "Total free study hours in preparation period: %.2f hours\n\n", totalFreeHours);

    // Count number of subjects by difficulty
    int hardCount = 0, mediumCount = 0, easyCount = 0;
    for (int i = 0; i < nSubjects; i++) {
        if (subjects[i].difficulty == 1)
            hardCount++;
        else if (subjects[i].difficulty == 2)
            mediumCount++;
        else
            easyCount++;
    }

    // Allocate time percentages: hard 50%, medium 30%, easy 20%
    totalHardHours = totalFreeHours * 0.5f;
    totalMediumHours = totalFreeHours * 0.3f;
    totalEasyHours = totalFreeHours * 0.2f;

    // Divide total hours among subjects of same difficulty
    for (int i = 0; i < nSubjects; i++) {
        if (subjects[i].difficulty == 1 && hardCount > 0)
            subjects[i].studyHours = totalHardHours / hardCount;
        else if (subjects[i].difficulty == 2 && mediumCount > 0)
            subjects[i].studyHours = totalMediumHours / mediumCount;
        else if (subjects[i].difficulty == 3 && easyCount > 0)
            subjects[i].studyHours = totalEasyHours / easyCount;
        else
            subjects[i].studyHours = 0.0f;
    }

    fprintf(filePtr, "Detailed Daily Study Schedule (randomized allocation within subject hours):\n\n");

    srand((unsigned)time(NULL)); // Seed RNG

    for (int day = 1; day <= nDays; day++) {
        printf("\nDay %d:\n", day);
        fprintf(filePtr, "Day %d:\n", day);
        for (int i = 0; i < nSubjects; i++) {
            // Random allocation between 50% and 100% of studyHours per subject per day
            float minHours = subjects[i].studyHours * 0.5f / nDays;
            float maxHours = subjects[i].studyHours / nDays;
            float randomHours = minHours + ((float)rand() / RAND_MAX) * (maxHours - minHours);
            float minutes = randomHours * 60;

            if (minutes >= 20) {
                printf("Subject %d (%s): Study %.2f minutes\n", i + 1, subjects[i].name, minutes);
                fprintf(filePtr, "Subject %d (%s): Study %.2f minutes\n", i + 1, subjects[i].name, minutes);
            }
        }
    }
    fclose(filePtr);
    printf("\nStudy schedule saved to file: %s\n", fileName);
}

int main() {
    struct Subject subjects[MAX_SUBJECTS];
    int choice, nSubjects, totalDays;
    char fileName[MAX_FILENAME_LENGTH];
    int running = 1;

    while (running) {
        printf("\nWELCOME TO STUDY SCHEDULE GENERATOR\n");
        printf("Select an option:\n");
        printf("1. New Entry\n");
        printf("2. Existing User (View Schedule)\n");
        printf("3. Exit\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); // clear stdin
            printf("Invalid input! Try again.\n");
            continue;
        }
        getchar(); // consume newline

        switch (choice) {
            case 1: {
                printf("Enter your username (filename to save schedule): ");
                fgets(ds.name, MAX_FILENAME_LENGTH, stdin);
                ds.name[strcspn(ds.name, "\n")] = 0; // remove newline

                printf("-- Enter starting date --\n");
                printf("Date (1-31): ");
                scanf("%d", &ds.d1);
                printf("Month (1-12): ");
                scanf("%d", &ds.m1);
                printf("Year (e.g. 2025): ");
                scanf("%d", &ds.y1);

                printf("-- Enter ending date --\n");
                printf("Date (1-31): ");
                scanf("%d", &ds.d2);
                printf("Month (1-12): ");
                scanf("%d", &ds.m2);
                printf("Year (e.g. 2025): ");
                scanf("%d", &ds.y2);

                // Basic validation for date input
                if (ds.y2 < ds.y1 || (ds.y2 == ds.y1 && ds.m2 < ds.m1) || 
                    (ds.y2 == ds.y1 && ds.m2 == ds.m1 && ds.d2 < ds.d1)) {
                    printf("Invalid date period entered.\n");
                    break;
                }

                totalDays = countDays(ds.d1, ds.m1, ds.y1, ds.d2, ds.m2, ds.y2);
                printf("Number of days between the two dates: %d\n", totalDays);

                if (inputSubjects(subjects, &nSubjects) == 0) {
                    generateStudySchedule(nSubjects, totalDays, subjects, ds.name);
                }
                break;
            }
            case 2: {
                printf("Enter the filename to open and view schedule: ");
                fgets(fileName, MAX_FILENAME_LENGTH, stdin);
                fileName[strcspn(fileName, "\n")] = 0; // remove newline

                FILE *filePtr = fopen(fileName, "r");
                if (!filePtr) {
                    printf("Unable to open file '%s'.\n", fileName);
                    break;
                }

                printf("\nContents of '%s':\n\n", fileName);
                char ch;
                while ((ch = fgetc(filePtr)) != EOF) {
                    putchar(ch);
                }
                fclose(filePtr);
                break;
            }
            case 3: {
                printf("Exiting program. Goodbye!\n");
                running = 0;
                break;
            }
            default:
                printf("Invalid choice. Please select 1, 2, or 3.\n");
        }
    }

    return 0;
}
