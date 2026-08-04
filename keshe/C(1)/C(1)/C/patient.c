// ============================================================
// 文件名: patient.c
// 描述: 患者管理模块（终极美化版）
// ============================================================

#include "his.h"

void addPatient() {
    clearScreen();
    drawModernBox(25, 5, 45, 20, " 添 加 患 者 ");
    Patient* p = (Patient*)malloc(sizeof(Patient));
    gotoxy(30, 8);  printf("患者ID: "); scanf("%d", &p->id);
    gotoxy(30, 10); printf("姓名: "); safeInput(p->name, MAX_NAME_LEN, 36, 10);
    gotoxy(30, 12); printf("性别(M/F): "); scanf(" %c", &p->gender);
    gotoxy(30, 14); printf("年龄: "); scanf("%d", &p->age);
    gotoxy(30, 16); printf("电话: "); safeInput(p->phone, MAX_PHONE_LEN, 36, 16);
    gotoxy(30, 18); printf("类型(0门诊/1住院): "); scanf("%d", &p->type);
    p->bedId = 0;
    if (p->type == 1) {
        int wardId;
        gotoxy(30, 20); printf("病房ID: "); scanf("%d", &wardId);
        p->bedId = allocateBed(p->id, wardId);
        if (p->bedId > 0) {
            setColor(COLOR_SUCCESS);
            gotoxy(30, 22); printf("分配床位成功，床位号:%d", p->bedId);
        } else {
            setColor(COLOR_ERROR);
            gotoxy(30, 22); printf("分配床位失败，请手动管理。");
        }
        setColor(COLOR_DEFAULT);
    }
    strcpy(p->creatTime, getCurrentTime());
    p->next = patientHead;
    patientHead = p;
    pressAnyKey();
}

void listPatients() {
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== 患 者 列 表 ====================\n\n");
    setColor(COLOR_DEFAULT);
    if (!patientHead) {
        printf("  暂无患者信息。\n");
        pressAnyKey();
        return;
    }
    const char* headers[] = {"ID", "姓名", "性别", "年龄", "电话", "类型", "床位", "建档时间"};
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
        const char* typeStr = (tail->type == 0) ? "门诊" : "住院";
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
}

Patient* findPatientById(int id) {
    Patient* p = patientHead;
    while (p) {
        if (p->id == id) return p;
        p = p->next;
    }
    return NULL;
}

void updatePatient() {
    clearScreen();
    drawModernBox(25, 5, 45, 11, " 修 改 患 者 信 息 ");
    int id;
    gotoxy(30, 8); printf("患者ID: "); scanf("%d", &id);
    Patient* p = findPatientById(id);
    if (!p) {
        setColor(COLOR_ERROR);
        gotoxy(30, 10); printf("患者不存在！");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }
    gotoxy(30, 10); printf("新姓名(原:%s): ", p->name);
    safeInput(p->name, MAX_NAME_LEN, 46, 10);
    gotoxy(30, 12); printf("新电话(原:%s): ", p->phone);
    safeInput(p->phone, MAX_PHONE_LEN, 46, 12);
    strcpy(p->creatTime, getCurrentTime());
    setColor(COLOR_SUCCESS);
    gotoxy(30, 14); printf("修改完成。");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void deletePatient() {
    clearScreen();
    drawModernBox(25, 5, 40, 8, " 删 除 患 者 ");
    int id;
    gotoxy(30, 8); printf("患者ID: "); scanf("%d", &id);
    Patient *p = patientHead, *prev = NULL;
    while (p) {
        if (p->id == id) {
            if (prev) prev->next = p->next;
            else patientHead = p->next;
            if (p->bedId > 0) freeBed(p->bedId);
            free(p);
            setColor(COLOR_SUCCESS);
            gotoxy(30, 10); printf("患者已删除。");
            setColor(COLOR_DEFAULT);
            pressAnyKey();
            return;
        }
        prev = p;
        p = p->next;
    }
    setColor(COLOR_ERROR);
    gotoxy(30, 10); printf("未找到该患者。");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}