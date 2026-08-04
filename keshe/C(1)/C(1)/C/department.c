// ============================================================
// 文件名: department.c
// 描述: 科室管理模块（美化版）
// ============================================================

#include "his.h"

void addDepartment() {
    clearScreen();
    drawModernBox(25, 5, 40, 13, " 添 加 科 室 ");
    Department* d = (Department*)malloc(sizeof(Department));
    gotoxy(30, 8);  printf("科室ID: "); scanf("%d", &d->id);
    gotoxy(30, 10); printf("科室名称: "); safeInput(d->name, MAX_NAME_LEN, 40, 10);
    gotoxy(30, 12); printf("关联病房信息: "); scanf("%d", &d->wardType);
    gotoxy(30, 14); printf("负责人工号: "); scanf("%d", &d->leaderId);
    d->next = deptHead;
    deptHead = d;
    setColor(COLOR_SUCCESS);
    gotoxy(30, 16); printf("科室添加成功！");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void listDepartments() {
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== 科 室 列 表 ====================\n\n");
    setColor(COLOR_DEFAULT);
    Department* d = deptHead;
    if (!d) { printf("  暂无科室信息。\n"); pressAnyKey(); return; }
    Department* tail = d;
    while(tail->next!=NULL) tail=tail->next;
    const char* headers[] = {"ID", "名称", "病房类型", "负责人ID"};
    int widths[] = {6, 15, 10, 10};
    printTableHeader(headers, widths, 4);
    int row = 0;
    while (tail) {
        char idStr[10], wardStr[10], leaderStr[10];
        sprintf(idStr, "%d", tail->id);
        sprintf(wardStr, "%d", tail->wardType);
        sprintf(leaderStr, "%d", tail->leaderId);
        const char* values[] = {idStr, tail->name, wardStr, leaderStr};
        printTableRow(values, widths, 4, (row % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD);
        Department* prev = deptHead;
        if(prev == tail) break;
        while(prev->next!=tail){
            prev=prev->next;
        }
        tail = prev;
        row++;
    }
    pressAnyKey();
}

Department* findDepartmentById(int id) {
    Department* d = deptHead;
    while (d) { if (d->id == id) return d; d = d->next; }
    return NULL;
}

void deleteDepartment() {
    clearScreen();
    drawModernBox(25, 5, 40, 8, " 删 除 科 室 ");
    int id;
    gotoxy(30, 8); printf("科室ID: "); scanf("%d", &id);
    Department *d = deptHead, *prev = NULL;
    while (d) {
        if (d->id == id) {
            if (prev) prev->next = d->next;
            else deptHead = d->next;
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
    gotoxy(30, 10); printf("未找到该科室");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void updateDepartment() {
    clearScreen();
    drawModernBox(25, 5, 45, 13, " 修 改 科 室 信 息 ");
    int id;
    gotoxy(30, 8); printf("科室ID: "); scanf("%d", &id);
    Department *p = findDepartmentById(id);
    if (!p) {
        setColor(COLOR_ERROR);
        gotoxy(30, 10); printf("未找到该科室！");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }
    gotoxy(30, 10); printf("新名称(原:%s): ", p->name);
    safeInput(p->name, MAX_NAME_LEN, 46, 10);
    gotoxy(30, 12); printf("新病房信息(原:%d): ", p->wardType); scanf("%d", &p->wardType);
    gotoxy(30, 14); printf("新负责人(原:%d): ", p->leaderId); scanf("%d", &p->leaderId);
    setColor(COLOR_SUCCESS);
    gotoxy(30, 16); printf("修改成功！");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}