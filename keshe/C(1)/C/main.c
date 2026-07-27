// ============================================================
// ?????: main.c
// ????: ???????????????????????????ó®?????????????
// ============================================================

#include "his.h"
#include "user.h"
#include <string.h>

// ??????˜œ?? appointment.c???????????????
int currentDoctorId = 0;                // ??????????? ID?????????????? 0??
UserRole currentUserRole = 0;           // ???????????????0=?????, 1=???, 2=?????

// ?????????????????????? void mainMenu();??
void mainMenu(UserRole role, int relatedId);
int handleCommandLine(int argc, char *argv[]);
int userAuthentication(UserRole *outRole, int *outRelatedId);

int main(int argc, char *argv[]) {
    if (argc > 1) {
        int result = handleCommandLine(argc, argv);
        if (result == 0) return 0;
        printf("¶ƒ?????????? --help ??????\n");
        return 1;
    }

if (!loadAllData()) {
    initDefaultUsers();  // ??????????????????????????????
    printf("¶ƒ????????????????????????????????\n");
}

    UserRole role;
    int relatedId;
    if (userAuthentication(&role, &relatedId) == 0) {
        printf("??ß›?????????\n");
        return 0;
    }

    mainMenu(role, relatedId);
    saveAllData();
    printf("????????ó®???????\n");
    return 0;
}

int userAuthentication(UserRole *outRole, int *outRelatedId) {
    int choice;
    char username[MAX_NAME_LEN];
    char password[MAX_NAME_LEN];
    UserRole role;
    int relatedId;
    int regRole, regRelatedId;
    char confirmPass[MAX_NAME_LEN];

    while (1) {
        clearScreen();
        hideCursor();
        printTitle();

        drawModernBox(30, 11, 30, 11, " ?? ? / ? ?? ");
        setColor(COLOR_DEFAULT);
        gotoxy(36, 13); printf("1. ??  ?");
        gotoxy(36, 15); printf("2. ?  ??");
        gotoxy(36, 17); printf("0. ?????");
        gotoxy(36, 19);
        setColor(COLOR_WARNING);
        printf("????? (0-2): ");
        setColor(COLOR_DEFAULT);
        scanf("%d", &choice);
        while (getchar() != '\n');

        if (choice == 0) {
            showCursor();
            return 0;                     // ?????????
        }

        clearScreen();
        drawModernBox(25, 8, 45, 14, (choice == 1) ? " ?? ?? ?? ? " : " ?? ?? ? ?? ");
        gotoxy(30, 11); printf("?????: ");
        gotoxy(30, 13); printf("??  ??: ");
        safeInput(username, 20, 38, 11);
        gotoxy(38, 13);
        getPassword(password, MAX_NAME_LEN);

        if (choice == 1) {
            int result = loginUser(username, password, &role, &relatedId);
            if (result == 1) {
                setColor(COLOR_SUCCESS);
                gotoxy(30, 16); printf("??????????? %s", username);
                setColor(COLOR_DEFAULT);
                gotoxy(30, 18); printf("???: %s", role == ROLE_ADMIN ? "?????" :
                                      (role == ROLE_DOCTOR ? "???" : "????"));
                pressAnyKey();
                showCursor();

                // ?????????????? appointment.c ?????????
                if (role == ROLE_DOCTOR) {
                    currentDoctorId = relatedId;
                } else {
                    currentDoctorId = 0;
                }
                currentUserRole = role;         // ?????????

                *outRole = role;
                *outRelatedId = relatedId;
                return 1;
            } else {
                setColor(COLOR_ERROR);
                gotoxy(30, 16);
                if (result == 0) printf("??????????");
                else printf("???????");
                setColor(COLOR_DEFAULT);
                pressAnyKey();
            }
        } else {
            gotoxy(30, 15); printf("???????: ");
            gotoxy(40, 15);
            getPassword(confirmPass, MAX_NAME_LEN);
            if (strcmp(password, confirmPass) != 0) {
                setColor(COLOR_ERROR);
                gotoxy(30, 18); printf("?????????????????");
                setColor(COLOR_DEFAULT);
                pressAnyKey();
                continue;
            }
            gotoxy(30, 17); printf("??? (0-?????, 1-???, 2-????): ");
            scanf("%d", &regRole);
            while (getchar() != '\n');
            gotoxy(30, 19); printf("????ID (???????/????ID?????????0): ");
            scanf("%d", &regRelatedId);
            while (getchar() != '\n');

            int regResult = registerUser(username, password, (UserRole)regRole, regRelatedId);
            if (regResult == 1) {
                setColor(COLOR_SUCCESS);
                gotoxy(30, 22); printf("?????????????");
            } else {
                setColor(COLOR_ERROR);
                gotoxy(30, 22);
                if (regResult == 0) printf("??????????????????");
                else if (regResult == -1) printf("??????????????ID???????");
                else if (regResult == -2) printf("???????????????ID???????");
                else printf("???????");
            }
            setColor(COLOR_DEFAULT);
            pressAnyKey();
        }
        showCursor();
    }
}

void mainMenu(UserRole role, int relatedId) {
    if (role == ROLE_PATIENT) {   // ??????????
        int choice;
        int patientId = relatedId;
        do {
            clearScreen();
            hideCursor();
            printTitle();
            drawModernBox(26, 11, 30, 18, " ?? ?? ?? ?? ?? ");
            setColor(COLOR_DEFAULT);
            char* items[] = {
                "1. ??????",
                "2. ???????",
                "3. ????¶À",
                "4. ??????",
                "5. ??????",
                "6. ??????",
                "7. ??????",
                "0. ???"
            };
            for (int i = 0; i < 8; i++) {
                gotoxy(33, 13 + i);
                printf("%s", items[i]);
            }
            gotoxy(33, 22);
            setColor(COLOR_WARNING);
            printf("?????: ");
            setColor(COLOR_DEFAULT);
            scanf("%d", &choice);
            while (getchar() != '\n');

            switch(choice) {
                case 1: {   // ??????
                    clearScreen();
                    setColor(COLOR_TITLE);
                    printf("\n  ==================== ?? ?? ?? ?? ? ?? ====================\n\n");
                    setColor(COLOR_DEFAULT);
                    MedicalRecord* rec = recordHead;
                    int doctorIds[50] = {0};
                    int docCount = 0;
                    while (rec) {
                        if (rec->patientId == patientId) {
                            int found = 0;
                            for (int i = 0; i < docCount; i++) {
                                if (doctorIds[i] == rec->doctorId) { found = 1; break; }
                            }
                            if (!found && docCount < 50) {
                                doctorIds[docCount++] = rec->doctorId;
                            }
                        }
                        rec = rec->next;
                    }
                    if (docCount == 0) {
                        printf("  ???????????????\n");
                    } else {
                        printf("  ??????????????\n");
                        for (int i = 0; i < docCount; i++) {
                            Doctor* d = findDoctorById(doctorIds[i]);
                            if (d) {
                                printf("  ID:%d  ????:%s  ???:%s  ????:%d\n",
                                       d->id, d->name, d->title, d->deptId);
                            }
                        }
                    }
                    pressAnyKey();
                    break;
                }
                case 2: {   // ???????
                    clearScreen();
                    setColor(COLOR_TITLE);
                    printf("\n  ==================== ?? ?? ?? ?? ?? ?? ====================\n\n");
                    setColor(COLOR_DEFAULT);
                    MedicalRecord* rec2 = recordHead;
                    int deptIds[20] = {0};
                    int deptCount = 0;
                    while (rec2) {
                        if (rec2->patientId == patientId) {
                            Doctor* d = findDoctorById(rec2->doctorId);
                            if (d) {
                                int found = 0;
                                for (int i = 0; i < deptCount; i++) {
                                    if (deptIds[i] == d->deptId) { found = 1; break; }
                                }
                                if (!found && deptCount < 20) {
                                    deptIds[deptCount++] = d->deptId;
                                }
                            }
                        }
                        rec2 = rec2->next;
                    }
                    if (deptCount == 0) {
                        printf("  ???????????????\n");
                    } else {
                        printf("  ?????/?????ùp??????\n");
                        for (int i = 0; i < deptCount; i++) {
                            Department* dept = findDepartmentById(deptIds[i]);
                            if (dept) {
                                printf("  ID:%d  ????????:%s\n", dept->id, dept->name);
                            }
                        }
                    }
                    pressAnyKey();
                    break;
                }
                case 3: {   // ????¶À
                    clearScreen();
                    Patient* me = findPatientById(patientId);
                    if (!me) {
                        setColor(COLOR_ERROR);
                        printf("\n  ???????????\n");
                        setColor(COLOR_DEFAULT);
                        pressAnyKey();
                        break;
                    }
                    setColor(COLOR_TITLE);
                    printf("\n  ==================== ?? ?? ?? ¶À ====================\n\n");
                    setColor(COLOR_DEFAULT);
                    if (me->type == 1 && me->bedId > 0) {
                        Ward* w = wardHead;
                        int bedFound = 0;
                        while (w && !bedFound) {
                            Bed* b = w->bedList;
                            while (b) {
                                if (b->bedId == me->bedId) {
                                    printf("  ????ID: %d  ????????: %s\n", w->id, w->type);
                                    printf("  ??¶À??: %d\n", me->bedId);
                                    bedFound = 1;
                                    break;
                                }
                                b = b->next;
                            }
                            w = w->next;
                        }
                        if (!bedFound) {
                            printf("  ?????¶À??: %d??¶ƒ???????????????\n", me->bedId);
                        }
                    } else {
                        printf("  ????????¶À???????????¶ƒ??????\n");
                    }

                    if (me->type == 1) {
                        printf("\n  ????????????VIP??????(1.?? / 0.????): ");
                        int vipChoice;
                        scanf("%d", &vipChoice);
                        if (vipChoice == 1) {
                            printf("\n  ?????VIP???????ß’?¶À??\n");
                            Ward* w = wardHead;
                            int foundAny = 0;
                            while (w) {
                                if (strcmp(w->type, "VIP") == 0) {
                                    Bed* b = w->bedList;
                                    while (b) {
                                        if (b->status == 0) {
                                            printf("  ????ID:%d  ??¶À??:%d\n", w->id, b->bedId);
                                            foundAny = 1;
                                        }
                                        b = b->next;
                                    }
                                }
                                w = w->next;
                            }
                            if (!foundAny) {
                                printf("  ???????VIP??¶À??\n");
                            } else {
                                printf("  ???????????VIP????ID: ");
                                int wardId;
                                scanf("%d", &wardId);
                                int valid = 0;
                                Ward* wv = wardHead;
                                while (wv) {
                                    if (wv->id == wardId && strcmp(wv->type, "VIP") == 0) {
                                        Bed* bedCheck = wv->bedList;
                                        while (bedCheck) {
                                            if (bedCheck->status == 0) { valid = 1; break; }
                                            bedCheck = bedCheck->next;
                                        }
                                        break;
                                    }
                                    wv = wv->next;
                                }
                                if (valid) {
                                    int oldBedId = me->bedId;
                                    int newBedId = allocateBed(patientId, wardId);
                                    if (newBedId > 0) {
                                        if (oldBedId > 0) freeBed(oldBedId);
                                        me->bedId = newBedId;
                                        setColor(COLOR_SUCCESS);
                                        printf("  ????????????¶À??: %d\n", newBedId);
                                    } else {
                                        setColor(COLOR_ERROR);
                                        printf("  ????????????®∞?????????ß’?¶À??\n");
                                    }
                                    setColor(COLOR_DEFAULT);
                                } else {
                                    setColor(COLOR_ERROR);
                                    printf("  ??ßπ??VIP????ID??????ß’?¶À??\n");
                                    setColor(COLOR_DEFAULT);
                                }
                            }
                        }
                    }
                    pressAnyKey();
                    break;
                }
                case 4: {   // ??????
                    clearScreen();
                    setColor(COLOR_TITLE);
                    printf("\n  ==================== ?? ?? ?? ?? ====================\n\n");
                    setColor(COLOR_DEFAULT);
                    Patient* me2 = findPatientById(patientId);
                    if (!me2) {
                        printf("  ??????????????\n");
                        pressAnyKey();
                        break;
                    }
                    printf("  ????:%s  ???:%c  ????:%d  ?¥¬:%s  ????:%s\n",
                           me2->name, me2->gender, me2->age, me2->phone,
                           (me2->type == 0 ? "????" : "??"));
                    if (me2->type == 1 && me2->bedId > 0) {
                        printf("  ????¶À??:%d\n", me2->bedId);
                    }
                    printf("\n  ?????:\n");
                    MedicalRecord* rec3 = recordHead;
                    int hasRec = 0;
                    while (rec3) {
                        if (rec3->patientId == patientId) {
                            printf("    [%s] %s ???ID:%d ???:%s\n",
                                   rec3->recordTime, rec3->recordType,
                                   rec3->doctorId, rec3->diagnosis);
                            hasRec = 1;
                        }
                        rec3 = rec3->next;
                    }
                    if (!hasRec) printf("  ????????\n");
                    pressAnyKey();
                    break;
                }
                case 5:     // ??????
                    searchDoctorByName();
                    break;
                case 6: {   // ??????
                    int sub;
                    do {
                        clearScreen();
                        drawModernBox(28, 8, 30, 12, " ?? ?? ?? ?? ");
                        gotoxy(33, 10); printf("1. ??????");
                        gotoxy(33, 12); printf("2. ?????");
                        gotoxy(33, 14); printf("0. ????");
                        gotoxy(33, 16); printf("?????: ");
                        scanf("%d", &sub);
                        if (sub == 1) { registerAppointment(); pressAnyKey(); }
                        else if (sub == 2) { showMyQueue(); pressAnyKey(); }
                    } while (sub != 0);
                    break;
                }
                case 7: {   // ??????
                    int sub;
                    do {
                        clearScreen();
                        drawModernBox(28, 8, 30, 15, " ?? ?? ?? ?? ");
                        gotoxy(33, 10); printf("1. ???");
                        gotoxy(33, 12); printf("2. ?????");
                        gotoxy(33, 14); printf("3. ???????");
                        gotoxy(33, 16); printf("4. ???????");
                        gotoxy(33, 18); printf("0. ????");
                        gotoxy(33, 20); printf("?????: ");
                        scanf("%d", &sub);
                        if (sub == 1) makePayment();
                        else if (sub == 2) { listPayments(); pressAnyKey(); }
                        else if (sub == 3) { showPatientBalance(); pressAnyKey(); }
                        else if (sub == 4) {
                            int subsub;
                            do {
                                clearScreen();
                                drawModernBox(28, 8, 30, 10, " ?? ?? ?? ?? ? ");
                                gotoxy(33, 10); printf("1. ???????");
                                gotoxy(33, 12); printf("2. ??????");
                                gotoxy(33, 14); printf("0. ????");
                                gotoxy(33, 16); printf("?????: ");
                                scanf("%d", &subsub);
                                if (subsub == 1) { settlePatientAccount(); pressAnyKey(); }
                                else if (subsub == 2) { printInvoice(); pressAnyKey(); }
                            } while (subsub != 0);
                        }
                    } while (sub != 0);
                    break;
                }
                case 0: break;
                default:
                    setColor(COLOR_ERROR);
                    gotoxy(33, 24); printf("??ßπ???");
                    setColor(COLOR_DEFAULT);
                    pressAnyKey();
            }
        } while (choice != 0);
    } else {   // ????? / ??????????????
        int choice;
        do {
            clearScreen();
            hideCursor();
            printTitle();
            drawModernBox(26, 11, 30, 16, " ?? ?? ?? ");
            setColor(COLOR_DEFAULT);
            char* items[] = {
                "1. ???????", "2. ???????", "3. ???????",
                "4. ??????¶À", "5. ??????", "6. ?????",
                "7. ???????", "8. ??????", "9. ???????",
                "10. ??????", "11. ??????", "0. ???????"
            };
            for (int i = 0; i < 12; i++) {
                gotoxy(33, 13 + i);
                printf("%s", items[i]);
            }
            gotoxy(33, 28);
            setColor(COLOR_WARNING);
            printf("?????: ");
            setColor(COLOR_DEFAULT);
            scanf("%d", &choice);
            while (getchar() != '\n');

            switch(choice) {
                case 1: {
                    int op;
                    do {
                        clearScreen();
                        drawModernBox(28, 8, 30, 15, " ?? ?? ?? ?? ");
                        gotoxy(33, 10); printf("1. ???????");
                        gotoxy(33, 12); printf("2. ??????");
                        gotoxy(33, 14); printf("3. ??????");
                        gotoxy(33, 16); printf("4. ???????");
                        gotoxy(33, 18); printf("0. ????");
                        gotoxy(33, 20); printf("?????: ");
                        scanf("%d", &op);
                        if (op == 1) addPatient();
                        else if (op == 2) { listPatients(); pressAnyKey(); }
                        else if (op == 3) updatePatient();
                        else if (op == 4) deletePatient();
                    } while (op != 0);
                    break;
                }
                case 2: {
                    int op;
                    do {
                        clearScreen();
                        drawModernBox(28, 8, 30, 15, " ? ?? ?? ?? ");
                        gotoxy(33, 10); printf("1. ???????");
                        gotoxy(33, 12); printf("2. ??????");
                        gotoxy(33, 14); printf("3. ??????");
                        gotoxy(33, 16); printf("4. ??????");
                        gotoxy(33, 18); printf("0. ????");
                        gotoxy(33, 20); printf("?????: ");
                        scanf("%d", &op);
                        if (op == 1) addDoctor();
                        else if (op == 2) { listDoctors(); pressAnyKey(); }
                        else if (op == 3) updateDoctor();
                        else if (op == 4) deleteDoctor();
                    } while (op != 0);
                    break;
                }
                case 3: {
                    int op;
                    do {
                        clearScreen();
                        drawModernBox(28, 8, 30, 15, " ?? ?? ?? ?? ");
                        gotoxy(33, 10); printf("1. ???????");
                        gotoxy(33, 12); printf("2. ??????");
                        gotoxy(33, 14); printf("3. ???????");
                        gotoxy(33, 16); printf("4. ??????");
                        gotoxy(33, 18); printf("0. ????");
                        gotoxy(33, 20); printf("?????: ");
                        scanf("%d", &op);
                        if (op == 1) addDepartment();
                        else if (op == 2) { listDepartments(); pressAnyKey(); }
                        else if (op == 3) deleteDepartment();
                        else if (op == 4) updateDepartment();
                    } while (op != 0);
                    break;
                }
                case 4: {
                    int op;
                    do {
                        clearScreen();
                        drawModernBox(28, 8, 30, 13, " ?? ?? ?? ¶À ");
                        gotoxy(33, 10); printf("1. ???????");
                        gotoxy(33, 12); printf("2. ??????");
                        gotoxy(33, 14); printf("3. ??¶À????/???");
                        gotoxy(33, 16); printf("0. ????");
                        gotoxy(33, 18); printf("?????: ");
                        scanf("%d", &op);
                        if (op == 1) addWard();
                        else if (op == 2) { listWards(); pressAnyKey(); }
                        else if (op == 3) manageBeds();
                    } while (op != 0);
                    break;
                }
                case 5: {
                    int op;
                    do {
                        clearScreen();
                        drawModernBox(28, 8, 30, 15, " ? ? ?? ?? ");
                        gotoxy(33, 10); printf("1. ??????");
                        gotoxy(33, 12); printf("2. ????");
                        gotoxy(33, 14); printf("3. ?????");
                        gotoxy(33, 16); printf("4. ?????");
                        gotoxy(33, 18); printf("0. ????");
                        gotoxy(33, 20); printf("?????: ");
                        scanf("%d", &op);
                        if (op == 1) addMedicine();
                        else if (op == 2) { listMedicines(); pressAnyKey(); }
                        else if (op == 3) updateMedicineStock();
                        else if (op == 4) deleteMedicine();
                    } while (op != 0);
                    break;
                }
                case 6: {
                    int op;
                    do {
                        clearScreen();
                        drawModernBox(28, 8, 30, 11, " ? ?? ?? ? ");
                        gotoxy(33, 10); printf("1. ??????");
                        gotoxy(33, 12); printf("2. ?????ßﬁ??");
                        gotoxy(33, 14); printf("0. ????");
                        gotoxy(33, 16); printf("?????: ");
                        scanf("%d", &op);
                        if (op == 1) addMedicalRecord();
                        else if (op == 2) { listMedicalRecords(); pressAnyKey(); }
                    } while (op != 0);
                    break;
                }
                case 7: {
                    int op;
                    do {
                        clearScreen();
                        drawModernBox(28, 8, 30, 11, " ?? ?? ?? ?? ");
                        gotoxy(33, 10); printf("1. ??????(???)");
                        gotoxy(33, 12); printf("2. ?????ß’???");
                        gotoxy(33, 14); printf("0. ????");
                        gotoxy(33, 16); printf("?????: ");
                        scanf("%d", &op);
                        if (op == 1) addPrescription();
                        else if (op == 2) { listPrescriptions(); pressAnyKey(); }
                    } while (op != 0);
                    break;
                }
                case 8: {
                    int op;
                    do {
                        clearScreen();
                        drawModernBox(28, 8, 35, 21, " ? ?? ?? ?? ");
                        gotoxy(33, 10); printf("1. ???????");
                        gotoxy(33, 12); printf("2. ?????????");
                        gotoxy(33, 14); printf("3. ??????????");
                        gotoxy(33, 16); printf("4. ????????");
                        gotoxy(33, 18); printf("5. ??????????");
                        gotoxy(33, 20); printf("6. ??????");
                        gotoxy(33, 22); printf("7. ??????????");
                        gotoxy(33, 24); printf("0. ????");
                        gotoxy(33, 26); printf("?????: ");
                        scanf("%d", &op);
                        if (op == 1) { reportPatientStats(); pressAnyKey(); }
                        else if (op == 2) { reportDoctorWorkload(); pressAnyKey(); }
                        else if (op == 3) { reportDeptStats(); pressAnyKey(); }
                        else if (op == 4) { reportMedicineStock(); pressAnyKey(); }
                        else if (op == 5) { reportWardUtilization(); pressAnyKey(); }
                        else if (op == 6) { reportComprehensive(); pressAnyKey(); }
                        else if (op == 7) { reportPatientView(); pressAnyKey(); }
                    } while (op != 0);
                    break;
                }
                case 9: {
                    int op;
                    do {
                        clearScreen();
                        drawModernBox(28, 8, 30, 13, " ?? ? ?? ?? ");
                        gotoxy(33, 10); printf("1. ???????ÌÎ??");
                        gotoxy(33, 12); printf("2. ???????????");
                        gotoxy(33, 14); printf("3. ?????????");
                        gotoxy(33, 16); printf("0. ????");
                        gotoxy(33, 18); printf("?????: ");
                        scanf("%d", &op);
                        if (op == 1) { searchPatientByName(); pressAnyKey(); }
                        else if (op == 2) { searchDoctorByName(); pressAnyKey(); }
                        else if (op == 3) { searchMedicineByName(); pressAnyKey(); }
                    } while (op != 0);
                    break;
                }
                case 10: {
                    int op;
                    do {
                        clearScreen();
                        drawModernBox(28, 8, 30, 17, " ?? ?? ?? ?? ");
                        gotoxy(33, 10); printf("1. ??????");
                        gotoxy(33, 12); printf("2. ?????????");
                        gotoxy(33, 14); printf("3. ????ß‹?");
                        gotoxy(33, 16); printf("4. ??????");
                        gotoxy(33, 18); printf("5. ????????");
                        gotoxy(33, 20); printf("0. ????");
                        gotoxy(33, 22); printf("?????: ");
                        scanf("%d", &op);
                        if (op == 1) { registerAppointment(); pressAnyKey(); }
                        else if (op == 2) { listQueueByDoctor(); pressAnyKey(); }
                        else if (op == 3) { callNextPatient(); pressAnyKey(); }
                        else if (op == 4) { completeCurrentPatient(); pressAnyKey(); }
                        else if (op == 5) { showMyQueue(); pressAnyKey(); }
                    } while (op != 0);
                    break;
                }
                case 11: {
                    int op;
                    do {
                        clearScreen();
                        drawModernBox(28, 8, 30, 15, " ?? ?? ?? ?? ");
                        gotoxy(33, 10); printf("1. ???????????");
                        gotoxy(33, 12); printf("2. ??????®¥???");
                        gotoxy(33, 14); printf("3. ??????");
                        gotoxy(33, 16); printf("4. ???????");
                        gotoxy(33, 18); printf("0. ????");
                        gotoxy(33, 20); printf("?????: ");
                        scanf("%d", &op);
                        if (op == 1) {
                            int sub;
                            do {
                                clearScreen();
                                drawModernBox(28, 8, 30, 15, " ?? ?? ?? ? ");
                                gotoxy(33, 10); printf("1. ????");
                                gotoxy(33, 12); printf("2. ?ß“?");
                                gotoxy(33, 14); printf("3. ???");
                                gotoxy(33, 16); printf("4. ???");
                                gotoxy(33, 18); printf("0. ????");
                                gotoxy(33, 20); printf("?????: ");
                                scanf("%d", &sub);
                                if (sub == 1) addFeeItem();
                                else if (sub == 2) { listFeeItems(); pressAnyKey(); }
                                else if (sub == 3) updateFeeItem();
                                else if (sub == 4) deleteFeeItem();
                            } while (sub != 0);
                        } else if (op == 2) {
                            int sub;
                            do {
                                clearScreen();
                                drawModernBox(28, 8, 35, 18, " ?? ?? ?? ?? ");
                                gotoxy(33, 10); printf("1. ???????");
                                gotoxy(33, 12); printf("2. ????????");
                                gotoxy(33, 14); printf("3. ??¶À????");
                                gotoxy(33, 16); printf("4. ?????ß“?");
                               
                                gotoxy(33, 18); printf("5. ????");
                                gotoxy(33, 20); printf("0. ????");
                                gotoxy(33, 22); printf("?????: ");
                                scanf("%d", &sub);
                                if (sub == 1) addPatientFee();
                                else if (sub == 2) addPrescriptionFee();
                                else if (sub == 3) addWardFee();
                                else if (sub == 4) { listPatientFees(); pressAnyKey(); }
                               
                                else if (sub == 5) { listUnpaidFeesByPatient(); pressAnyKey(); }
                            } while (sub != 0);
                        } else if (op == 3) {
                            int sub;
                            do {
                                clearScreen();
                                drawModernBox(28, 8, 30, 15, " ?? ?? ?? ?? ");
                                gotoxy(33, 10); printf("1. ???");
                                gotoxy(33, 12); printf("2. ?????");
                               
                                gotoxy(33, 14); printf("3. ???????");
                                gotoxy(33, 16); printf("0. ????");
                                gotoxy(33, 18); printf("?????: ");
                                scanf("%d", &sub);
                                if (sub == 1) makePayment();
                                else if (sub == 2) { listPayments(); pressAnyKey(); }
                             
                                else if (sub == 3) { showPatientBalance(); pressAnyKey(); }
                            } while (sub != 0);
                        } else if (op == 4) {
                            int sub;
                            do {
                                clearScreen();
                                drawModernBox(28, 8, 30, 10, " ?? ?? ?? ? ");
                                gotoxy(33, 10); printf("1. ???????");
                                gotoxy(33, 12); printf("2. ??????");
                                gotoxy(33, 14); printf("0. ????");
                                gotoxy(33, 16); printf("?????: ");
                                scanf("%d", &sub);
                                if (sub == 1) { settlePatientAccount(); pressAnyKey(); }
                                else if (sub == 2) { printInvoice(); pressAnyKey(); }
                            } while (sub != 0);
                        }
                    } while (op != 0);
                    break;
                }
                case 0: break;
                default:
                    setColor(COLOR_ERROR);
                    gotoxy(33, 30); printf("??ßπ???");
                    setColor(COLOR_DEFAULT);
                    pressAnyKey();
            }
        } while(choice != 0);
    }
}

int handleCommandLine(int argc, char *argv[]) {
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        printf("HIS ?????????°¬?:\n");
        printf("  his.exe --stats              ????????????(JSON)\n");
        printf("  his.exe --patients           ????????ß“?(JSON)\n");
        printf("  his.exe --doctors            ???????ß“?(JSON)\n");
        printf("  his.exe --queue <???ID>     ?????????????????(JSON)\n");
        printf("  his.exe --call-next <???ID> ?ß‹????¶À????(JSON)\n");
        printf("  his.exe --fees <????ID>      ?????????????(JSON)\n");
        printf("  his.exe --unpaid <????ID>    ?????????????(JSON)\n");
        printf("  his.exe --invoice <????ID>   ?????????????(JSON)\n");
        printf("  his.exe --register <?????> <????> <???(0/1/2)> <????ID>  ??????(JSON)\n");
        printf("  his.exe --login <?????> <????>                            ??????(JSON)\n");
        return 0;
    }

    if (!loadAllData()) {
        initDefaultData();
    }

    if (strcmp(argv[1], "--stats") == 0) {
        outputStatsJson();
        return 0;
    }
    else if (strcmp(argv[1], "--patients") == 0) {
        outputPatientsJson();
        return 0;
    }
    else if (strcmp(argv[1], "--doctors") == 0) {
        outputDoctorsJson();
        return 0;
    }
    else if (strcmp(argv[1], "--queue") == 0) {
        if (argc < 3) { fprintf(stderr, "????: ????????ID\n"); return 1; }
        int doctorId = atoi(argv[2]);
        outputQueueJson(doctorId);
        return 0;
    }
    else if (strcmp(argv[1], "--call-next") == 0) {
        if (argc < 3) { fprintf(stderr, "????: ????????ID\n"); return 1; }
        int doctorId = atoi(argv[2]);
        outputCallNextJson(doctorId);
        return 0;
    }
    else if (strcmp(argv[1], "--fees") == 0) {
        if (argc < 3) { fprintf(stderr, "????: ?????????ID\n"); return 1; }
        int patientId = atoi(argv[2]);
        outputFeesJson(patientId);
        return 0;
    }
    else if (strcmp(argv[1], "--unpaid") == 0) {
        if (argc < 3) { fprintf(stderr, "????: ?????????ID\n"); return 1; }
        int patientId = atoi(argv[2]);
        outputUnpaidFeesJson(patientId);
        return 0;
    }
    else if (strcmp(argv[1], "--invoice") == 0) {
        if (argc < 3) { fprintf(stderr, "????: ?????????ID\n"); return 1; }
        int patientId = atoi(argv[2]);
        outputInvoiceJson(patientId);
        return 0;
    }
    else if (strcmp(argv[1], "--register") == 0) {
        if (argc < 6) {
            fprintf(stderr, "?°¬?: --register <?????> <????> <???(0/1/2)> <????ID>\n");
            return 1;
        }
        const char* username = argv[2];
        const char* password = argv[3];
        int role = atoi(argv[4]);
        int relatedId = atoi(argv[5]);
        outputRegisterJson(username, password, role, relatedId);
        return 0;
    }
    else if (strcmp(argv[1], "--login") == 0) {
        if (argc < 4) {
            fprintf(stderr, "?°¬?: --login <?????> <????>\n");
            return 1;
        }
        const char* username = argv[2];
        const char* password = argv[3];
        outputLoginJson(username, password);
        return 0;
    }

    return -1;
}