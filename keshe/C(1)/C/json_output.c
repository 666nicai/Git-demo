// ============================================================
// 文件名: json_output.c
// 描述: 命令行 JSON 输出功能实现（含叫号功能）
// ============================================================

#include "his.h"
#include <stdio.h>
#include <string.h>

static void printJsonString(const char *str) {
    putchar('"');
    while (*str) {
        switch (*str) {
            case '"':  printf("\\\""); break;
            case '\\': printf("\\\\"); break;
            case '\n': printf("\\n"); break;
            case '\r': printf("\\r"); break;
            case '\t': printf("\\t"); break;
            default:   putchar(*str);
        }
        str++;
    }
    putchar('"');
}

void outputStatsJson() {
    int totalPatients = 0, inpatients = 0, outpatients = 0;
    Patient *p = patientHead;
    while (p) {
        totalPatients++;
        if (p->type == 1) inpatients++;
        else outpatients++;
        p = p->next;
    }

    int todayReg = 0;
    MedicalRecord *r = recordHead;
    char today[11];
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(today, sizeof(today), "%Y-%m-%d", tm);
    while (r) {
        if (strncmp(r->recordTime, today, 10) == 0) {
            todayReg++;
        }
        r = r->next;
    }

    int unpaidCount = 0;
    PatientFee *pf = patientFeeHead;
    int unpaidIds[1000] = {0};
    int idx = 0;
    while (pf) {
        if (pf->status == 0) {
            int found = 0;
            for (int i = 0; i < idx; i++) {
                if (unpaidIds[i] == pf->patientId) { found = 1; break; }
            }
            if (!found && idx < 1000) {
                unpaidIds[idx++] = pf->patientId;
            }
        }
        pf = pf->next;
    }
    unpaidCount = idx;

    printf("{");
    printf("\"totalPatients\":%d,", totalPatients);
    printf("\"inpatients\":%d,", inpatients);
    printf("\"outpatients\":%d,", outpatients);
    printf("\"todayRegistrations\":%d,", todayReg);
    printf("\"unpaidPatients\":%d", unpaidCount);
    printf("}\n");
}

void outputPatientsJson() {
    printf("[");
    Patient *p = patientHead;
    int first = 1;
    while (p) {
        if (!first) printf(",");
        first = 0;
        printf("{");
        printf("\"id\":%d,", p->id);
        printf("\"name\":"); printJsonString(p->name); printf(",");
        printf("\"gender\":\"%c\",", p->gender);
        printf("\"age\":%d,", p->age);
        printf("\"phone\":"); printJsonString(p->phone); printf(",");
        printf("\"type\":%d,", p->type);
        printf("\"bedId\":%d", p->bedId);
        printf("}");
        p = p->next;
    }
    printf("]\n");
}

void outputDoctorsJson() {
    printf("[");
    Doctor *d = doctorHead;
    int first = 1;
    while (d) {
        if (!first) printf(",");
        first = 0;
        printf("{");
        printf("\"id\":%d,", d->id);
        printf("\"name\":"); printJsonString(d->name); printf(",");
        printf("\"deptId\":%d,", d->deptId);
        printf("\"title\":"); printJsonString(d->title);
        printf("}");
        d = d->next;
    }
    printf("]\n");
}

void outputQueueJson(int doctorId) {
    Doctor *doc = findDoctorById(doctorId);
    if (!doc) {
        printf("{\"error\":\"医生不存在\"}\n");
        return;
    }
    printf("{\"doctorId\":%d,\"doctorName\":", doctorId);
    printJsonString(doc->name);
    printf(",\"queue\":[");
    MedicalRecord *r = recordHead;
    int first = 1;
    while (r) {
        if (r->doctorId == doctorId && r->status == 0) {
            if (!first) printf(",");
            first = 0;
            Patient *p = findPatientById(r->patientId);
            printf("{");
            printf("\"recordId\":%d,", r->id);
            printf("\"patientId\":%d,", r->patientId);
            printf("\"patientName\":"); printJsonString(p ? p->name : "未知"); printf(",");
            printf("\"queueNumber\":%d,", r->queueNumber);
            printf("\"recordType\":"); printJsonString(r->recordType); printf(",");
            printf("\"recordTime\":"); printJsonString(r->recordTime);
            printf("}");
        }
        r = r->next;
    }
    printf("]}\n");
}

void outputFeesJson(int patientId) {
    Patient *pat = findPatientById(patientId);
    if (!pat) {
        printf("{\"error\":\"患者不存在\"}\n");
        return;
    }
    printf("{\"patientId\":%d,\"patientName\":", patientId);
    printJsonString(pat->name);
    printf(",\"fees\":[");
    PatientFee *pf = patientFeeHead;
    int first = 1;
    float total = 0, paid = 0;
    while (pf) {
        if (pf->patientId == patientId) {
            if (!first) printf(",");
            first = 0;
            printf("{");
            printf("\"id\":%d,", pf->id);
            char itemName[50];
            if (pf->feeItemId == 888) strcpy(itemName, "床位费");
            else if (pf->feeItemId == 999) strcpy(itemName, "药品费");
            else {
                FeeItem *item = findFeeItemById(pf->feeItemId);
                if (item) strcpy(itemName, item->name);
                else strcpy(itemName, "未知");
            }
            printf("\"itemName\":"); printJsonString(itemName); printf(",");
            printf("\"quantity\":%d,", pf->quantity);
            printf("\"unitPrice\":%.2f,", pf->unitPrice);
            printf("\"totalAmount\":%.2f,", pf->totalAmount);
            printf("\"status\":%d,", pf->status);
            printf("\"chargeTime\":"); printJsonString(pf->chargeTime);
            printf("}");
            total += pf->totalAmount;
            if (pf->status == 1) paid += pf->totalAmount;
        }
        pf = pf->next;
    }
    printf("],\"total\":%.2f,\"paid\":%.2f,\"unpaid\":%.2f}\n", total, paid, total - paid);
}

void outputUnpaidFeesJson(int patientId) {
    Patient *pat = findPatientById(patientId);
    if (!pat) {
        printf("{\"error\":\"患者不存在\"}\n");
        return;
    }
    printf("{\"patientId\":%d,\"patientName\":", patientId);
    printJsonString(pat->name);
    printf(",\"unpaidFees\":[");
    PatientFee *pf = patientFeeHead;
    int first = 1;
    float totalUnpaid = 0;
    while (pf) {
        if (pf->patientId == patientId && pf->status == 0) {
            if (!first) printf(",");
            first = 0;
            printf("{");
            printf("\"id\":%d,", pf->id);
            char itemName[50];
            if (pf->feeItemId == 888) strcpy(itemName, "床位费");
            else if (pf->feeItemId == 999) strcpy(itemName, "药品费");
            else {
                FeeItem *item = findFeeItemById(pf->feeItemId);
                if (item) strcpy(itemName, item->name);
                else strcpy(itemName, "未知");
            }
            printf("\"itemName\":"); printJsonString(itemName); printf(",");
            printf("\"totalAmount\":%.2f,", pf->totalAmount);
            printf("\"chargeTime\":"); printJsonString(pf->chargeTime);
            printf("}");
            totalUnpaid += pf->totalAmount;
        }
        pf = pf->next;
    }
    printf("],\"totalUnpaid\":%.2f}\n", totalUnpaid);
}

void outputInvoiceJson(int patientId) {
    Patient *p = findPatientById(patientId);
    if (!p) {
        printf("{\"error\":\"患者不存在\"}\n");
        return;
    }
    printf("{");
    printf("\"patientId\":%d,", p->id);
    printf("\"patientName\":"); printJsonString(p->name); printf(",");
    printf("\"gender\":\"%c\",", p->gender);
    printf("\"age\":%d,", p->age);
    printf("\"patientType\":\"%s\",", p->type == 0 ? "门诊" : "住院");

    printf("\"items\":[");
    PatientFee *pf = patientFeeHead;
    int first = 1;
    float total = 0, paid = 0;
    while (pf) {
        if (pf->patientId == patientId) {
            if (!first) printf(",");
            first = 0;
            char itemName[50];
            if (pf->feeItemId == 888) strcpy(itemName, "床位费");
            else if (pf->feeItemId == 999) strcpy(itemName, "药品费");
            else {
                FeeItem *item = findFeeItemById(pf->feeItemId);
                if (item) strcpy(itemName, item->name);
                else strcpy(itemName, "其他");
            }
            printf("{");
            printf("\"name\":"); printJsonString(itemName); printf(",");
            printf("\"quantity\":%d,", pf->quantity);
            printf("\"unitPrice\":%.2f,", pf->unitPrice);
            printf("\"amount\":%.2f,", pf->totalAmount);
            printf("\"status\":\"%s\"", pf->status == 0 ? "未付" : "已付");
            printf("}");
            total += pf->totalAmount;
            if (pf->status == 1) paid += pf->totalAmount;
        }
        pf = pf->next;
    }
    printf("],");
    printf("\"totalAmount\":%.2f,", total);
    printf("\"paidAmount\":%.2f,", paid);
    printf("\"dueAmount\":%.2f", total - paid);
    printf("}\n");
}

void outputCallNextJson(int doctorId) {
    Doctor *doc = findDoctorById(doctorId);
    if (!doc) {
        printf("{\"error\":\"医生不存在\"}\n");
        return;
    }

    MedicalRecord *nextRec = NULL;
    MedicalRecord *r = recordHead;
    while (r) {
        if (r->doctorId == doctorId && r->status == 0) {
            if (nextRec == NULL || r->queueNumber < nextRec->queueNumber) {
                nextRec = r;
            }
        }
        r = r->next;
    }

    if (!nextRec) {
        printf("{\"error\":\"当前没有待就诊患者\"}\n");
        return;
    }

    nextRec->status = 1;

    Patient *p = findPatientById(nextRec->patientId);
    char patientName[MAX_NAME_LEN] = "未知";
    if (p) strcpy(patientName, p->name);

    printf("{");
    printf("\"success\":true,");
    printf("\"recordId\":%d,", nextRec->id);
    printf("\"patientId\":%d,", nextRec->patientId);
    printf("\"patientName\":"); printJsonString(patientName); printf(",");
    printf("\"queueNumber\":%d,", nextRec->queueNumber);
    printf("\"doctorId\":%d,", doctorId);
    printf("\"doctorName\":"); printJsonString(doc->name);
    printf("}\n");
}