// ============================================================
// 文件名: medical_record.c
// 描述: 医疗记录管理模块（美化版）
// ============================================================

#include "his.h"

void addMedicalRecord() {
    clearScreen();
    drawModernBox(25, 5, 50, 15, " 添 加 医 疗 记 录 ");
    MedicalRecord* rec = (MedicalRecord*)malloc(sizeof(MedicalRecord));
    gotoxy(30, 8);  printf("记录ID: "); scanf("%d", &rec->id);
    gotoxy(30, 10); printf("患者ID: "); scanf("%d", &rec->patientId);
    gotoxy(30, 12); printf("医生ID: "); scanf("%d", &rec->doctorId);
    gotoxy(30, 14); printf("记录类型(挂号/看诊/检查/住院): "); safeInput(rec->recordType, 20, 56, 14);
    gotoxy(30, 16); printf("诊断/详情: "); safeInput(rec->diagnosis, MAX_DESC_LEN, 40, 16);
    strcpy(rec->recordTime, getCurrentTime());
    strcpy(rec->appointmentTime, "");
    rec->queueNumber = 0;
    rec->status = 1;
    rec->next = recordHead;
    recordHead = rec;
    setColor(COLOR_SUCCESS);
    gotoxy(30, 18); printf("医疗记录添加成功！");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void listMedicalRecords() {
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== 医 疗 记 录 列 表 ====================\n\n");
    setColor(COLOR_DEFAULT);
    MedicalRecord* r = recordHead;
    if (!r) { printf("  暂无医疗记录。\n"); pressAnyKey(); return; }
    MedicalRecord* tail = r;
    while(tail->next!=NULL) tail=tail->next;
    const char* headers[] = {"ID", "患者ID", "医生ID", "类型", "排队号", "状态", "时间", "诊断"};
    int widths[] = {6, 8, 8, 10, 8, 8, 20, 20};
    printTableHeader(headers, widths, 8);
    int row = 0;
    while (tail) {
        char idStr[10], pidStr[10], didStr[10], queueStr[10];
        sprintf(idStr, "%d", tail->id);
        sprintf(pidStr, "%d", tail->patientId);
        sprintf(didStr, "%d", tail->doctorId);
        sprintf(queueStr, "%d", tail->queueNumber);
        const char* statusStr = (tail->status==0?"待诊":(tail->status==1?"完成":"爽约"));
        const char* values[] = {idStr, pidStr, didStr, tail->recordType, queueStr, statusStr, tail->recordTime, tail->diagnosis};
        printTableRow(values, widths, 8, (row % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD);
        MedicalRecord* prev = recordHead;
        if(prev == tail) break;
        while(prev->next!=tail){
            prev=prev->next;
        }
        tail = prev;
        row++;
    }
    pressAnyKey();
}

MedicalRecord* findRecordById(int id) {
    MedicalRecord* r = recordHead;
    while (r) { if (r->id == id) return r; r = r->next; }
    return NULL;
}

void listRecordsByPatient(int patientId) {
    MedicalRecord* r = recordHead;
    printf("患者ID %d 的医疗记录:\n", patientId);
    while (r) {
        if (r->patientId == patientId)
            printf("  [%s] %s 医生ID:%d 诊断:%s\n", r->recordTime, r->recordType, r->doctorId, r->diagnosis);
        r = r->next;
    }
}