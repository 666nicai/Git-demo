// ============================================================
// 文件名: search.c
// 描述: 查询功能模块（美化版）
// ============================================================

#include "his.h"

void searchPatientByName() {
    clearScreen();
    drawModernBox(25, 5, 45, 8, " 查 询 患 者 ");
    char name[MAX_NAME_LEN];
    gotoxy(30, 8); printf("请输入患者姓名: "); safeInput(name, MAX_NAME_LEN, 46, 8);
    Patient* p = patientHead;
    int found = 0;
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== 查 询 结 果 ====================\n\n");
    setColor(COLOR_DEFAULT);
    while (p) {
        if (strstr(p->name, name) != NULL) {
            printf("  ID:%d  %s  性别:%c  年龄:%d  电话:%s\n", p->id, p->name, p->gender, p->age, p->phone);
            found = 1;
        }
        p = p->next;
    }
    if (!found) printf("  未找到匹配患者。\n");
    pressAnyKey();
}

void searchDoctorByName() {
    clearScreen();
    drawModernBox(25, 5, 45, 8, " 查 询 医 生 ");
    char name[MAX_NAME_LEN];
    gotoxy(30, 8); printf("请输入医生姓名: "); safeInput(name, MAX_NAME_LEN, 46, 8);
    Doctor* d = doctorHead;
    int found = 0;
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== 查 询 结 果 ====================\n\n");
    setColor(COLOR_DEFAULT);
    while (d) {
        if (strstr(d->name, name) != NULL) {
            printf("  ID:%d  姓名:%s  科室ID:%d  职称:%s\n", d->id, d->name, d->deptId, d->title);
            found = 1;
        }
        d = d->next;
    }
    if (!found) printf("  未找到匹配医生。\n");
    pressAnyKey();
}

void searchMedicineByName() {
    clearScreen();
    drawModernBox(25, 5, 50, 8, " 查 询 药 品 ");
    char name[MAX_DRUG_NAME_LEN];
    gotoxy(30, 8); printf("请输入药品名: "); safeInput(name, MAX_DRUG_NAME_LEN, 44, 8);
    Medicine* m = medicineHead;
    int found = 0;
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== 查 询 结 果 ====================\n\n");
    setColor(COLOR_DEFAULT);
    while (m) {
        if (strstr(m->commonName, name) || strstr(m->tradeName, name) || strstr(m->alias, name)) {
            printf("  ID:%d  通用名:%s  库存:%d  单价:%.2f\n", m->id, m->commonName, m->stock, m->price);
            found = 1;
        }
        m = m->next;
    }
    if (!found) printf("  未找到匹配药品。\n");
    pressAnyKey();
}