// ============================================================
// 文件名: medicine.c
// 描述: 药品管理模块（美化版）
// ============================================================

#include "his.h"

void addMedicine() {
    clearScreen();
    drawModernBox(25, 5, 50, 23, " 添 加 药 品 ");
    Medicine* m = (Medicine*)malloc(sizeof(Medicine));
    gotoxy(30, 8);  printf("药品ID: "); scanf("%d", &m->id);
    gotoxy(30, 10); printf("通用名: "); safeInput(m->commonName, MAX_DRUG_NAME_LEN, 38, 10);
    gotoxy(30, 12); printf("商品名: "); safeInput(m->tradeName, MAX_DRUG_NAME_LEN, 38, 12);
    gotoxy(30, 14); printf("别名: "); safeInput(m->alias, MAX_DRUG_NAME_LEN, 36, 14);
    gotoxy(30, 16); printf("库存: "); scanf("%d", &m->stock);
    gotoxy(30, 18); printf("单价: "); scanf("%f", &m->price);
    gotoxy(30, 20); printf("关联科室ID: "); scanf("%d", &m->deptId);
    gotoxy(30, 22); printf("单位(盒/瓶): "); safeInput(m->unit, 10, 44, 22);
    gotoxy(30, 24); printf("有效期(月-日): "); safeInput(m->expireDate, 20, 46, 24);
    m->next = medicineHead;
    medicineHead = m;
    setColor(COLOR_SUCCESS);
    gotoxy(30, 26); printf("药品添加成功。");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void listMedicines() {
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== 药 品 库 存 列 表 ====================\n\n");
    setColor(COLOR_DEFAULT);
    Medicine* m = medicineHead;
    if (!m) { printf("  暂无药品信息。\n"); pressAnyKey(); return; }
    Medicine* tail = m;
    while(tail->next!=NULL) tail=tail->next;
    const char* headers[] = {"ID", "通用名", "库存", "单价", "科室ID", "单位", "有效期"};
    int widths[] = {6, 15, 8, 8, 8, 8, 10};
    printTableHeader(headers, widths, 7);
    int row = 0;
    while (tail) {
        char idStr[10], stockStr[10], priceStr[15], deptStr[10];
        sprintf(idStr, "%d", tail->id);
        sprintf(stockStr, "%d", tail->stock);
        sprintf(priceStr, "%.2f", tail->price);
        sprintf(deptStr, "%d", tail->deptId);
        const char* values[] = {idStr, tail->commonName, stockStr, priceStr, deptStr, tail->unit, tail->expireDate};
        int color = (row % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD;
        if (tail->stock < 20) color = COLOR_WARNING;
        printTableRow(values, widths, 7, color);
        Medicine* prev = medicineHead;
        if(prev == tail) break;
        while(prev->next!=tail){
            prev=prev->next;
        }
        tail = prev;
        row++;
    }
    pressAnyKey();
}

Medicine* findMedicineById(int id) {
    Medicine* m = medicineHead;
    while (m) { if (m->id == id) return m; m = m->next; }
    return NULL;
}

void updateMedicineStock() {
    clearScreen();
    drawModernBox(25, 5, 40, 11, " 库 存 变 更 ");
    int id, amount, type;
    gotoxy(30, 8);  printf("药品ID: "); scanf("%d", &id);
    Medicine* m = findMedicineById(id);
    if (!m) { setColor(COLOR_ERROR); gotoxy(30, 10); printf("药品不存在。"); setColor(COLOR_DEFAULT); pressAnyKey(); return; }
    gotoxy(30, 10); printf("1.入库 2.出库: "); scanf("%d", &type);
    gotoxy(30, 12); printf("数量: "); scanf("%d", &amount);
    if (type == 1) {
        m->stock += amount;
        setColor(COLOR_SUCCESS);
        gotoxy(30, 14); printf("入库成功，当前库存:%d", m->stock);
    } else if (type == 2) {
        if (m->stock < amount) {
            setColor(COLOR_ERROR);
            gotoxy(30, 14); printf("库存不足！");
        } else {
            m->stock -= amount;
            setColor(COLOR_SUCCESS);
            gotoxy(30, 14); printf("出库成功，剩余库存:%d", m->stock);
        }
    }
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void deleteMedicine() {
    clearScreen();
    drawModernBox(25, 5, 40, 8, " 删 除 药 品 ");
    int id;
    gotoxy(30, 8); printf("药品ID: "); scanf("%d", &id);
    Medicine *m = medicineHead, *prev = NULL;
    while (m) {
        if (m->id == id) {
            if (prev) prev->next = m->next;
            else medicineHead = m->next;
            free(m);
            setColor(COLOR_SUCCESS);
            gotoxy(30, 10); printf("药品已删除。");
            setColor(COLOR_DEFAULT);
            pressAnyKey();
            return;
        }
        prev = m;
        m = m->next;
    }
    setColor(COLOR_ERROR);
    gotoxy(30, 10); printf("未找到该药品。");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}