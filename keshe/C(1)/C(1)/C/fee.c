// ============================================================
// 文件名: fee.c
// 描述: 收费管理模块 (费用项目、患者费用、缴费管理) 美化版
// ============================================================

#include "his.h"

static int getNextFeeId(void);
static int getNextPaymentId(void);

extern FeeItem* feeItemHead;
extern PatientFee* patientFeeHead;
extern Payment* paymentHead;

// ======================= 费用项目管理 =======================
void addFeeItem() {
    clearScreen();
    drawModernBox(20, 5, 45, 12, " 添 加 费 用 项 目 ");
    FeeItem* item = (FeeItem*)malloc(sizeof(FeeItem));
    gotoxy(25, 8);  printf("费用项目ID: "); scanf("%d", &item->id);
    if (findFeeItemById(item->id) != NULL) {
        setColor(COLOR_ERROR);
        gotoxy(25, 10); printf("费用项目ID已存在！");
        setColor(COLOR_DEFAULT);
        free(item);
        pressAnyKey();
        return;
    }
    gotoxy(25, 10); printf("项目名称: "); safeInput(item->name, MAX_NAME_LEN, 35, 10);
    gotoxy(25, 12); printf("单价: "); scanf("%f", &item->price);
    gotoxy(25, 14); printf("类别(诊疗/检查/住院/手术/其他): "); safeInput(item->category, 20, 45, 14);
    item->next = feeItemHead;
    feeItemHead = item;
    setColor(COLOR_SUCCESS);
    gotoxy(25, 16); printf("费用项目添加成功！");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void listFeeItems() {
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== 费 用 项 目 列 表 ====================\n\n");
    setColor(COLOR_DEFAULT);
    FeeItem* item = feeItemHead;
    if (!item) {
        printf("  暂无费用项目信息。\n");
        pressAnyKey();
        return;
    }
    const char* headers[] = {"ID", "项目名称", "单价", "类别"};
    int widths[] = {6, 22, 10, 14};
    printTableHeader(headers, widths, 4);
    int row = 0;
    while (item) {
        char idStr[10], priceStr[15];
        sprintf(idStr, "%d", item->id);
        sprintf(priceStr, "%.2f", item->price);
        const char* values[] = {idStr, item->name, priceStr, item->category};
        printTableRow(values, widths, 4, (row % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD);
        item = item->next;
        row++;
    }
    pressAnyKey();
}

void updateFeeItem() {
    clearScreen();
    drawModernBox(20, 5, 45, 12, " 修 改 费 用 项 目 ");
    int id;
    gotoxy(25, 8); printf("请输入要修改的费用项目ID: "); scanf("%d", &id);
    FeeItem* item = findFeeItemById(id);
    if (!item) {
        setColor(COLOR_ERROR);
        gotoxy(25, 10); printf("费用项目不存在！");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }
    char buffer[MAX_NAME_LEN];
    gotoxy(25, 10); printf("当前项目名称: %s", item->name);
    gotoxy(25, 12); printf("请输入新名称(直接回车保留): ");
    getchar();
    fgets(buffer, MAX_NAME_LEN, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    if (strlen(buffer) > 0) strcpy(item->name, buffer);
    
    gotoxy(25, 14); printf("当前单价: %.2f，请输入新单价: ", item->price);
    float newPrice;
    scanf("%f", &newPrice);
    if (newPrice > 0) item->price = newPrice;
    
    gotoxy(25, 16); printf("当前类别: %s，请输入新类别: ", item->category);
    safeInput(buffer, 20, 45, 16);
    if (strlen(buffer) > 0) strcpy(item->category, buffer);
    
    setColor(COLOR_SUCCESS);
    gotoxy(25, 18); printf("费用项目修改成功！");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void deleteFeeItem() {
    clearScreen();
    drawModernBox(20, 5, 45, 8, " 删 除 费 用 项 目 ");
    int id;
    gotoxy(25, 8); printf("请输入要删除的费用项目ID: "); scanf("%d", &id);
    FeeItem *item = feeItemHead, *prev = NULL;
    while (item) {
        if (item->id == id) {
            if (prev) prev->next = item->next;
            else feeItemHead = item->next;
            free(item);
            setColor(COLOR_SUCCESS);
            gotoxy(25, 10); printf("费用项目已删除。");
            setColor(COLOR_DEFAULT);
            pressAnyKey();
            return;
        }
        prev = item;
        item = item->next;
    }
    setColor(COLOR_ERROR);
    gotoxy(25, 10); printf("未找到该费用项目。");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

FeeItem* findFeeItemById(int id) {
    FeeItem* item = feeItemHead;
    while (item) {
        if (item->id == id) return item;
        item = item->next;
    }
    return NULL;
}

// ======================= 患者费用管理 =======================
static int getNextFeeId() {
    int maxId = 0;
    PatientFee* pf = patientFeeHead;
    while (pf) {
        if (pf->id > maxId) maxId = pf->id;
        pf = pf->next;
    }
    return maxId + 1;
}

void addPatientFee() {
    clearScreen();
    drawModernBox(20, 5, 50, 14, " 添 加 患 者 费 用 ");
    PatientFee* fee = (PatientFee*)malloc(sizeof(PatientFee));
    fee->id = getNextFeeId();
    gotoxy(25, 8);  printf("请输入患者ID: "); scanf("%d", &fee->patientId);
    Patient* p = findPatientById(fee->patientId);
    if (!p) {
        setColor(COLOR_ERROR);
        gotoxy(25, 10); printf("患者不存在！");
        setColor(COLOR_DEFAULT);
        free(fee);
        pressAnyKey();
        return;
    }
    
    // 显示费用项目列表供选择
    clearScreen();
    drawModernBox(20, 5, 50, 14, " 选 择 费 用 项 目 ");
    listFeeItems();
    gotoxy(25, 8); printf("请输入费用项目ID: "); scanf("%d", &fee->feeItemId);
    FeeItem* item = findFeeItemById(fee->feeItemId);
    if (!item) {
        setColor(COLOR_ERROR);
        gotoxy(25, 10); printf("费用项目不存在！");
        setColor(COLOR_DEFAULT);
        free(fee);
        pressAnyKey();
        return;
    }
    gotoxy(25, 12); printf("请输入数量: "); scanf("%d", &fee->quantity);
    fee->unitPrice = item->price;
    fee->totalAmount = item->price * fee->quantity;
    strcpy(fee->chargeTime, getCurrentTime());
    fee->status = 0;
    gotoxy(25, 14); printf("关联医疗记录ID (无则0): "); scanf("%d", &fee->recordId);
    fee->next = patientFeeHead;
    patientFeeHead = fee;
    setColor(COLOR_SUCCESS);
    gotoxy(25, 16); printf("费用已添加！总金额: %.2f", fee->totalAmount);
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void addPrescriptionFee() {
    clearScreen();
    drawModernBox(20, 5, 45, 10, " 处 方 费 用 ");
    int prescriptionId;
    gotoxy(25, 8); printf("请输入处方ID: "); scanf("%d", &prescriptionId);
    Prescription* pre = findPrescriptionById(prescriptionId);
    if (!pre) {
        setColor(COLOR_ERROR);
        gotoxy(25, 10); printf("处方不存在！");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }
    float totalPrice = 0;
    PrescriptionItem* item = pre->items;
    while (item) {
        Medicine* med = findMedicineById(item->medicineId);
        if (med) totalPrice += med->price * item->quantity;
        item = item->next;
    }
    PatientFee* fee = (PatientFee*)malloc(sizeof(PatientFee));
    fee->id = getNextFeeId();
    fee->patientId = pre->patientId;
    fee->feeItemId = 999;
    fee->quantity = 1;
    fee->unitPrice = totalPrice;
    fee->totalAmount = totalPrice;
    strcpy(fee->chargeTime, getCurrentTime());
    fee->status = 0;
    fee->recordId = pre->recordId;
    fee->next = patientFeeHead;
    patientFeeHead = fee;
    setColor(COLOR_SUCCESS);
    gotoxy(25, 12); printf("药品费用已添加！总金额: %.2f", totalPrice);
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void addWardFee() {
    clearScreen();
    drawModernBox(20, 5, 45, 12, " 床 位 费 用 ");
    int patientId, days;
    gotoxy(25, 8); printf("请输入患者ID: "); scanf("%d", &patientId);
    Patient* p = findPatientById(patientId);
    if (!p || p->type != 1) {
        setColor(COLOR_ERROR);
        gotoxy(25, 10); printf("患者不存在或不是住院患者！");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }
    gotoxy(25, 10); printf("请输入住院天数: "); scanf("%d", &days);
    float bedFeePerDay = 50.0;
    Ward* w = wardHead;
    while (w) {
        Bed* b = w->bedList;
        while (b) {
            if (b->bedId == p->bedId && b->status == 1) {
                if (strcmp(w->type, "VIP") == 0) bedFeePerDay = 200.0;
                else if (strcmp(w->type, "双人") == 0) bedFeePerDay = 80.0;
                break;
            }
            b = b->next;
        }
        w = w->next;
    }
    PatientFee* fee = (PatientFee*)malloc(sizeof(PatientFee));
    fee->id = getNextFeeId();
    fee->patientId = patientId;
    fee->feeItemId = 888;
    fee->quantity = days;
    fee->unitPrice = bedFeePerDay;
    fee->totalAmount = bedFeePerDay * days;
    strcpy(fee->chargeTime, getCurrentTime());
    fee->status = 0;
    fee->recordId = 0;
    fee->next = patientFeeHead;
    patientFeeHead = fee;
    setColor(COLOR_SUCCESS);
    gotoxy(25, 14); printf("床位费已添加！天数: %d, 单价: %.2f, 总金额: %.2f", days, bedFeePerDay, fee->totalAmount);
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void listPatientFees() {
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== 患 者 费 用 明 细 ====================\n\n");
    setColor(COLOR_DEFAULT);
    PatientFee* fee = patientFeeHead;
    if (!fee) {
        printf("  暂无费用记录。\n");
        pressAnyKey();
        return;
    }
    const char* headers[] = {"ID", "患者ID", "费用项目", "数量", "单价", "总金额", "状态", "时间"};
    int widths[] = {5, 7, 14, 5, 8, 10, 7, 20};
    printTableHeader(headers, widths, 8);
    int row = 0;
    while (fee) {
        char idStr[8], pidStr[8], qtyStr[8], priceStr[12], totalStr[12];
        sprintf(idStr, "%d", fee->id);
        sprintf(pidStr, "%d", fee->patientId);
        sprintf(qtyStr, "%d", fee->quantity);
        sprintf(priceStr, "%.2f", fee->unitPrice);
        sprintf(totalStr, "%.2f", fee->totalAmount);
        char itemName[30];
        if (fee->feeItemId == 888) strcpy(itemName, "床位费");
        else if (fee->feeItemId == 999) strcpy(itemName, "药品费");
        else {
            FeeItem* item = findFeeItemById(fee->feeItemId);
            if (item) strcpy(itemName, item->name);
            else strcpy(itemName, "未知");
        }
        const char* statusStr = (fee->status == 0) ? "未支付" : "已支付";
        const char* values[] = {idStr, pidStr, itemName, qtyStr, priceStr, totalStr, statusStr, fee->chargeTime};
        int color = (row % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD;
        if (fee->status == 0) color = COLOR_WARNING;
        printTableRow(values, widths, 8, color);
        fee = fee->next;
        row++;
    }
    pressAnyKey();
}

void listFeesByPatient() {
    clearScreen();
    drawModernBox(20, 5, 45, 8, " 按 患 者 查 询 费 用 ");
    int patientId;
    gotoxy(25, 8); printf("请输入患者ID: "); scanf("%d", &patientId);
    Patient* p = findPatientById(patientId);
    if (!p) {
        setColor(COLOR_ERROR);
        gotoxy(25, 10); printf("患者不存在！");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ========== 患者 %s (ID:%d) 费用明细 ==========\n\n", p->name, patientId);
    setColor(COLOR_DEFAULT);
    const char* headers[] = {"ID", "费用项目", "数量", "单价", "总金额", "状态", "时间"};
    int widths[] = {5, 16, 5, 8, 10, 7, 20};
    printTableHeader(headers, widths, 7);
    float totalUnpaid = 0, totalPaid = 0;
    PatientFee* fee = patientFeeHead;
    int row = 0;
    while (fee) {
        if (fee->patientId == patientId) {
            char idStr[8], qtyStr[8], priceStr[12], totalStr[12];
            sprintf(idStr, "%d", fee->id);
            sprintf(qtyStr, "%d", fee->quantity);
            sprintf(priceStr, "%.2f", fee->unitPrice);
            sprintf(totalStr, "%.2f", fee->totalAmount);
            char itemName[30];
            if (fee->feeItemId == 888) strcpy(itemName, "床位费");
            else if (fee->feeItemId == 999) strcpy(itemName, "药品费");
            else {
                FeeItem* item = findFeeItemById(fee->feeItemId);
                if (item) strcpy(itemName, item->name);
                else strcpy(itemName, "未知");
            }
            const char* statusStr = (fee->status == 0) ? "未支付" : "已支付";
            const char* values[] = {idStr, itemName, qtyStr, priceStr, totalStr, statusStr, fee->chargeTime};
            int color = (row % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD;
            if (fee->status == 0) color = COLOR_WARNING;
            printTableRow(values, widths, 7, color);
            if (fee->status == 0) totalUnpaid += fee->totalAmount;
            else totalPaid += fee->totalAmount;
            row++;
        }
        fee = fee->next;
    }
    printf("\n");
    setColor(COLOR_TITLE);
    printf("  已支付金额: %.2f    未支付金额: ", totalPaid);
    setColor(COLOR_ERROR);
    printf("%.2f\n", totalUnpaid);
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void listUnpaidFeesByPatient() {
    clearScreen();
    drawModernBox(20, 5, 45, 8, " 查 询 欠 费 明 细 ");
    int patientId;
    gotoxy(25, 8); printf("请输入患者ID: "); scanf("%d", &patientId);
    Patient* p = findPatientById(patientId);
    if (!p) {
        setColor(COLOR_ERROR);
        gotoxy(25, 10); printf("患者不存在！");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ========== 患者 %s (ID:%d) 未缴费明细 ==========\n\n", p->name, patientId);
    setColor(COLOR_DEFAULT);
    const char* headers[] = {"ID", "费用项目", "数量", "单价", "总金额", "时间"};
    int widths[] = {5, 16, 5, 8, 10, 20};
    printTableHeader(headers, widths, 6);
    float totalUnpaid = 0;
    PatientFee* fee = patientFeeHead;
    int row = 0;
    while (fee) {
        if (fee->patientId == patientId && fee->status == 0) {
            char idStr[8], qtyStr[8], priceStr[12], totalStr[12];
            sprintf(idStr, "%d", fee->id);
            sprintf(qtyStr, "%d", fee->quantity);
            sprintf(priceStr, "%.2f", fee->unitPrice);
            sprintf(totalStr, "%.2f", fee->totalAmount);
            char itemName[30];
            if (fee->feeItemId == 888) strcpy(itemName, "床位费");
            else if (fee->feeItemId == 999) strcpy(itemName, "药品费");
            else {
                FeeItem* item = findFeeItemById(fee->feeItemId);
                if (item) strcpy(itemName, item->name);
                else strcpy(itemName, "未知");
            }
            const char* values[] = {idStr, itemName, qtyStr, priceStr, totalStr, fee->chargeTime};
            printTableRow(values, widths, 6, (row % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD);
            totalUnpaid += fee->totalAmount;
            row++;
        }
        fee = fee->next;
    }
    printf("\n");
    setColor(COLOR_ERROR);
    printf("  欠费总额: %.2f\n", totalUnpaid);
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void listUnpaidFeesByPatientId(int patientId) {
    Patient* p = findPatientById(patientId);
    if (!p) return;
    printf("\n========== 患者 %s (ID:%d) 未缴费明细 ==========\n", p->name, patientId);
    printf("ID\t费用项目\t数量\t单价\t总金额\t时间\n");
    float totalUnpaid = 0;
    PatientFee* fee = patientFeeHead;
    while (fee) {
        if (fee->patientId == patientId && fee->status == 0) {
            printf("%d\t", fee->id);
            if (fee->feeItemId == 888) printf("床位费\t");
            else if (fee->feeItemId == 999) printf("药品费\t");
            else {
                FeeItem* item = findFeeItemById(fee->feeItemId);
                if (item) printf("%s\t", item->name);
                else printf("未知\t");
            }
            printf("%d\t%.2f\t%.2f\t%s\n",
                   fee->quantity, fee->unitPrice, fee->totalAmount,
                   fee->chargeTime);
            totalUnpaid += fee->totalAmount;
        }
        fee = fee->next;
    }
    printf("欠费总额: %.2f\n", totalUnpaid);
}

// ======================= 缴费管理 =======================
static int getNextPaymentId() {
    int maxId = 0;
    Payment* pay = paymentHead;
    while (pay) {
        if (pay->id > maxId) maxId = pay->id;
        pay = pay->next;
    }
    return maxId + 1;
}

void makePayment() {
    clearScreen();
    drawModernBox(20, 5, 50, 16, " 患 者 缴 费 ");
    int patientId;
    gotoxy(25, 8); printf("请输入患者ID: "); scanf("%d", &patientId);
    Patient* p = findPatientById(patientId);
    if (!p) {
        setColor(COLOR_ERROR);
        gotoxy(25, 10); printf("患者不存在！");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }
    float totalUnpaid = 0;
    PatientFee* fee = patientFeeHead;
    while (fee) {
        if (fee->patientId == patientId && fee->status == 0) totalUnpaid += fee->totalAmount;
        fee = fee->next;
    }
    if (totalUnpaid <= 0) {
        setColor(COLOR_WARNING);
        gotoxy(25, 10); printf("该患者无欠费记录。");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }
    gotoxy(25, 10); printf("当前欠费总额: %.2f", totalUnpaid);
    gotoxy(25, 12); printf("缴费方式: 1.全部缴清  2.部分缴费: ");
    int choice;
    scanf("%d", &choice);
    float payAmount;
    if (choice == 1) payAmount = totalUnpaid;
    else {
        gotoxy(25, 14); printf("请输入缴费金额: "); scanf("%f", &payAmount);
        if (payAmount > totalUnpaid) payAmount = totalUnpaid;
    }
    gotoxy(25, 16); printf("支付方式: 1.现金 2.医保 3.微信 4.支付宝: ");
    int method;
    scanf("%d", &method);
    char methodStr[20];
    switch(method) {
        case 1: strcpy(methodStr, "现金"); break;
        case 2: strcpy(methodStr, "医保"); break;
        case 3: strcpy(methodStr, "微信"); break;
        case 4: strcpy(methodStr, "支付宝"); break;
        default: strcpy(methodStr, "其他");
    }
    Payment* payment = (Payment*)malloc(sizeof(Payment));
    payment->id = getNextPaymentId();
    payment->patientId = patientId;
    payment->amount = payAmount;
    strcpy(payment->paymentTime, getCurrentTime());
    strcpy(payment->paymentMethod, methodStr);
    gotoxy(25, 18); printf("备注(可选): ");
    getchar();
    fgets(payment->remark, MAX_DESC_LEN, stdin);
    payment->remark[strcspn(payment->remark, "\n")] = 0;
    payment->next = paymentHead;
    paymentHead = payment;
    float remaining = payAmount;
    PatientFee* feeToUpdate = patientFeeHead;
    while (feeToUpdate && remaining > 0) {
        if (feeToUpdate->patientId == patientId && feeToUpdate->status == 0) {
            if (remaining >= feeToUpdate->totalAmount) {
                remaining -= feeToUpdate->totalAmount;
                feeToUpdate->status = 1;
            } else break;
        }
        feeToUpdate = feeToUpdate->next;
    }
    clearScreen();
    setColor(COLOR_SUCCESS);
    printf("\n\n  ========== 缴费成功！ ==========\n");
    printf("    缴费金额: %.2f\n", payAmount);
    printf("    缴费时间: %s\n", payment->paymentTime);
    printf("    支付方式: %s\n", methodStr);
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void makePaymentForPatient(int patientId) {
    Patient* p = findPatientById(patientId);
    if (!p) return;
    float totalUnpaid = 0;
    PatientFee* fee = patientFeeHead;
    while (fee) {
        if (fee->patientId == patientId && fee->status == 0) totalUnpaid += fee->totalAmount;
        fee = fee->next;
    }
    if (totalUnpaid <= 0) return;
    printf("\n当前欠费总额: %.2f\n", totalUnpaid);
    printf("请选择缴费方式:\n1. 全部缴清\n2. 部分缴费\n请选择: ");
    int choice;
    scanf("%d", &choice);
    float payAmount;
    if (choice == 1) payAmount = totalUnpaid;
    else {
        printf("请输入缴费金额: ");
        scanf("%f", &payAmount);
        if (payAmount > totalUnpaid) payAmount = totalUnpaid;
    }
    printf("请选择支付方式:\n1. 现金\n2. 医保\n3. 微信\n4. 支付宝\n");
    int method;
    scanf("%d", &method);
    char methodStr[20];
    switch(method) {
        case 1: strcpy(methodStr, "现金"); break;
        case 2: strcpy(methodStr, "医保"); break;
        case 3: strcpy(methodStr, "微信"); break;
        case 4: strcpy(methodStr, "支付宝"); break;
        default: strcpy(methodStr, "其他");
    }
    Payment* payment = (Payment*)malloc(sizeof(Payment));
    payment->id = getNextPaymentId();
    payment->patientId = patientId;
    payment->amount = payAmount;
    strcpy(payment->paymentTime, getCurrentTime());
    strcpy(payment->paymentMethod, methodStr);
    printf("请输入备注(可选): ");
    getchar();
    fgets(payment->remark, MAX_DESC_LEN, stdin);
    payment->remark[strcspn(payment->remark, "\n")] = 0;
    payment->next = paymentHead;
    paymentHead = payment;
    float remaining = payAmount;
    PatientFee* feeToUpdate = patientFeeHead;
    while (feeToUpdate && remaining > 0) {
        if (feeToUpdate->patientId == patientId && feeToUpdate->status == 0) {
            if (remaining >= feeToUpdate->totalAmount) {
                remaining -= feeToUpdate->totalAmount;
                feeToUpdate->status = 1;
            } else break;
        }
        feeToUpdate = feeToUpdate->next;
    }
    printf("\n缴费成功！\n缴费金额: %.2f\n缴费时间: %s\n支付方式: %s\n", payAmount, payment->paymentTime, methodStr);
}

void listPayments() {
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ==================== 缴 费 记 录 列 表 ====================\n\n");
    setColor(COLOR_DEFAULT);
    Payment* pay = paymentHead;
    if (!pay) {
        printf("  暂无缴费记录。\n");
        pressAnyKey();
        return;
    }
    const char* headers[] = {"ID", "患者ID", "金额", "支付方式", "缴费时间", "备注"};
    int widths[] = {5, 7, 10, 10, 20, 20};
    printTableHeader(headers, widths, 6);
    int row = 0;
    while (pay) {
        char idStr[8], pidStr[8], amtStr[12];
        sprintf(idStr, "%d", pay->id);
        sprintf(pidStr, "%d", pay->patientId);
        sprintf(amtStr, "%.2f", pay->amount);
        const char* values[] = {idStr, pidStr, amtStr, pay->paymentMethod, pay->paymentTime, pay->remark};
        printTableRow(values, widths, 6, (row % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD);
        pay = pay->next;
        row++;
    }
    pressAnyKey();
}

void listPaymentsByPatient() {
    clearScreen();
    drawModernBox(20, 5, 45, 8, " 查 询 缴 费 记 录 ");
    int patientId;
    gotoxy(25, 8); printf("请输入患者ID: "); scanf("%d", &patientId);
    Patient* p = findPatientById(patientId);
    if (!p) {
        setColor(COLOR_ERROR);
        gotoxy(25, 10); printf("患者不存在！");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ========== 患者 %s (ID:%d) 缴费记录 ==========\n\n", p->name, patientId);
    setColor(COLOR_DEFAULT);
    const char* headers[] = {"ID", "金额", "支付方式", "缴费时间", "备注"};
    int widths[] = {5, 10, 10, 20, 20};
    printTableHeader(headers, widths, 5);
    float totalPaid = 0;
    Payment* pay = paymentHead;
    int row = 0;
    while (pay) {
        if (pay->patientId == patientId) {
            char idStr[8], amtStr[12];
            sprintf(idStr, "%d", pay->id);
            sprintf(amtStr, "%.2f", pay->amount);
            const char* values[] = {idStr, amtStr, pay->paymentMethod, pay->paymentTime, pay->remark};
            printTableRow(values, widths, 5, (row % 2 == 0) ? COLOR_ROW_EVEN : COLOR_ROW_ODD);
            totalPaid += pay->amount;
            row++;
        }
        pay = pay->next;
    }
    printf("\n");
    setColor(COLOR_TITLE);
    printf("  累计缴费总额: %.2f\n", totalPaid);
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void showPatientBalance() {
    clearScreen();
    drawModernBox(20, 5, 45, 10, " 费 用 汇 总 ");
    int patientId;
    gotoxy(25, 8); printf("请输入患者ID: "); scanf("%d", &patientId);
    Patient* p = findPatientById(patientId);
    if (!p) {
        setColor(COLOR_ERROR);
        gotoxy(25, 10); printf("患者不存在！");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }
    float totalFee = 0, totalPaid = 0;
    PatientFee* fee = patientFeeHead;
    while (fee) {
        if (fee->patientId == patientId) totalFee += fee->totalAmount;
        fee = fee->next;
    }
    Payment* pay = paymentHead;
    while (pay) {
        if (pay->patientId == patientId) totalPaid += pay->amount;
        pay = pay->next;
    }
    float balance = totalFee - totalPaid;
    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ========== 患者 %s (ID:%d) 费用汇总 ==========\n\n", p->name, patientId);
    setColor(COLOR_DEFAULT);
    printf("  总费用: %.2f\n", totalFee);
    printf("  已缴费: %.2f\n", totalPaid);
    printf("  欠费金额: ");
    if (balance > 0) { setColor(COLOR_ERROR); printf("%.2f\n", balance); }
    else if (balance < 0) { setColor(COLOR_SUCCESS); printf("预存余额 %.2f\n", -balance); }
    else { setColor(COLOR_SUCCESS); printf("已结清\n"); }
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}

void settlePatientAccount() {
    clearScreen();
    drawModernBox(20, 5, 55, 18, " 出 院 结 算 ");
    int patientId;
    gotoxy(25, 8); printf("请输入要结算的患者ID: "); scanf("%d", &patientId);
    Patient* p = findPatientById(patientId);
    if (!p) {
        setColor(COLOR_ERROR);
        gotoxy(25, 10); printf("患者不存在！");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }

    if (p->type == 1) {
        gotoxy(25, 10); printf("是否为住院患者结算? (1是/0否): ");
        int confirm;
        scanf("%d", &confirm);
        if (confirm == 1) {
            int days;
            gotoxy(25, 12); printf("请输入住院天数: "); scanf("%d", &days);
            float bedFeePerDay = 50.0;
            Ward* w = wardHead;
            while (w) {
                Bed* b = w->bedList;
                while (b) {
                    if (b->bedId == p->bedId && b->status == 1) {
                        if (strcmp(w->type, "VIP") == 0) bedFeePerDay = 200.0;
                        else if (strcmp(w->type, "双人") == 0) bedFeePerDay = 80.0;
                        break;
                    }
                    b = b->next;
                }
                w = w->next;
            }
            PatientFee* fee = (PatientFee*)malloc(sizeof(PatientFee));
            fee->id = getNextFeeId();
            fee->patientId = patientId;
            fee->feeItemId = 888;
            fee->quantity = days;
            fee->unitPrice = bedFeePerDay;
            fee->totalAmount = bedFeePerDay * days;
            strcpy(fee->chargeTime, getCurrentTime());
            fee->status = 0;
            fee->recordId = 0;
            fee->next = patientFeeHead;
            patientFeeHead = fee;
            setColor(COLOR_SUCCESS);
            gotoxy(25, 14); printf("已添加床位费: %.2f", fee->totalAmount);
            setColor(COLOR_DEFAULT);
        }
    }

    listUnpaidFeesByPatientId(patientId);
    float totalUnpaid = 0;
    PatientFee* fee = patientFeeHead;
    while (fee) {
        if (fee->patientId == patientId && fee->status == 0)
            totalUnpaid += fee->totalAmount;
        fee = fee->next;
    }
    if (totalUnpaid > 0) {
        printf("\n当前欠费总额: %.2f\n", totalUnpaid);
        makePaymentForPatient(patientId);
        float remainingUnpaid = 0;
        fee = patientFeeHead;
        while (fee) {
            if (fee->patientId == patientId && fee->status == 0)
                remainingUnpaid += fee->totalAmount;
            fee = fee->next;
        }
        if (remainingUnpaid > 0) {
            setColor(COLOR_ERROR);
            printf("\n警告: 仍有 %.2f 欠费未结清！\n", remainingUnpaid);
            setColor(COLOR_DEFAULT);
        } else {
            setColor(COLOR_SUCCESS);
            printf("\n结算完成！\n");
            if (p->type == 1 && p->bedId > 0) {
                freeBed(p->bedId);
                p->bedId = 0;
                p->type = 0;
                printf("已释放床位，患者状态已改为门诊。\n");
            }
            setColor(COLOR_DEFAULT);
        }
    } else {
        setColor(COLOR_WARNING);
        printf("\n该患者无欠费，无需结算。\n");
        setColor(COLOR_DEFAULT);
    }
    pressAnyKey();
}

void printInvoice() {
    clearScreen();
    drawModernBox(20, 5, 50, 8, " 打 印 发 票 ");
    int patientId;
    gotoxy(25, 8); printf("请输入患者ID: "); scanf("%d", &patientId);
    Patient* p = findPatientById(patientId);
    if (!p) {
        setColor(COLOR_ERROR);
        gotoxy(25, 10); printf("患者不存在！");
        setColor(COLOR_DEFAULT);
        pressAnyKey();
        return;
    }

    clearScreen();
    setColor(COLOR_TITLE);
    printf("\n  ╔══════════════════════════════════════════════════════════╗\n");
    printf("  ║                   医院费用结算清单                        ║\n");
    printf("  ╠══════════════════════════════════════════════════════════╣\n");
    setColor(COLOR_DEFAULT);

    char genderStr[8];
    if (p->gender == 'M' || p->gender == 'm') strcpy(genderStr, "男");
    else if (p->gender == 'F' || p->gender == 'f') strcpy(genderStr, "女");
    else strcpy(genderStr, "未知");

    printf("  ║  患者姓名：%-10s     患者ID：%-6d                   ║\n", p->name, p->id);
    printf("  ║  性别：%-4s          年龄：%-4d                                ║\n", genderStr, p->age);
    printf("  ║  患者类型：%-8s                                            ║\n", p->type == 0 ? "门诊" : "住院");
    printf("  ╠══════════════════════════════════════════════════════════╣\n");
    printf("  ║ 序号  费用项目       数量   单价     金额     状态        ║\n");
    printf("  ╠══════════════════════════════════════════════════════════╣\n");

    PatientFee* feeArray[100];
    int feeCount = 0;
    PatientFee* fee = patientFeeHead;
    while (fee) {
        if (fee->patientId == patientId) feeArray[feeCount++] = fee;
        fee = fee->next;
    }

    float total = 0, paid = 0;
    if (feeCount == 0) {
        printf("  ║                  暂无费用记录                            ║\n");
    } else {
        for (int i = feeCount - 1; i >= 0; i--) {
            fee = feeArray[i];
            char itemName[20];
            if (fee->feeItemId == 888) strcpy(itemName, "床位费");
            else if (fee->feeItemId == 999) strcpy(itemName, "药品费");
            else {
                FeeItem* item = findFeeItemById(fee->feeItemId);
                if (item) strcpy(itemName, item->name);
                else strcpy(itemName, "其他");
            }
            char statusStr[8];
            strcpy(statusStr, fee->status == 0 ? "未付" : "已付");
            printf("  ║ %-4d  %-12s   %-4d  %7.2f  %7.2f   %-4s      ║\n",
                   feeCount - i, itemName, fee->quantity,
                   fee->unitPrice, fee->totalAmount, statusStr);
            total += fee->totalAmount;
            if (fee->status == 1) paid += fee->totalAmount;
        }
    }

    printf("  ╠══════════════════════════════════════════════════════════╣\n");
    printf("  ║  费用合计：%-8.2f  已付金额：%-8.2f  应付金额：%-8.2f ║\n", total, paid, total - paid);
    printf("  ╚══════════════════════════════════════════════════════════╝\n");

    setColor(COLOR_WARNING);
    printf("\n  >>> 按任意键返回 <<<");
    setColor(COLOR_DEFAULT);
    pressAnyKey();
}