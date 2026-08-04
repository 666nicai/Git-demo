// ============================================================
// 文件名: doctor.c
// 描述: 医生管理模块（美化版）
// ============================================================

#include "his.h"

void addDoctor() {
    clearScreen();
    drawModernBox(25, 5, 45, 15, " 添 加 医 生 ");
    Doctor* doc = (Doctor*)malloc(sizeof(Doctor));
    gotoxy(30, 8);  printf("医生ID: "); scanf("%d", &doc->id);
    gotoxy(30, 10); printf("姓名: "); safeInput(doc->name, MAX_NAME_LEN, 36, 10);
    gotoxy(30, 12); printf("所属科室ID: "); scanf("%d", &doc->deptId);
    gotoxy(30, 14); printf("职称: "); safeInput(doc->title, MAX_NAME_LEN, 36, 14);
    gotoxy(30, 16); printf("出诊时间: "); safeInput(doc->workTime, MAX_NAME_LEN, 40, 16);
    doc->next = doctorHead;
    doctorHead = doc;
    setColor(COLOR_SUCCESS);
    gotoxy(30, 18); printf("医生添加成功！");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void listDoctors() {
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== 医 生 列 表 ====================\n\n");
    setColor(COLOR_DEFAULT);
    Doctor* doc = doctorHead;
    if (!doc) { printf("  暂无医生信息。\n"); pressAnyKey(); return; }
    Doctor* tail = doc;
    while(tail->next!=NULL) tail=tail->next;
    const char* headers[] = {"ID", "姓名", "科室ID", "职称", "出诊时间"};
    int widths[] = {6, 10, 8, 12, 20};
    printTableHeader(headers, widths, 5);
    int row = 0;
    while (tail) {
        char idStr[10], deptStr[10];
        sprintf(idStr, "%d", tail->id);
        sprintf(deptStr, "%d", tail->deptId);
        const char* values[] = {idStr, tail->name, deptStr, tail->title, tail->workTime};
        printTableRow(values, widths, 5, (row % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD);
        Doctor* prev = doctorHead;
        if(prev == tail) break;
        while(prev->next!=tail){
            prev=prev->next;
        }
        tail = prev;
        row++;
    }
    pressAnyKey();
}

Doctor* findDoctorById(int id) {
    Doctor* doc = doctorHead;
    while (doc) { if (doc->id == id) return doc; doc = doc->next; }
    return NULL;
}

void listDoctorsByDept(int deptId) {
    Doctor* doc = doctorHead;
    printf("科室%d的医生:\n", deptId);
    while (doc) {
        if (doc->deptId == deptId) printf("  %d. %s (%s)\n", doc->id, doc->name, doc->title);
        doc = doc->next;
    }
}

void updateDoctor() {
    clearScreen();
    drawModernBox(25, 5, 45, 15, " 修 改 医 生 信 息 ");
    int id;
    gotoxy(30, 8); printf("医生ID: "); scanf("%d", &id);
    Doctor *p = findDoctorById(id);
    if (!p) {
        setColor(COLOR_ERROR);
        gotoxy(30, 10); printf("未找到该医生！");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }
    gotoxy(30, 10); printf("新姓名(原:%s): ", p->name);
    safeInput(p->name, MAX_NAME_LEN, 46, 10);
    gotoxy(30, 12); printf("新科室ID(原:%d): ", p->deptId); scanf("%d", &p->deptId);
    gotoxy(30, 14); printf("新职称(原:%s): ", p->title);
    safeInput(p->title, MAX_NAME_LEN, 46, 14);
    gotoxy(30, 16); printf("新出诊时间(原:%s): ", p->workTime);
    safeInput(p->workTime, MAX_NAME_LEN, 50, 16);
    setColor(COLOR_SUCCESS);
    gotoxy(30, 18); printf("修改成功！");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void deleteDoctor() {
    clearScreen();
    drawModernBox(25, 5, 40, 8, " 删 除 医 生 ");
    int id;
    gotoxy(30, 8); printf("医生ID: "); scanf("%d", &id);
    Doctor *d = doctorHead, *prev = NULL;
    while (d) {
        if (d->id == id) {
            if (prev) prev->next = d->next;
            else doctorHead = d->next;
            free(d);
            setColor(COLOR_SUCCESS);
            gotoxy(30, 10); printf("删除成功！");
            setColor(COLOR_DEFAULT);
            pressAnyKey();
            return;
        }
        prev = d;
        d = d->next;
    }
    setColor(COLOR_ERROR);
    gotoxy(30, 10); printf("未找到该医生");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}