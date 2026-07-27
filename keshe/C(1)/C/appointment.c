// ============================================================
// 文件名: appointment.c
// 描述: 预约挂号、候诊队列、医生叫号模块（美化版）
//       医生登录时自动使用自己的ID，无需手动输入
// ============================================================

#include "his.h"

static int getMaxQueueNumber(int doctorId) {    //获取指定医生当前最大排位号
    int maxNum = 0;
    MedicalRecord* r = recordHead;
    while (r) {
        if (r->doctorId == doctorId && r->status == 0)              
            if (r->queueNumber > maxNum) maxNum = r->queueNumber;   //找最大值
        r = r->next;                                                //往后递推
    }
    return maxNum;                                                  //返回最大值
}

static int getNextRecordId() {                                      //获取新病历记录的ID
    int maxId = 0;
    MedicalRecord* r = recordHead;
    while (r) { if (r->id > maxId) maxId = r->id; r = r->next; }    //找最大的ID
    return maxId + 1;
}

void registerAppointment() {                                            //挂号主函数
    clearScreen();                                                      //清屏
    drawModernBox(25, 5, 50, 16, " 门 诊 挂 号 ");                       //输出终端框架
    MedicalRecord* rec = (MedicalRecord*)malloc(sizeof(MedicalRecord)); //申请内存大小
    int patientId, doctorId, choice;
    gotoxy(30, 8); printf("患者ID: "); scanf("%d", &patientId);          //输入患者ID
    Patient* p = findPatientById(patientId);                            // 用ID查找
    if (!p) { setColor(COLOR_ERROR);                                    
        gotoxy(30, 10); printf("患者不存在！");
         setColor(COLOR_DEFAULT); free(rec); pressAnyKey(); return; }   // 没找到
    gotoxy(30, 10); printf("挂号方式：1.预约挂号  2.现场挂号: ");         
    scanf("%d", &choice);                                                // 选择挂号
    if (choice == 1) strcpy(rec->recordType, "预约挂号");
    else strcpy(rec->recordType, "现场挂号");                             // 两种情况
    gotoxy(30, 12);                                                      // 终端范围
    printf("医生ID（0则仅选科室）: "); scanf("%d", &doctorId);             // 选择挂号医生
    if (doctorId == 0) {
        int deptId;
        gotoxy(30, 14); printf("科室ID: "); scanf("%d", &deptId);         // 选择科室
        Doctor* d = doctorHead;
        while (d) { if (d->deptId == deptId) { doctorId = d->id; break; } d = d->next; } // 从科室中选择医生
        if (doctorId == 0) { setColor(COLOR_ERROR); gotoxy(30, 16); printf("该科室暂无医生！");// 科室中没有医生 
            setColor(COLOR_DEFAULT); free(rec); pressAnyKey(); return; }  // 结束返回
    }
    Doctor* doc = findDoctorById(doctorId);                                // 通过ID查找医生
    if (!doc) { setColor(COLOR_ERROR); gotoxy(30, 14); printf("医生不存在！");
         setColor(COLOR_DEFAULT); free(rec); pressAnyKey(); return; }      // 医生不存在 结束 返回
    rec->id = getNextRecordId();    //
    rec->patientId = patientId;     //
    rec->doctorId = doctorId;       //记录信息
    strcpy(rec->recordTime, getCurrentTime());  // 记录时间
    if (choice == 1) { gotoxy(30, 14); printf("预约日期(YYYY-MM-DD): ");  // 输入预约日期
                        safeInput(rec->appointmentTime, 15, 56, 14); }   // 安全输入预约日期
    else strcpy(rec->appointmentTime, "今日就诊");       // 现场挂号：预约时间标记为"今日就诊"
    rec->queueNumber = getMaxQueueNumber(doctorId) + 1; // 当前医生最大排队号+1
    rec->status = 0;                                    // 设置就诊状态：0-待就诊
    strcpy(rec->diagnosis, "");                         // 初始化诊断结果为空
    rec->next = recordHead;                             // 将新病历插入链表头部
    recordHead = rec;
    setColor(COLOR_SUCCESS);
    gotoxy(30, 18); printf("挂号成功！记录ID:%d 排队号:%d 医生:%s", rec->id, rec->queueNumber, doc->name);
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void listQueueByDoctor() {
    clearScreen();
    int doctorId;
    // 若为医生角色，则自动使用当前医生ID，不再询问
    if (currentUserRole == ROLE_DOCTOR) {
        doctorId = currentDoctorId;
    } else {
        drawModernBox(25, 5, 45, 8, " 查 看 候 诊 队 列 ");
        gotoxy(30, 8); printf("医生ID: "); scanf("%d", &doctorId);
    }
    Doctor* doc = findDoctorById(doctorId);
    if (!doc) { setColor(COLOR_ERROR); gotoxy(30, 10); printf("医生不存在！"); setColor(COLOR_DEFAULT); pressAnyKey(); return; }
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ========== 医生 %s 的候诊队列 ==========\n\n", doc->name);
    setColor(COLOR_DEFAULT);
    const char* headers[] = {"排队号", "患者ID", "患者姓名", "挂号时间", "状态"}; // 定义表格表头
    int widths[] = {8, 8, 12, 20, 8};                                          // 定义表格列宽
    printTableHeader(headers, widths, 5);                                      // 打印表格表头
    MedicalRecord* r = recordHead;
    int found = 0, row = 0;                                                    // 标记是否找到记录、行号
    while (r) {
        if (r->doctorId == doctorId && r->status == 0) {                       // 筛选当前医生的待就诊记录
            Patient* p = findPatientById(r->patientId);                        // 查找患者信息
            char queueStr[10], pidStr[10];                                     // 初始化
            sprintf(queueStr, "%d", r->queueNumber);
            sprintf(pidStr, "%d", r->patientId);
            const char* values[] = {queueStr, pidStr, p ? p->name : "未知", r->recordTime, "待就诊"};  // 定义表格行数据
            printTableRow(values, widths, 5, (row % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD); // 隔行变色
            found = 1; row++;    // 标记找到记录 行号自增
        }
        r = r->next;
    }
    if (!found) printf("\n  当前无候诊患者。\n");
    pressAnyKey();
}

void callNextPatient() {
    clearScreen();
    int doctorId;
    // 医生角色自动取自己的ID
    if (currentUserRole == ROLE_DOCTOR) {
        doctorId = currentDoctorId;
    } else {
        drawModernBox(25, 5, 45, 10, " 叫 号 就 诊 ");              // 绘制叫号就诊窗口
        gotoxy(30, 8); printf("医生ID: "); scanf("%d", &doctorId); // 提示输入医生ID
    }
    Doctor* doc = findDoctorById(doctorId);
    if (!doc) { setColor(COLOR_ERROR); gotoxy(30, 10); printf("医生不存在！"); setColor(COLOR_DEFAULT); pressAnyKey(); return; }
    MedicalRecord* nextRec = NULL;         // 初始化下一位患者记录为NULL
    MedicalRecord* r = recordHead;
    while (r) {
        if (r->doctorId == doctorId && r->status == 0)  // 筛选当前医生的待就诊记录
            if (nextRec == NULL || r->queueNumber < nextRec->queueNumber) nextRec = r;  // 找到排队号最小的记录
        r = r->next;
    }
    if (!nextRec) { setColor(COLOR_WARNING); gotoxy(30, 10); printf("当前没有待就诊患者。");
         setColor(COLOR_DEFAULT); pressAnyKey(); return; } // 无待就诊患者的提示
    Patient* p = findPatientById(nextRec->patientId);
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ========== 请下一位患者就诊 ==========\n\n"); // 打印
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
    // 医生角色自动取自己的ID
    if (currentUserRole == ROLE_DOCTOR) {
        doctorId = currentDoctorId;
    } else {
        gotoxy(30, 8);  printf("医生ID: "); scanf("%d", &doctorId);
    }
    gotoxy(30, 10); printf("患者ID: "); scanf("%d", &patientId);
    MedicalRecord* r = recordHead;
    while (r) {
        if (r->doctorId == doctorId && r->patientId == patientId && r->status == 0) { // 筛选当前医生的指定待就诊患者
            gotoxy(30, 12); printf("诊断结果: "); getchar(); 
            fgets(r->diagnosis, MAX_DESC_LEN, stdin);      // 读取诊断结果
            r->diagnosis[strcspn(r->diagnosis, "\n")] = 0; // 去除换行符
            r->status = 1;                                 // 更新就诊状态：1-已就诊
            setColor(COLOR_SUCCESS);
            gotoxy(30, 14); printf("就诊完成，诊断已记录。");
            setColor(COLOR_DEFAULT);
            
            MedicalRecord* visitRec = (MedicalRecord*)malloc(sizeof(MedicalRecord));  // 生成看诊记录
            visitRec->id = getNextRecordId();
            visitRec->patientId = patientId;
            visitRec->doctorId = doctorId;
            strcpy(visitRec->recordType, "看诊");            // 设置记录类型为"看诊"
            strcpy(visitRec->recordTime, getCurrentTime());
            strcpy(visitRec->appointmentTime, "");           // 设置记录类型为"看诊"
            visitRec->queueNumber = 0;                       // 看诊记录无需排队
            visitRec->status = 1;                            // 状态置为已就诊 
            strcpy(visitRec->diagnosis, r->diagnosis);       // 复制诊断结果
            visitRec->next = recordHead;
            recordHead = visitRec;                           // 插入看诊记录到链表头部
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
    drawModernBox(25, 5, 80, 9, " 查 询 排 队 信 息 ");          // 绘制查询排队信息窗口
    int patientId;
    gotoxy(30, 8); printf("患者ID: "); scanf("%d", &patientId);
    MedicalRecord* r = recordHead;
    int found = 0;
    while (r) {
        if (r->patientId == patientId && r->status == 0) {
            Doctor* doc = findDoctorById(r->doctorId);
            gotoxy(30, 10); printf("您有一条待就诊记录：");
            gotoxy(30, 12); printf("医生: %s  排队号: %d  挂号时间: %s", doc ? doc->name : "未知", r->queueNumber, r->recordTime);
            found = 1; break;      // 标记找到记录
        }
        r = r->next;
    }
    if (!found) gotoxy(30, 10); printf("您当前无待就诊记录。");
    pressAnyKey();
}