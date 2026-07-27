// ============================================================
// 文件名: ward.c
// 描述: 病房与床位管理模块（含权限控制）
//       管理员可增删改查病房；医生仅可查看和办理入院/出院。
//       添加病房功能仅管理员可用。
// ============================================================

#include "his.h"

// 外部变量：当前用户角色及医生ID
extern UserRole currentUserRole;
extern int currentDoctorId;

// ======================= 添加病房（仅管理员） =======================
void addWard() {
    // 权限检查：仅管理员可添加病房
    if (currentUserRole != ROLE_ADMIN) {
        clearScreen();
        setColor(COLOR_ERROR);
        printf("\n  权限不足，仅管理员可添加病房。\n");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }

    clearScreen();
    drawModernBox(25, 5, 40, 13, " 添 加 病 房 ");
    Ward* w = (Ward*)malloc(sizeof(Ward));
    gotoxy(30, 8);  printf("病房ID: "); scanf("%d", &w->id);
    gotoxy(30, 10); printf("病房类型: "); safeInput(w->type, 20, 40, 10);
    gotoxy(30, 12); printf("关联科室ID: "); scanf("%d", &w->deptId);
    gotoxy(30, 14); printf("总床位数: "); scanf("%d", &w->totalBeds);
    w->bedList = NULL;
    for (int i = 1; i <= w->totalBeds; i++) {
        Bed* b = (Bed*)malloc(sizeof(Bed));
        b->bedId = (w->id-1)*100 + i;
        b->wardId = w->id;
        b->patientId = 0;
        b->status = 0;
        b->next = w->bedList;
        w->bedList = b;
    }
    w->next = wardHead;
    wardHead = w;
    setColor(COLOR_SUCCESS);
    gotoxy(30, 16); printf("病房添加成功！");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

// ======================= 查看所有病房（所有角色可查看） =======================
void listWards() {
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== 病 房 列 表 ====================\n\n");
    setColor(COLOR_DEFAULT);
    Ward* w = wardHead;
    if (!w) { printf("  暂无病房信息。\n"); pressAnyKey(); return; }
    Ward* tail = w;
    while(tail->next!=NULL) tail=tail->next;
    const char* headers[] = {"病房ID", "类型", "科室ID", "总床位", "空闲床位"};
    int widths[] = {8, 12, 8, 8, 10};
    printTableHeader(headers, widths, 5);
    int row = 0;
    while (tail) {
        int freeCount = 0;
        Bed* b = tail->bedList;
        while (b) { if (b->status == 0) freeCount++; b = b->next; }
        char idStr[10], deptStr[10], totalStr[10], freeStr[10];
        sprintf(idStr, "%d", tail->id);
        sprintf(deptStr, "%d", tail->deptId);
        sprintf(totalStr, "%d", tail->totalBeds);
        sprintf(freeStr, "%d", freeCount);
        const char* values[] = {idStr, tail->type, deptStr, totalStr, freeStr};
        printTableRow(values, widths, 5, (row % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD);
        Ward* prev = wardHead;
        if(prev == tail) break;
        while(prev->next!=tail){
            prev=prev->next;
        }
        tail = prev;
        row++;
    }
    printf("\n按任意键查看详细床位信息...");
    pressAnyKey();
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== 床 位 详 情 ====================\n\n");
    w = wardHead;
    while (w) {
        printf("病房ID: %d  类型: %s\n", w->id, w->type);
        Bed* b = w->bedList;
        while (b) {
            printf("  床位%d: ", b->bedId);
            if (b->status == 0) printf("空闲");
            else if (b->status == 1) printf("占用(患者ID:%d)", b->patientId);
            else printf("维修中");
            printf("\n");
            b = b->next;
        }
        w = w->next;
    }
    pressAnyKey();
}

// ======================= 查找空闲床位 =======================
Bed* findFreeBed(int wardId) {
    Ward* w = wardHead;
    while (w && w->id != wardId) w = w->next;
    if (!w) return NULL;
    Bed* b = w->bedList;
    while (b) { if (b->status == 0) return b; b = b->next; }
    return NULL;
}

// ======================= 分配床位 =======================
int allocateBed(int patientId, int wardId) {
    Bed* b = findFreeBed(wardId);
    if (!b) return 0;
    b->status = 1;
    b->patientId = patientId;
    return b->bedId;
}

// ======================= 释放床位 =======================
void freeBed(int bedId) {
    Ward* w = wardHead;
    while (w) {
        Bed* b = w->bedList;
        while (b) {
            if (b->bedId == bedId) { b->status = 0; b->patientId = 0; return; }
            b = b->next;
        }
        w = w->next;
    }
}

// ======================= 床位管理主菜单（入院/出院） =======================
void manageBeds() {
    clearScreen();
    drawModernBox(25, 5, 40, 15, " 床 位 管 理 ");
    int choice;
    gotoxy(30, 8);  printf("1. 分配床位");
    gotoxy(30, 10); printf("2. 出院释放床位");
    gotoxy(30, 12); printf("请选择: ");
    scanf("%d", &choice);
    if (choice == 1) {
        int pid, wid;
        gotoxy(30, 14); printf("患者ID: "); scanf("%d", &pid);

        // 医生角色：列出本科室的病房供选择
        if (currentUserRole == ROLE_DOCTOR) {
            Doctor* doc = findDoctorById(currentDoctorId);
            if (!doc) {
                setColor(COLOR_ERROR);
                gotoxy(30, 16); printf("医生信息不存在！");
                setColor(COLOR_DEFAULT);
                pressAnyKey();
                return;
            }
            int deptId = doc->deptId;
            printf("\n  本科室 (%s) 的病房列表:\n", findDepartmentById(deptId)->name);
            Ward* w = wardHead;
            int found = 0;
            while (w) {
                if (w->deptId == deptId) {
                    printf("    病房ID: %d  类型: %s  空闲床位: ", w->id, w->type);
                    int freeCnt = 0;
                    Bed* b = w->bedList;
                    while (b) { if(b->status==0) freeCnt++; b = b->next; }
                    printf("%d\n", freeCnt);
                    found = 1;
                }
                w = w->next;
            }
            if (!found) {
                setColor(COLOR_ERROR);
                gotoxy(30, 16); printf("本科室暂无病房！");
                setColor(COLOR_DEFAULT);
                pressAnyKey();
                return;
            }
            gotoxy(30, 16); printf("请输入病房ID: ");
            scanf("%d", &wid);
            // 校验是否属于本科室
            Ward* selected = wardHead;
            while (selected && selected->id != wid) selected = selected->next;
            if (!selected || selected->deptId != deptId) {
                setColor(COLOR_ERROR);
                gotoxy(30, 18); printf("无权分配其他科室的病房！");
                setColor(COLOR_DEFAULT);
                pressAnyKey();
                return;
            }
        } else {
            gotoxy(30, 16); printf("病房ID: "); scanf("%d", &wid);
        }

        int bid = allocateBed(pid, wid);
        if (bid > 0) {
            Patient* p = findPatientById(pid);
            if (p) p->bedId = bid;
            setColor(COLOR_SUCCESS);
            gotoxy(30, 18); printf("分配成功，床位号:%d", bid);
        } else {
            setColor(COLOR_ERROR);
            gotoxy(30, 18); printf("分配失败，无空闲床位。");
        }
    } else if (choice == 2) {
        int bid;
        gotoxy(30, 14); printf("床位ID: "); scanf("%d", &bid);
        // 医生角色需验证释放的床位是否属于本科室
        if (currentUserRole == ROLE_DOCTOR) {
            Ward* w = wardHead;
            int deptId = findDoctorById(currentDoctorId)->deptId;
            int valid = 0;
            while (w) {
                if (w->deptId == deptId) {
                    Bed* b = w->bedList;
                    while (b) {
                        if (b->bedId == bid) { valid = 1; break; }
                        b = b->next;
                    }
                }
                if (valid) break;
                w = w->next;
            }
            if (!valid) {
                setColor(COLOR_ERROR);
                gotoxy(30, 16); printf("无权释放其他科室的床位！");
                setColor(COLOR_DEFAULT);
                pressAnyKey();
                return;
            }
        }
        freeBed(bid);
        setColor(COLOR_SUCCESS);
        gotoxy(30, 16); printf("床位已释放。");
    }
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}