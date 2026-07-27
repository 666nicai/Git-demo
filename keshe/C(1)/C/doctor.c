// ============================================================
// �ļ���: doctor.c
// ����: ҽ������ģ�飨��Ȩ�޿��ƣ�
//       ����Ա����ɾ�Ĳ飻ҽ�����ɲ鿴��Ϣ
//       �������ܣ����ӡ��޸ġ�ɾ����������Ա����
// ============================================================

#include "his.h"

// �Ҳ����⿴����
// �ⲿ��������ǰ�û���ɫ
extern UserRole currentUserRole;

// ======================= ����ҽ����������Ա�� =======================
void addDoctor() {
    // Ȩ�޼�飺������Ա������ҽ��
    if (currentUserRole != ROLE_ADMIN) {
        clearScreen();
        setColor(COLOR_ERROR);
        printf("\n  Ȩ�޲��㣬������Ա������ҽ����\n");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }

    clearScreen();
    drawModernBox(25, 5, 45, 15, " �� �� ҽ �� ");
    Doctor* doc = (Doctor*)malloc(sizeof(Doctor));
    gotoxy(30, 8);  printf("ҽ��ID: "); scanf("%d", &doc->id);
    gotoxy(30, 10); printf("����: "); safeInput(doc->name, 20, 36, 10);
    gotoxy(30, 12); printf("��������ID: "); scanf("%d", &doc->deptId);
    gotoxy(30, 14); printf("ְ��: "); safeInput(doc->title, 20, 36, 14);
    gotoxy(30, 16); printf("����ʱ��: "); safeInput(doc->workTime, 20, 40, 16);
    doc->next = doctorHead;
    doctorHead = doc;
    setColor(COLOR_SUCCESS);
    gotoxy(30, 18); printf("ҽ�����ӳɹ���");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

// ======================= �鿴����ҽ�������н�ɫ�ɲ鿴�� =======================
void listDoctors() {
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== ҽ �� �� �� ====================\n\n");
    setColor(COLOR_DEFAULT);
    Doctor* doc = doctorHead;
    if (!doc) { printf("  ����ҽ����Ϣ��\n"); pressAnyKey(); return; }
    Doctor* tail = doc;
    while(tail->next!=NULL) tail=tail->next;
    const char* headers[] = {"ID", "����", "����ID", "ְ��", "����ʱ��"};
    int widths[] = {6, 10, 8, 12, 20};
    printTableHeader(headers, widths, 5);
    int row = 0;
    while (tail) {
        char idStr[10], deptStr[10];
        sprintf(idStr, "%d", tail->id);
        sprintf(deptStr, "%d", tail->deptId);
        const char* values[] = {idStr, tail->name, deptStr, tail->title, tail->workTime};
        printTableRow(values, widths, 5, (row % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD);
        Doctor* prev = doctorHead;
        if(prev == tail) break;
        while(prev->next!=tail){
            prev=prev->next;
        }
        tail = prev;
        row++;
    }
    pressAnyKey();
}

// ======================= ����ҽ�� =======================
Doctor* findDoctorById(int id) {
    Doctor* doc = doctorHead;
    while (doc) { if (doc->id == id) return doc; doc = doc->next; }
    return NULL;
}

// ======================= �����Ҳ鿴ҽ�� =======================
void listDoctorsByDept(int deptId) {
    Doctor* doc = doctorHead;
    printf("����%d��ҽ��:\n", deptId);
    while (doc) {
        if (doc->deptId == deptId) printf("  %d. %s (%s)\n", doc->id, doc->name, doc->title);
        doc = doc->next;
    }
}

// ======================= �޸�ҽ����������Ա�� =======================
void updateDoctor() {
    // Ȩ�޼�飺������Ա���޸�ҽ����Ϣ
    if (currentUserRole != ROLE_ADMIN) {
        clearScreen();
        setColor(COLOR_ERROR);
        printf("\n  Ȩ�޲��㣬������Ա���޸�ҽ����Ϣ��\n");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }

    clearScreen();
    drawModernBox(25, 5, 45, 15, " �� �� ҽ �� �� Ϣ ");
    int id;
    gotoxy(30, 8); printf("ҽ��ID: "); scanf("%d", &id);
    Doctor *p = findDoctorById(id);
    if (!p) {
        setColor(COLOR_ERROR);
        gotoxy(30, 10); printf("δ�ҵ���ҽ����");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }

    // ����
    gotoxy(30, 10); printf("������(ԭ:%s): ", p->name);
    safeInput(p->name, 10, 47, 10);

    // ����ID
    gotoxy(30, 12); printf("�¿���ID(ԭ:%d): ", p->deptId);
    scanf("%d", &p->deptId);

    // ְ��
    gotoxy(30, 14); printf("��ְ��(ԭ:%s): ", p->title);
    safeInput(p->title, 10, 51, 14);

    // ����ʱ��
    gotoxy(30, 16); printf("�³���ʱ��(ԭ:%s): ", p->workTime);
    safeInput(p->workTime, 10, 55, 16);

    setColor(COLOR_SUCCESS);
    gotoxy(30, 18); printf("�޸ĳɹ���");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

// ======================= ɾ��ҽ����������Ա�� =======================
void deleteDoctor() {
    // Ȩ�޼�飺������Ա��ɾ��ҽ��
    if (currentUserRole != ROLE_ADMIN) {
        clearScreen();
        setColor(COLOR_ERROR);
        printf("\n  Ȩ�޲��㣬������Ա��ɾ��ҽ����\n");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }

    clearScreen();
    drawModernBox(25, 5, 40, 8, " ɾ �� ҽ �� ");
    int id;
    gotoxy(30, 8); printf("ҽ��ID: "); scanf("%d", &id);
    Doctor *d = doctorHead, *prev = NULL;
    while (d) {
        if (d->id == id) {
            if (prev) prev->next = d->next;
            else doctorHead = d->next;
            free(d);
            setColor(COLOR_SUCCESS);
            gotoxy(30, 10); printf("ɾ���ɹ���");
            setColor(COLOR_DEFAULT);
            pressAnyKey();
            return;
        }
        prev = d;
        d = d->next;
    }
    setColor(COLOR_ERROR);
    gotoxy(30, 10); printf("δ�ҵ���ҽ��");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}