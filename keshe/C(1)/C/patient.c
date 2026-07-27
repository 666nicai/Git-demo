// ============================================================
// ?????: patient.c
// ????: ?????????÷Ï?????????
//       ????????????ï”???????????????????????
//       ??????????¦Ê¦Ë???
// ============================================================

#include "his.h"

extern UserRole currentUserRole;
extern int currentDoctorId;

// ?????????????????????????????????
static int isPatientBelongsToDoctor(int doctorId, int patientId) {
    MedicalRecord* r = recordHead;
    while (r) {
        if (r->doctorId == doctorId && r->patientId == patientId)
            return 1;
        r = r->next;
    }
    return 0;
}

// ======================= ???????????§ß??????? =======================
void addPatient() {
    clearScreen();
    drawModernBox(25, 5, 45, 20, " ?? ?? ?? ?? ");
    Patient* p = (Patient*)malloc(sizeof(Patient));
    gotoxy(30, 8);  printf("????ID: "); scanf("%d", &p->id);
    gotoxy(30, 10); printf("????: "); safeInput(p->name, 20, 36, 10);
    gotoxy(30, 12); printf("???(M/F): "); scanf(" %c", &p->gender);
    gotoxy(30, 14); printf("????: "); scanf("%d", &p->age);
    gotoxy(30, 16); printf("?´Â: "); safeInput(p->phone, MAX_PHONE_LEN, 36, 16);
    gotoxy(30, 18); printf("????(0????/1??): "); scanf("%d", &p->type);
    p->bedId = 0;
    if (p->type == 1) {
        int wardId;
        gotoxy(30, 20); printf("????ID: "); scanf("%d", &wardId);
        p->bedId = allocateBed(p->id, wardId);
        if (p->bedId > 0) {
            setColor(COLOR_SUCCESS);
            gotoxy(30, 22); printf("????¦Ë???????¦Ë??:%d", p->bedId);
        } else {
            setColor(COLOR_ERROR);
            gotoxy(30, 22); printf("????¦Ë???????????????");
        }
        setColor(COLOR_DEFAULT);
    }
    strcpy(p->creatTime, getCurrentTime());
    p->next = patientHead;
    patientHead = p;
    pressAnyKey();
}

// ======================= ?????§Ý??? =======================
void listPatients() {
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== ?? ?? ?? ?? ====================\n\n");
    setColor(COLOR_DEFAULT);
    if (!patientHead) {
        printf("  ????????????\n");
        pressAnyKey();
        return;
    }
    const char* headers[] = {"ID", "????", "???", "????", "?´Â", "????", "??¦Ë", "???????"};
    int widths[] = {6, 10, 6, 6, 15, 8, 6, 20};
    printTableHeader(headers, widths, 8);
    Patient* p = patientHead;
    Patient* tail = p;
    while(tail->next!=NULL) tail=tail->next;
    int row = 0;
    while (tail) {
        char idStr[10], ageStr[10], bedStr[10];
        sprintf(idStr, "%d", tail->id);
        sprintf(ageStr, "%d", tail->age);
        sprintf(bedStr, "%d", tail->bedId);
        char genderStr[4] = {tail->gender, '\0'};
        const char* typeStr = (tail->type == 0) ? "????" : "??";
        const char* values[] = {idStr, tail->name, genderStr, ageStr, tail->phone, typeStr, bedStr, tail->creatTime};
        printTableRow(values, widths, 8, (row % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD);
        Patient* prev = patientHead;
        if(prev == tail) break;
        while(prev->next!=tail){
            prev=prev->next;
        }
        tail = prev;
        row++;
    }
    pressAnyKey();
}

// ======================= ??????? =======================
Patient* findPatientById(int id) {
    Patient* p = patientHead;
    while (p) {
        if (p->id == id) return p;
        p = p->next;
    }
    return NULL;
}

// ======================= ????????? =======================
void updatePatient() {
    clearScreen();
    drawModernBox(25, 5, 45, 11, " ?? ?? ?? ?? ?? ? ");
    int id;
    gotoxy(30, 8); printf("????ID: "); scanf("%d", &id);
    Patient* p = findPatientById(id);
    if (!p) {
        setColor(COLOR_ERROR);
        gotoxy(30, 10); printf("??????????");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }
    gotoxy(30, 10); printf("??????(?:%s): ", p->name);
    safeInput(p->name, 20, 46, 10);
    gotoxy(30, 12); printf("??´Â(?:%s): ", p->phone);
    safeInput(p->phone, MAX_PHONE_LEN, 46, 12);
    strcpy(p->creatTime, getCurrentTime());
    setColor(COLOR_SUCCESS);
    gotoxy(30, 14); printf("?????¨À?");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

// ======================= ???????????????????? =======================
void deletePatient() {
    if (currentUserRole != ROLE_ADMIN) {
        clearScreen();
        setColor(COLOR_ERROR);
        printf("\n  ???????????????????????\n");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }

    clearScreen();
    drawModernBox(25, 5, 40, 8, " ? ?? ?? ?? ");
    int id;
    gotoxy(30, 8); printf("????ID: "); scanf("%d", &id);
    Patient *p = patientHead, *prev = NULL;
    while (p) {
        if (p->id == id) {
            if (prev) prev->next = p->next;
            else patientHead = p->next;
            if (p->bedId > 0) freeBed(p->bedId);
            free(p);
            setColor(COLOR_SUCCESS);
            gotoxy(30, 10); printf("???????????");
            setColor(COLOR_DEFAULT);
            pressAnyKey();
            return;
        }
        prev = p;
        p = p->next;
    }
    setColor(COLOR_ERROR);
    gotoxy(30, 10); printf("¦Ä?????????");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

// ======================= ???????????????????? =======================
void listMyPatients(int doctorId) {
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ========== ?? ?? ?? ?? ?? ?? ==========\n\n");
    setColor(COLOR_DEFAULT);

    int patientIds[200];
    int count = 0;
    MedicalRecord* r = recordHead;
    while (r) {
        if (r->doctorId == doctorId) {
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (patientIds[i] == r->patientId) { found = 1; break; }
            }
            if (!found && count < 200) {
                patientIds[count++] = r->patientId;
            }
        }
        r = r->next;
    }

    if (count == 0) {
        printf("  ?????????????\n");
        pressAnyKey();
        return;
    }

    const char* headers[] = {"ID", "????", "???", "????", "?´Â", "????"};
    int widths[] = {6, 10, 6, 6, 15, 8};
    printTableHeader(headers, widths, 6);

    for (int i = 0; i < count; i++) {
        Patient* p = findPatientById(patientIds[i]);
        if (!p) continue;
        char idStr[10], ageStr[10];
        sprintf(idStr, "%d", p->id);
        sprintf(ageStr, "%d", p->age);
        char genderStr[4] = {p->gender, '\0'};
        const char* typeStr = (p->type == 0) ? "????" : "??";
        const char* values[] = {idStr, p->name, genderStr, ageStr, p->phone, typeStr};
        printTableRow(values, widths, 6, (i % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD);
    }
    pressAnyKey();
}

// ======================= ????????????????????? =======================
void updateMyPatient(int doctorId) {
    clearScreen();
    drawModernBox(25, 5, 45, 11, " ?? ?? ?? ?? ?? ?? ");
    int patientId;
    gotoxy(30, 8); printf("????ID: "); scanf("%d", &patientId);
    if (!isPatientBelongsToDoctor(doctorId, patientId)) {
        setColor(COLOR_ERROR);
        gotoxy(30, 10); printf("???????????????????????");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }
    Patient* p = findPatientById(patientId);
    if (!p) {
        setColor(COLOR_ERROR);
        gotoxy(30, 10); printf("??????????");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }
    gotoxy(30, 10); printf("??????(?:%s): ", p->name);
    safeInput(p->name, MAX_NAME_LEN, 46, 10);
    gotoxy(30, 12); printf("??´Â(?:%s): ", p->phone);
    safeInput(p->phone, MAX_PHONE_LEN, 46, 12);
    strcpy(p->creatTime, getCurrentTime());
    setColor(COLOR_SUCCESS);
    gotoxy(30, 14); printf("?????¨À?");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

// ======================= ????????????????????? =======================
void deleteMyPatient(int doctorId) {
    setColor(COLOR_ERROR);
    printf("\n  ???????????????????\n");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}