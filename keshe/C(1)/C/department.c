// ============================================================
// 文件名: department.c
// 描述: 科室管理模块（含权限控制）
//       管理员可增删改查；医生仅可查看信息。
//       修改、删除仅管理员可用。
// ============================================================

#include "his.h"

// 外部变量：当前用户角色
extern UserRole currentUserRole;

// ======================= 添加科室（仅管理员） =======================
void addDepartment() {
    // 权限检查：仅管理员可添加科室
    if (currentUserRole != ROLE_ADMIN) {
        clearScreen();
        setColor(COLOR_ERROR);
        printf("\n  权限不足，仅管理员可添加科室。\n");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }

    clearScreen();
    drawModernBox(25, 5, 40, 13, " 添 加 科 室 ");
    Department* d = (Department*)malloc(sizeof(Department));
    gotoxy(30, 8);  printf("科室ID: "); scanf("%d", &d->id);
    gotoxy(30, 10); printf("科室名称: "); safeInput(d->name, 20, 40, 10);
    gotoxy(30, 12); printf("关联病房信息: "); scanf("%d", &d->wardType);
    gotoxy(30, 14); printf("负责人工号: "); scanf("%d", &d->leaderId);
    d->next = deptHead;
    deptHead = d;
    setColor(COLOR_SUCCESS);
    gotoxy(30, 16); printf("科室添加成功！");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

// ======================= 查看所有科室（所有角色可查看） =======================
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

// ======================= 查找科室 =======================
Department* findDepartmentById(int id) {
    Department* d = deptHead;
    while (d) { if (d->id == id) return d; d = d->next; }
    return NULL;
}

// ======================= 修改科室（仅管理员） =======================
void updateDepartment() {
    // 权限检查：仅管理员可修改科室信息
    if (currentUserRole != ROLE_ADMIN) {
        clearScreen();
        setColor(COLOR_ERROR);
        printf("\n  权限不足，仅管理员可修改科室信息。\n");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }

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
    safeInput(p->name, 20, 46, 10);
    gotoxy(30, 12); printf("新病房信息(原:%d): ", p->wardType); scanf("%d", &p->wardType);
    gotoxy(30, 14); printf("新负责人(原:%d): ", p->leaderId); scanf("%d", &p->leaderId);
    setColor(COLOR_SUCCESS);
    gotoxy(30, 16); printf("修改成功！");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

// ======================= 删除科室（仅管理员） =======================
void deleteDepartment() {
    // 权限检查：仅管理员可删除科室
    if (currentUserRole != ROLE_ADMIN) {
        clearScreen();
        setColor(COLOR_ERROR);
        printf("\n  权限不足，仅管理员可删除科室。\n");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }

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

// ============================================================
// 医生专用：查看自己科室详情
// ============================================================
void showMyDeptDetails(int doctorId) {
    Doctor* doc = findDoctorById(doctorId);
    if (!doc) {
        setColor(COLOR_ERROR);
        printf("医生信息不存在！\n");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }
    int deptId = doc->deptId;
    Department* dept = findDepartmentById(deptId);
    if (!dept) {
        setColor(COLOR_ERROR);
        printf("您所属的科室信息不存在！\n");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }

    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ========== 我 的 科 室 详 情 ==========\n\n");
    setColor(COLOR_DEFAULT);
    printf("  科室ID: %d\n", dept->id);
    printf("  科室名称: %s\n", dept->name);
    printf("  病房类型: %d\n", dept->wardType);

    Doctor* leader = findDoctorById(dept->leaderId);
    printf("  负责人工号: %d", dept->leaderId);
    if (leader) printf(" (姓名: %s)", leader->name);
    printf("\n\n");

    printf("  科室医生列表:\n");
    Doctor* temp = doctorHead;
    int hasDoctor = 0;
    while (temp) {
        if (temp->deptId == deptId) {
            printf("    %d. %s (%s)\n", temp->id, temp->name, temp->title);
            hasDoctor = 1;
        }
        temp = temp->next;
    }
    if (!hasDoctor) printf("    (暂无医生)\n");
    pressAnyKey();
}