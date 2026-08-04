// ============================================================
// 文件名: prescription.c
// 描述: 处方与发药管理（美化版）
// ============================================================

#include "his.h"

void addPrescription() {
    clearScreen();
    drawModernBox(25, 5, 50, 21, " 开 立 处 方 ");
    Prescription* pre = (Prescription*)malloc(sizeof(Prescription));
    gotoxy(30, 8);  printf("处方ID: "); scanf("%d", &pre->id);
    gotoxy(30, 10); printf("关联医疗记录ID: "); scanf("%d", &pre->recordId);
    gotoxy(30, 12); printf("患者ID: "); scanf("%d", &pre->patientId);
    gotoxy(30, 14); printf("医生ID: "); scanf("%d", &pre->doctorId);
    strcpy(pre->issueTime, getCurrentTime());
    pre->items = NULL;
    int more = 1;
    while (more) {
        PrescriptionItem* item = (PrescriptionItem*)malloc(sizeof(PrescriptionItem));
        gotoxy(30, 16); printf("药品ID: "); scanf("%d", &item->medicineId);
        gotoxy(30, 18); printf("数量: "); scanf("%d", &item->quantity);
        Medicine* med = findMedicineById(item->medicineId);
        if (!med || med->stock < item->quantity) {
            setColor(COLOR_ERROR);
            gotoxy(30, 20); printf("药品库存不足，取消该药品。");
            setColor(COLOR_DEFAULT);
            free(item);
        } else {
            med->stock -= item->quantity;
            item->next = pre->items;
            pre->items = item;
        }
        gotoxy(30, 22); printf("继续添加? 1是/0否: "); scanf("%d", &more);
        clearScreen();
        drawModernBox(25, 5, 50, 20, " 开 立 处 方 ");
    }
    pre->next = prescriptionHead;
    prescriptionHead = pre;
    setColor(COLOR_SUCCESS);
    gotoxy(30, 20); printf("处方开立成功，药品库存已扣减。");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void listPrescriptions() {
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== 处 方 列 表 ====================\n\n");
    setColor(COLOR_DEFAULT);
    Prescription* p = prescriptionHead;
    if (!p) { printf("  暂无处方记录。\n"); pressAnyKey(); return; }
    Prescription* tail = p;
    while(tail->next!=NULL) tail=tail->next;
    const char* headers[] = {"处方ID", "记录ID", "患者ID", "医生ID", "时间"};
    int widths[] = {8, 8, 8, 8, 20};
    printTableHeader(headers, widths, 5);
    int row = 0;
    while (tail) {
        char idStr[10], ridStr[10], pidStr[10], didStr[10];
        sprintf(idStr, "%d", tail->id);
        sprintf(ridStr, "%d", tail->recordId);
        sprintf(pidStr, "%d", tail->patientId);
        sprintf(didStr, "%d", tail->doctorId);
        const char* values[] = {idStr, ridStr, pidStr, didStr, tail->issueTime};
        printTableRow(values, widths, 5, (row % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD);
        Prescription* prev = prescriptionHead;
        if(prev == tail) break;
        while(prev->next!=tail){
            prev=prev->next;
        }
        tail = prev;
        row++;
    }
    pressAnyKey();
}

Prescription* findPrescriptionById(int id) {
    Prescription* p = prescriptionHead;
    while (p) { if (p->id == id) return p; p = p->next; }
    return NULL;
}

void showPrescriptionDetail(Prescription* p) {
    if (!p) return;
    printf("处方详情: ID=%d, 记录ID=%d, 患者ID=%d, 医生ID=%d\n", p->id, p->recordId, p->patientId, p->doctorId);
    PrescriptionItem* item = p->items;
    while (item) {
        Medicine* m = findMedicineById(item->medicineId);
        printf("  药品:%s 数量:%d\n", m?m->commonName:"未知", item->quantity);
        item = item->next;
    }
}