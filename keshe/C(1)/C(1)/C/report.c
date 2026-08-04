// ============================================================
// 文件名: report.c
// 描述: 统计报表模块（美化版）
// ============================================================

#include "his.h"

void reportPatientStats() {
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== 患 者 统 计 ====================\n\n");
    setColor(COLOR_DEFAULT);
    int total = 0, outpatient = 0, inpatient = 0, male=0, female=0;
    Patient* p = patientHead;
    while (p) {
        total++;
        if (p->type == 0) outpatient++;
        else inpatient++;
        if (p->gender == 'M') male++;
        else female++;
        p = p->next;
    }
    printf("  总患者数: %d\n", total);
    printf("  门诊患者: %d\n", outpatient);
    printf("  住院患者: %d\n", inpatient);
    printf("  男性: %d\n", male);
    printf("  女性: %d\n", female);
}

void reportDoctorWorkload() {
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== 医 生 工 作 量 ====================\n\n");
    setColor(COLOR_DEFAULT);
    int workload[100] = {0};
    MedicalRecord* r = recordHead;
    while (r) {
        if (strcmp(r->recordType, "看诊") == 0 && r->doctorId >=1 && r->doctorId <= 100)
            workload[r->doctorId-1]++;
        r = r->next;
    }
    const char* headers[] = {"医生ID", "姓名", "看诊次数"};
    int widths[] = {8, 12, 10};
    printTableHeader(headers, widths, 3);
    Doctor* d = doctorHead;
    Doctor* tail = d;
    while(tail->next!=NULL) tail=tail->next;
    int row = 0;
    while (tail) {
        char idStr[10], countStr[10];
        sprintf(idStr, "%d", tail->id);
        sprintf(countStr, "%d", workload[tail->id-1]);
        const char* values[] = {idStr, tail->name, countStr};
        printTableRow(values, widths, 3, (row % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD);
        Doctor* prev = doctorHead;
        if(prev==tail) break;
        while(prev->next!=tail) prev=prev->next;
        tail=prev;
        row++;
    }
}

void reportDeptStats() {
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== 科 室 就 诊 统 计 ====================\n\n");
    setColor(COLOR_DEFAULT);
    int deptCount[10] = {0};
    MedicalRecord* r = recordHead;
    while (r) {
        Doctor* d = findDoctorById(r->doctorId);
        if (d) deptCount[d->deptId-1]++;
        r = r->next;
    }
    const char* headers[] = {"科室ID", "科室名称", "就诊次数"};
    int widths[] = {8, 15, 10};
    printTableHeader(headers, widths, 3);
    Department* dept = deptHead;
    Department* tail = dept;
    while(tail->next!=NULL) tail=tail->next;
    int row = 0;
    while (tail) {
        char idStr[10], countStr[10];
        sprintf(idStr, "%d", tail->id);
        sprintf(countStr, "%d", deptCount[tail->id-1]);
        const char* values[] = {idStr, tail->name, countStr};
        printTableRow(values, widths, 3, (row % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD);
        Department* prev = deptHead;
        if(prev == tail) break;
        while(prev->next!=tail){
            prev=prev->next;
        }
        tail = prev;
        row++;
    }
}

void reportMedicineStock() {
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== 药 品 库 存 报 表 ====================\n\n");
    setColor(COLOR_DEFAULT);
    const char* headers[] = {"ID", "通用名", "库存", "状态"};
    int widths[] = {6, 15, 8, 10};
    printTableHeader(headers, widths, 4);
    Medicine* m = medicineHead;
    Medicine* tail = m;
    while(tail->next!=NULL) tail=tail->next;
    int row = 0;
    while (tail) {
        char idStr[10], stockStr[10];
        sprintf(idStr, "%d", tail->id);
        sprintf(stockStr, "%d", tail->stock);
        const char* status = (tail->stock < 20) ? "库存不足" : "正常";
        const char* values[] = {idStr, tail->commonName, stockStr, status};
        int color = (row % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD;
        if (tail->stock < 20) color = COLOR_WARNING;
        printTableRow(values, widths, 4, color);
        Medicine* prev = medicineHead;
        if(prev == tail) break;
        while(prev->next!=tail){
            prev=prev->next;
        }
        tail = prev;
        row++;
    }
}

void reportWardUtilization() {
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== 病 房 利 用 率 ====================\n\n");
    setColor(COLOR_DEFAULT);
    const char* headers[] = {"病房ID", "类型", "总床位", "已用", "利用率"};
    int widths[] = {8, 12, 8, 6, 10};
    printTableHeader(headers, widths, 5);
    Ward* w = wardHead;
    Ward* tail = w;
    int row = 0;
    while (tail) {
        int used = 0;
        Bed* b = tail->bedList;
        while (b) { if (b->status == 1) used++; b = b->next; }
        float rate = (tail->totalBeds > 0) ? (used*100.0/tail->totalBeds) : 0;
        char idStr[10], totalStr[10], usedStr[10], rateStr[15];
        sprintf(idStr, "%d", tail->id);
        sprintf(totalStr, "%d", tail->totalBeds);
        sprintf(usedStr, "%d", used);
        sprintf(rateStr, "%.1f%%", rate);
        const char* values[] = {idStr, tail->type, totalStr, usedStr, rateStr};
        int color = (row % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD;
        if (rate >= 90) color = COLOR_WARNING;
        printTableRow(values, widths, 5, color);
        Ward* prev = wardHead;
        if(prev == tail) break;
        while(prev->next!=tail){
            prev=prev->next;
        }
        tail = prev;
        row++;
    }
}

void reportComprehensive() {
    reportPatientStats(); printf("\n"); pressAnyKey();
    reportWardUtilization(); printf("\n"); pressAnyKey();
    reportMedicineStock();
}

void reportPatientView() {
    clearScreen();
    drawModernBox(25, 5, 50, 8, " 患 者 病 历 查 询 ");
    int pid;
    gotoxy(30, 8); printf("请输入患者ID: "); scanf("%d", &pid);
    Patient* p = findPatientById(pid);
    if (!p) { setColor(COLOR_ERROR); gotoxy(30, 10); printf("患者不存在。"); setColor(COLOR_DEFAULT); pressAnyKey(); return; }
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== 患 者 病 历 摘 要 ====================\n\n");
    setColor(COLOR_DEFAULT);
    printf("  姓名:%s  性别:%c  年龄:%d  电话:%s  类型:%s\n", p->name, p->gender, p->age, p->phone, (p->type==0?"门诊":"住院"));
    if (p->type == 1) printf("  住院床位号:%d\n", p->bedId);
    printf("\n  医疗记录:\n");
    MedicalRecord* r = recordHead;
    while (r) {
        if (r->patientId == pid)
            printf("    [%s] %s 医生ID:%d 诊断:%s\n", r->recordTime, r->recordType, r->doctorId, r->diagnosis);
        r = r->next;
    }
    pressAnyKey();
}