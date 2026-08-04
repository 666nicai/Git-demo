// ============================================================
// 文件名: main.c
// 描述: 主菜单及程序入口（最终完美对齐版，包含全部子菜单）
// ============================================================

#include "his.h"
#include "user.h"
#include <string.h>

void mainMenu();
int handleCommandLine(int argc, char *argv[]);
void userAuthentication();

int main(int argc, char *argv[]) {
    if (argc > 1) {
        int result = handleCommandLine(argc, argv);
        if (result == 0) return 0;
        printf("未知参数，使用 --help 查看帮助\n");
        return 1;
    }

    if (!loadAllData()) {
        initDefaultData();
        printf("系统初始化完成，已创建默认数据。\n");
    }
    userAuthentication();
    mainMenu();
    saveAllData();
    printf("数据已保存，系统退出。\n");
    return 0;
}

void userAuthentication() {
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
        printTitle();   // 标题共占用 10 行（第 1~10 行）
        
        // 登录框从第 12 行开始，留出两行空隙
        drawModernBox(30, 11, 30, 11, " 登 录 / 注 册 ");
        setColor(COLOR_DEFAULT);
        gotoxy(36, 13); printf("1. 登  录");
        gotoxy(36, 15); printf("2. 注  册");
        gotoxy(36, 17); printf("0. 退出系统");
        gotoxy(36, 19);
        setColor(COLOR_WARNING);
        printf("请选择 (0-2): ");
        setColor(COLOR_DEFAULT);
        scanf("%d", &choice);
        while (getchar() != '\n');

        if (choice == 0) {
            showCursor();
            printf("\n感谢使用，再见！\n");
            exit(0);
        }

        clearScreen();
        drawModernBox(25, 8, 45, 14, (choice == 1) ? " 用 户 登 录 " : " 用 户 注 册 ");
        gotoxy(30, 11); printf("用户名: ");
        gotoxy(30, 13); printf("密  码: ");
        safeInput(username, MAX_NAME_LEN, 38, 11);
        gotoxy(38, 13);
        getPassword(password, MAX_NAME_LEN);

        if (choice == 1) {
            int result = loginUser(username, password, &role, &relatedId);
            if (result == 1) {
                setColor(COLOR_SUCCESS);
                gotoxy(30, 16); printf("登录成功！欢迎 %s", username);
                setColor(COLOR_DEFAULT);
                gotoxy(30, 18); printf("角色: %s", role == ROLE_ADMIN ? "管理员" :
                                      (role == ROLE_DOCTOR ? "医生" : "患者"));
                pressAnyKey();
                showCursor();
                return;
            } else {
                setColor(COLOR_ERROR);
                gotoxy(30, 16);
                if (result == 0) printf("用户不存在！");
                else printf("密码错误！");
                setColor(COLOR_DEFAULT);
                pressAnyKey();
            }
        } else {
            gotoxy(30, 15); printf("确认密码: ");
            gotoxy(40, 15);
            getPassword(confirmPass, MAX_NAME_LEN);
            if (strcmp(password, confirmPass) != 0) {
                setColor(COLOR_ERROR);
                gotoxy(30, 18); printf("两次输入的密码不一致！");
                setColor(COLOR_DEFAULT);
                pressAnyKey();
                continue;
            }
            gotoxy(30, 17); printf("角色 (0-管理员, 1-医生, 2-患者): ");
            scanf("%d", &regRole);
            while (getchar() != '\n');
            gotoxy(30, 19); printf("关联ID (医生工号/患者ID，管理员填0): ");
            scanf("%d", &regRelatedId);
            while (getchar() != '\n');

            int regResult = registerUser(username, password, (UserRole)regRole, regRelatedId);
            if (regResult == 1) {
                setColor(COLOR_SUCCESS);
                gotoxy(30, 22); printf("注册成功！请登录。");
            } else {
                setColor(COLOR_ERROR);
                gotoxy(30, 22);
                if (regResult == 0) printf("注册失败：用户名已存在！");
                else if (regResult == -1) printf("注册失败：关联医生ID不存在！");
                else if (regResult == -2) printf("注册失败：关联患者ID不存在！");
                else printf("注册失败！");
            }
            setColor(COLOR_DEFAULT);
            pressAnyKey();
        }
        showCursor();
    }
}

void mainMenu() {
    int choice;
    do {
        clearScreen();
        hideCursor();
        printTitle();
        drawModernBox(26, 11, 30, 16, " 主 菜 单 ");
        setColor(COLOR_DEFAULT);
        char* items[] = {
            "1. 患者管理", "2. 医生管理", "3. 科室管理",
            "4. 病房床位", "5. 药品管理", "6. 医疗记录",
            "7. 处方发药", "8. 统计报表", "9. 查询功能",
            "10. 挂号候诊", "11. 收费管理", "0. 保存退出"
        };
        for (int i = 0; i < 12; i++) {
            gotoxy(33, 13 + i);
            printf("%s", items[i]);
        }
        gotoxy(33, 28);
        setColor(COLOR_WARNING);
        printf("请选择: ");
        setColor(COLOR_DEFAULT);
        scanf("%d", &choice);
        while (getchar() != '\n');

        switch(choice) {
            case 1: {
                int op;
                do {
                    clearScreen();
                    drawModernBox(28, 8, 30, 15, " 患 者 管 理 ");
                    gotoxy(33, 10); printf("1. 添加患者");
                    gotoxy(33, 12); printf("2. 查看所有");
                    gotoxy(33, 14); printf("3. 修改信息");
                    gotoxy(33, 16); printf("4. 删除患者");
                    gotoxy(33, 18); printf("0. 返回");
                    gotoxy(33, 20); printf("请选择: ");
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
                    drawModernBox(28, 8, 30, 15, " 医 生 管 理 ");
                    gotoxy(33, 10); printf("1. 添加医生");
                    gotoxy(33, 12); printf("2. 查看所有");
                    gotoxy(33, 14); printf("3. 修改信息");
                    gotoxy(33, 16); printf("4. 删除医生");
                    gotoxy(33, 18); printf("0. 返回");
                    gotoxy(33, 20); printf("请选择: ");
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
                    drawModernBox(28, 8, 30, 15, " 科 室 管 理 ");
                    gotoxy(33, 10); printf("1. 添加科室");
                    gotoxy(33, 12); printf("2. 查看所有");
                    gotoxy(33, 14); printf("3. 删除科室");
                    gotoxy(33, 16); printf("4. 修改信息");
                    gotoxy(33, 18); printf("0. 返回");
                    gotoxy(33, 20); printf("请选择: ");
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
                    drawModernBox(28, 8, 30, 13, " 病 房 床 位 ");
                    gotoxy(33, 10); printf("1. 添加病房");
                    gotoxy(33, 12); printf("2. 查看病房");
                    gotoxy(33, 14); printf("3. 床位分配/出院");
                    gotoxy(33, 16); printf("0. 返回");
                    gotoxy(33, 18); printf("请选择: ");
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
                    drawModernBox(28, 8, 30, 15, " 药 品 管 理 ");
                    gotoxy(33, 10); printf("1. 添加药品");
                    gotoxy(33, 12); printf("2. 查看药品");
                    gotoxy(33, 14); printf("3. 库存变更");
                    gotoxy(33, 16); printf("4. 删除药品");
                    gotoxy(33, 18); printf("0. 返回");
                    gotoxy(33, 20); printf("请选择: ");
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
                    drawModernBox(28, 8, 30, 11, " 医 疗 记 录 ");
                    gotoxy(33, 10); printf("1. 添加记录");
                    gotoxy(33, 12); printf("2. 查看所有记录");
                    gotoxy(33, 14); printf("0. 返回");
                    gotoxy(33, 16); printf("请选择: ");
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
                    drawModernBox(28, 8, 30, 11, " 处 方 管 理 ");
                    gotoxy(33, 10); printf("1. 开处方(发药)");
                    gotoxy(33, 12); printf("2. 查看所有处方");
                    gotoxy(33, 14); printf("0. 返回");
                    gotoxy(33, 16); printf("请选择: ");
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
                    drawModernBox(28, 8, 35, 21, " 统 计 报 表 ");
                    gotoxy(33, 10); printf("1. 患者统计");
                    gotoxy(33, 12); printf("2. 医生工作量");
                    gotoxy(33, 14); printf("3. 科室就诊统计");
                    gotoxy(33, 16); printf("4. 药品库存预警");
                    gotoxy(33, 18); printf("5. 病房利用率");
                    gotoxy(33, 20); printf("6. 综合报表");
                    gotoxy(33, 22); printf("7. 患者视角病历");
                    gotoxy(33, 24); printf("0. 返回");
                    gotoxy(33, 26); printf("请选择: ");
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
                    drawModernBox(28, 8, 30, 13, " 查 询 功 能 ");
                    gotoxy(33, 10); printf("1. 按姓名查患者");
                    gotoxy(33, 12); printf("2. 按姓名查医生");
                    gotoxy(33, 14); printf("3. 按药品名查询");
                    gotoxy(33, 16); printf("0. 返回");
                    gotoxy(33, 18); printf("请选择: ");
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
                    drawModernBox(28, 8, 30, 17, " 挂 号 候 诊 ");
                    gotoxy(33, 10); printf("1. 患者挂号");
                    gotoxy(33, 12); printf("2. 医生查看队列");
                    gotoxy(33, 14); printf("3. 医生叫号");
                    gotoxy(33, 16); printf("4. 完成就诊");
                    gotoxy(33, 18); printf("5. 患者查排队");
                    gotoxy(33, 20); printf("0. 返回");
                    gotoxy(33, 22); printf("请选择: ");
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
                    drawModernBox(28, 8, 30, 15, " 收 费 管 理 ");
                    gotoxy(33, 10); printf("1. 费用项目管理");
                    gotoxy(33, 12); printf("2. 患者费用管理");
                    gotoxy(33, 14); printf("3. 缴费管理");
                    gotoxy(33, 16); printf("4. 结算与发票");
                    gotoxy(33, 18); printf("0. 返回");
                    gotoxy(33, 20); printf("请选择: ");
                    scanf("%d", &op);
                    if (op == 1) {
                        int sub;
                        do {
                            clearScreen();
                            drawModernBox(28, 8, 30, 15, " 费 用 项 目 ");
                            gotoxy(33, 10); printf("1. 添加");
                            gotoxy(33, 12); printf("2. 列表");
                            gotoxy(33, 14); printf("3. 修改");
                            gotoxy(33, 16); printf("4. 删除");
                            gotoxy(33, 18); printf("0. 返回");
                            gotoxy(33, 20); printf("请选择: ");
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
                            drawModernBox(28, 8, 35, 18, " 患 者 费 用 ");
                            gotoxy(33, 10); printf("1. 添加费用");
                            gotoxy(33, 12); printf("2. 处方费用");
                            gotoxy(33, 14); printf("3. 床位费用");
                            gotoxy(33, 16); printf("4. 费用列表");
                            gotoxy(33, 18); printf("5. 按患者查看");
                            gotoxy(33, 20); printf("6. 欠费查看");
                            gotoxy(33, 22); printf("0. 返回");
                            gotoxy(33, 24); printf("请选择: ");
                            scanf("%d", &sub);
                            if (sub == 1) addPatientFee();
                            else if (sub == 2) addPrescriptionFee();
                            else if (sub == 3) addWardFee();
                            else if (sub == 4) { listPatientFees(); pressAnyKey(); }
                            else if (sub == 5) { listFeesByPatient(); pressAnyKey(); }
                            else if (sub == 6) { listUnpaidFeesByPatient(); pressAnyKey(); }
                        } while (sub != 0);
                    } else if (op == 3) {
                        int sub;
                        do {
                            clearScreen();
                            drawModernBox(28, 8, 30, 15, " 缴 费 管 理 ");
                            gotoxy(33, 10); printf("1. 缴费");
                            gotoxy(33, 12); printf("2. 缴费记录");
                            gotoxy(33, 14); printf("3. 按患者查缴费");
                            gotoxy(33, 16); printf("4. 费用汇总");
                            gotoxy(33, 18); printf("0. 返回");
                            gotoxy(33, 20); printf("请选择: ");
                            scanf("%d", &sub);
                            if (sub == 1) makePayment();
                            else if (sub == 2) { listPayments(); pressAnyKey(); }
                            else if (sub == 3) { listPaymentsByPatient(); pressAnyKey(); }
                            else if (sub == 4) { showPatientBalance(); pressAnyKey(); }
                        } while (sub != 0);
                    } else if (op == 4) {
                        int sub;
                        do {
                            clearScreen();
                            drawModernBox(28, 8, 30, 10, " 结 算 发 票 ");
                            gotoxy(33, 10); printf("1. 出院结算");
                            gotoxy(33, 12); printf("2. 打印发票");
                            gotoxy(33, 14); printf("0. 返回");
                            gotoxy(33, 16); printf("请选择: ");
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
                gotoxy(33, 30); printf("无效选择！");
                setColor(COLOR_DEFAULT);
                pressAnyKey();
        }
    } while(choice != 0);
}

int handleCommandLine(int argc, char *argv[]) {
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        printf("HIS 命令行模式用法:\n");
        printf("  his.exe --stats              输出系统统计数据(JSON)\n");
        printf("  his.exe --patients           输出患者列表(JSON)\n");
        printf("  his.exe --doctors            输出医生列表(JSON)\n");
        printf("  his.exe --queue <医生ID>     输出指定医生的候诊队列(JSON)\n");
        printf("  his.exe --call-next <医生ID> 叫号下一位患者(JSON)\n");
        printf("  his.exe --fees <患者ID>      输出患者费用明细(JSON)\n");
        printf("  his.exe --unpaid <患者ID>    输出患者欠费明细(JSON)\n");
        printf("  his.exe --invoice <患者ID>   输出患者发票数据(JSON)\n");
        printf("  his.exe --register <用户名> <密码> <角色(0/1/2)> <关联ID>  注册用户(JSON)\n");
        printf("  his.exe --login <用户名> <密码>                            登录验证(JSON)\n");
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
        if (argc < 3) { fprintf(stderr, "错误: 需要提供医生ID\n"); return 1; }
        int doctorId = atoi(argv[2]);
        outputQueueJson(doctorId);
        return 0;
    }
    else if (strcmp(argv[1], "--call-next") == 0) {
        if (argc < 3) { fprintf(stderr, "错误: 需要提供医生ID\n"); return 1; }
        int doctorId = atoi(argv[2]);
        outputCallNextJson(doctorId);
        return 0;
    }
    else if (strcmp(argv[1], "--fees") == 0) {
        if (argc < 3) { fprintf(stderr, "错误: 需要提供患者ID\n"); return 1; }
        int patientId = atoi(argv[2]);
        outputFeesJson(patientId);
        return 0;
    }
    else if (strcmp(argv[1], "--unpaid") == 0) {
        if (argc < 3) { fprintf(stderr, "错误: 需要提供患者ID\n"); return 1; }
        int patientId = atoi(argv[2]);
        outputUnpaidFeesJson(patientId);
        return 0;
    }
    else if (strcmp(argv[1], "--invoice") == 0) {
        if (argc < 3) { fprintf(stderr, "错误: 需要提供患者ID\n"); return 1; }
        int patientId = atoi(argv[2]);
        outputInvoiceJson(patientId);
        return 0;
    }
    else if (strcmp(argv[1], "--register") == 0) {
        if (argc < 6) {
            fprintf(stderr, "用法: --register <用户名> <密码> <角色(0/1/2)> <关联ID>\n");
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
            fprintf(stderr, "用法: --login <用户名> <密码>\n");
            return 1;
        }
        const char* username = argv[2];
        const char* password = argv[3];
        outputLoginJson(username, password);
        return 0;
    }

    return -1;
}