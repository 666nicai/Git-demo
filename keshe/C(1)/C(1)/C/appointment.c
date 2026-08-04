// ============================================================
// 文件名: appointment.c
// 描述: 预约挂号、候诊队列、医生叫号模块（美化版）
// ============================================================

#include "his.h"

static int getMaxQueueNumber(int doctorId) {
    int maxNum = 0;
    MedicalRecord* r = recordHead;
    while (r) {
        if (r->doctorId == doctorId && r->status == 0)
            if (r->queueNumber > maxNum) maxNum = r->queueNumber;
        r = r->next;
    }
    return maxNum;
}

static int getNextRecordId() {
    int maxId = 0;
    MedicalRecord* r = recordHead;
    while (r) { if (r->id > maxId) maxId = r->id; r = r->next; }
    return maxId + 1;
}

void registerAppointment() {
    clearScreen();
    drawModernBox(25, 5, 50, 16, " 门 诊 挂 号 ");
    MedicalRecord* rec = (MedicalRecord*)malloc(sizeof(MedicalRecord));
    int patientId, doctorId, choice;
    gotoxy(30, 8); printf("患者ID: "); scanf("%d", &patientId);
    Patient* p = findPatientById(patientId);
    if (!p) { setColor(COLOR_ERROR); gotoxy(30, 10); printf("患者不存在！"); setColor(COLOR_DEFAULT); free(rec); pressAnyKey(); return; }
    gotoxy(30, 10); printf("挂号方式：1.预约挂号  2.现场挂号: "); scanf("%d", &choice);
    if (choice == 1) strcpy(rec->recordType, "预约挂号");
    else strcpy(rec->recordType, "现场挂号");
    gotoxy(30, 12); printf("医生ID（0则仅选科室）: "); scanf("%d", &doctorId);
    if (doctorId == 0) {
        int deptId;
        gotoxy(30, 14); printf("科室ID: "); scanf("%d", &deptId);
        Doctor* d = doctorHead;
        while (d) { if (d->deptId == deptId) { doctorId = d->id; break; } d = d->next; }
        if (doctorId == 0) { setColor(COLOR_ERROR); gotoxy(30, 16); printf("该科室暂无医生！"); setColor(COLOR_DEFAULT); free(rec); pressAnyKey(); return; }
    }
    Doctor* doc = findDoctorById(doctorId);
    if (!doc) { setColor(COLOR_ERROR); gotoxy(30, 14); printf("医生不存在！"); setColor(COLOR_DEFAULT); free(rec); pressAnyKey(); return; }
    rec->id = getNextRecordId();
    rec->patientId = patientId;
    rec->doctorId = doctorId;
    strcpy(rec->recordTime, getCurrentTime());
    if (choice == 1) { gotoxy(30, 14); printf("预约日期(YYYY-MM-DD): "); safeInput(rec->appointmentTime, 30, 48, 14); }
    else strcpy(rec->appointmentTime, "今日就诊");
    rec->queueNumber = getMaxQueueNumber(doctorId) + 1;
    rec->status = 0;
    strcpy(rec->diagnosis, "");
    rec->next = recordHead;
    recordHead = rec;
    setColor(COLOR_SUCCESS);
    gotoxy(30, 18); printf("挂号成功！记录ID:%d 排队号:%d 医生:%s", rec->id, rec->queueNumber, doc->name);
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void listQueueByDoctor() {
    clearScreen();
    drawModernBox(25, 5, 45, 8, " 查 看 候 诊 队 列 ");
    int doctorId;
    gotoxy(30, 8); printf("医生ID: "); scanf("%d", &doctorId);
    Doctor* doc = findDoctorById(doctorId);
    if (!doc) { setColor(COLOR_ERROR); gotoxy(30, 10); printf("医生不存在！"); setColor(COLOR_DEFAULT); pressAnyKey(); return; }
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ========== 医生 %s 的候诊队列 ==========\n\n", doc->name);
    setColor(COLOR_DEFAULT);
    const char* headers[] = {"排队号", "患者ID", "患者姓名", "挂号时间", "状态"};
    int widths[] = {8, 8, 12, 20, 8};
    printTableHeader(headers, widths, 5);
    MedicalRecord* r = recordHead;
    int found = 0, row = 0;
    while (r) {
        if (r->doctorId == doctorId && r->status == 0) {
            Patient* p = findPatientById(r->patientId);
            char queueStr[10], pidStr[10];
            sprintf(queueStr, "%d", r->queueNumber);
            sprintf(pidStr, "%d", r->patientId);
            const char* values[] = {queueStr, pidStr, p ? p->name : "未知", r->recordTime, "待就诊"};
            printTableRow(values, widths, 5, (row % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD);
            found = 1; row++;
        }
        r = r->next;
    }
    if (!found) printf("\n  当前无候诊患者。\n");
    pressAnyKey();
}

void callNextPatient() {
    clearScreen();
    drawModernBox(25, 5, 45, 10, " 叫 号 就 诊 ");
    int doctorId;
    gotoxy(30, 8); printf("医生ID: "); scanf("%d", &doctorId);
    Doctor* doc = findDoctorById(doctorId);
    if (!doc) { setColor(COLOR_ERROR); gotoxy(30, 10); printf("医生不存在！"); setColor(COLOR_DEFAULT); pressAnyKey(); return; }
    MedicalRecord* nextRec = NULL;
    MedicalRecord* r = recordHead;
    while (r) {
        if (r->doctorId == doctorId && r->status == 0)
            if (nextRec == NULL || r->queueNumber < nextRec->queueNumber) nextRec = r;
        r = r->next;
    }
    if (!nextRec) { setColor(COLOR_WARNING); gotoxy(30, 10); printf("当前没有待就诊患者。"); setColor(COLOR_DEFAULT); pressAnyKey(); return; }
    Patient* p = findPatientById(nextRec->patientId);
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ========== 请下一位患者就诊 ==========\n\n");
    setColor(COLOR_DEFAULT);
    printf("  排队号: %d  患者ID: %d  姓名: %s\n", nextRec->queueNumber, nextRec->patientId, p ? p->name : "未知");
    printf("  挂号类型: %s  挂号时间: %s\n", nextRec->recordType, nextRec->recordTime);
    printf("  请准备接诊...\n");
    pressAnyKey();
}

void completeCurrentPatient() {
    clearScreen();
    drawModernBox(25, 5, 50, 12, " 完 成 就 诊 ");
    int doctorId, patientId;
    gotoxy(30, 8);  printf("医生ID: "); scanf("%d", &doctorId);
    gotoxy(30, 10); printf("患者ID: "); scanf("%d", &patientId);
    MedicalRecord* r = recordHead;
    while (r) {
        if (r->doctorId == doctorId && r->patientId == patientId && r->status == 0) {
            gotoxy(30, 12); printf("诊断结果: "); getchar(); fgets(r->diagnosis, MAX_DESC_LEN, stdin);
            r->diagnosis[strcspn(r->diagnosis, "\n")] = 0;
            r->status = 1;
            setColor(COLOR_SUCCESS);
            gotoxy(30, 14); printf("就诊完成，诊断已记录。");
            setColor(COLOR_DEFAULT);
            MedicalRecord* visitRec = (MedicalRecord*)malloc(sizeof(MedicalRecord));
            visitRec->id = getNextRecordId();
            visitRec->patientId = patientId;
            visitRec->doctorId = doctorId;
            strcpy(visitRec->recordType, "看诊");
            strcpy(visitRec->recordTime, getCurrentTime());
            strcpy(visitRec->appointmentTime, "");
            visitRec->queueNumber = 0;
            visitRec->status = 1;
            strcpy(visitRec->diagnosis, r->diagnosis);
            visitRec->next = recordHead;
            recordHead = visitRec;
            pressAnyKey();
            return;
        }
        r = r->next;
    }
    setColor(COLOR_ERROR);
    gotoxy(30, 12); printf("未找到对应的待就诊记录。");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void showMyQueue() {
    clearScreen();
    drawModernBox(25, 5, 80, 9, " 查 询 排 队 信 息 ");
    int patientId;
    gotoxy(30, 8); printf("患者ID: "); scanf("%d", &patientId);
    MedicalRecord* r = recordHead;
    int found = 0;
    while (r) {
        if (r->patientId == patientId && r->status == 0) {
            Doctor* doc = findDoctorById(r->doctorId);
            gotoxy(30, 10); printf("您有一条待就诊记录：");
            gotoxy(30, 12); printf("医生: %s  排队号: %d  挂号时间: %s", doc ? doc->name : "未知", r->queueNumber, r->recordTime);
            found = 1; break;
        }
        r = r->next;
    }
    if (!found) gotoxy(30, 10); printf("您当前无待就诊记录。");
    pressAnyKey();
}