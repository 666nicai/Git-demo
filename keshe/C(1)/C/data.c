// ============================================================
// 文件名: data.c
// 描述: 数据初始化、文件加载与保存 (满足数量要求)
// ============================================================

#include "his.h"
#include <assert.h>
#include "user.h"
// 全局链表头指针定义
Department* deptHead = NULL;
Doctor* doctorHead = NULL;
Patient* patientHead = NULL;
MedicalRecord* recordHead = NULL;
Ward* wardHead = NULL;
Medicine* medicineHead = NULL;
Prescription* prescriptionHead = NULL;
FeeItem* feeItemHead = NULL;
PatientFee* patientFeeHead = NULL;
Payment* paymentHead = NULL;

// 初始化默认数据，确保数量满足要求
void initDefaultData() {
    // 1. 初始化科室 (5个)
    Department* d;
    int i;
    char* deptNames[] = {"内科", "外科", "儿科", "妇产科", "急诊科"};
    for (i = 0; i < 5; i++) {
        d = (Department*)malloc(sizeof(Department));
        d->id = i+1;
        strcpy(d->name, deptNames[i]);
        d->wardType = 1;               // 拷贝科室名称
        d->leaderId = (i*4)+1;
        d->next = deptHead;
        deptHead = d;
    }

    // 2. 初始化医生 (20名)
    char* docNames[] = {"张伟", "李强", "王芳", "刘敏", "陈静", "赵磊", "周婷", "吴刚", "郑洁", "林晨",
                        "郭峰", "唐雅", "孙鹏", "宋阳", "徐璐", "黄娟", "马超", "朱琳", "秦昊", "顾佳"};
    int deptAssign[] = {1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,1,2,3,4,5};

    // 职称候选池（5种）
    char* titles[] = {"主任医师", "副主任医师", "主治医师", "医师", "住院医师"};
    // 出诊时间候选池（20种，每人独有）
    char* shifts[] = {
        "周一全天", "周二全天", "周三全天", "周四全天", "周五全天",
        "周一、三上午", "周二、四下午", "周一至周五上午", "周二、四全天",
        "周三、五下午", "周一、三、五全天", "周二、四上午", "周四、五下午",
        "周一上午、周三下午", "周二全天、周五上午", "仅周三上午", "仅周四下午",
        "周一全天、周四上午", "周二上午、周五下午", "周三、四全天"
    };

    for (i = 0; i < 20; i++) {
        Doctor* doc = (Doctor*)malloc(sizeof(Doctor)); // 为单个医生节点分配内存
        doc->id = i+1;
        strcpy(doc->name, docNames[i]);
        doc->deptId = deptAssign[i];
        strcpy(doc->title, titles[i % 5]);      // 循环使用5种职称，避免单调
        strcpy(doc->workTime, shifts[i]);       // 每人一种独特出诊时间
        doc->next = doctorHead;
        doctorHead = doc;
    }

    // 3. 初始化患者: 100名门诊 + 30名住院 (使用姓名池避免重复)
    char* patientNamePool[130] = {
        "王小明","李丽","张强","刘芳","陈涛","周敏","吴磊","郑爽","林娜","郭鹏",
        "唐雅欣","孙浩","宋茜","徐晨","黄伟","马晓","朱莉","秦岚","顾磊","欧阳雪",
        "赵刚","王磊","李娜","张勇","刘洋","陈静","周杰","吴迪","郑欣","林峰",
        "郭涛","唐雅","孙莉","宋阳","徐静","黄鑫","马超","朱敏","秦汉","顾欣",
        "王博","李响","张宇","刘慧","陈飞","周莹","吴凡","郑爽","林浩","郭峰",
        "唐艺","孙鹏","宋佳","徐凯","黄娟","马丽","朱超","秦雪","顾洋","欧阳静",
        "赵宇","王莹","李想","张旭","刘晶","陈阳","周璐","吴涛","郑宇","林欣",
        "郭瑞","唐静","孙宇","宋波","徐梅","黄亮","马杰","朱涛","秦怡","顾涛",
        "王娜","李斌","张蕾","刘旭","陈思","周洋","吴婷","郑凯","林杰","郭旭",
        "唐莉","孙昊","宋妍","徐昊","黄璐","马欣","朱旭","秦璐","顾阳","欧阳旭",
        "赵鹏","王洁","李旭","张欣","刘宁","陈旭","周欣","吴旭","郑旭","林旭",
        "郭旭","唐旭","孙旭","宋旭","徐旭","黄旭","马旭","朱旭","秦旭","顾旭",
        "王旭","李旭","张旭","刘旭","陈旭","周旭","吴旭","郑旭","林旭","郭旭"
    };

    for (i = 0; i < 130; i++) {
        Patient* p = (Patient*)malloc(sizeof(Patient));
        p->id = i+1;
        strcpy(p->name, patientNamePool[i]);
        p->gender = (i%2==0)?'M':'F';           // 性别：偶数为男(M)，奇数为女(F)
        p->age = 18 + (i%70);                   // 年龄：18-88岁随机分布
        sprintf(p->phone, "138%08d", i);        // 生成手机号：138开头+8位数字
        strcpy(p->creatTime, getCurrentTime());
        if (i < 100) {
            p->type = 0;            // 0 = 门诊患者
            p->bedId = 0;           // 门诊患者无床位ID
        } else {
            p->type = 1;            // 1 = 住院患者
            p->bedId = 0;           // 先初始化床位ID为0，后续分配
        }
        p->next = patientHead;
        patientHead = p;
    }

    // 4. 初始化病房与床位
    Ward* w;
    int wardId = 1;
    char* wardTypes[] = {"普通", "双人", "VIP"};
    int deptWard[] = {1,1,2,2,3,4,5};
    int totalBedsEach[] = {10,8,8,6,5,5,5};
    for (i = 0; i < 7; i++) {
        w = (Ward*)malloc(sizeof(Ward));
        w->id = wardId++;
        strcpy(w->type, wardTypes[i%3]);           // 循环分配病房类型
        w->deptId = deptWard[i];
        w->totalBeds = totalBedsEach[i];
        w->bedList = NULL;
        for (int j = 1; j <= w->totalBeds; j++) {  // 为当前病房创建对应数量的床位节点
            Bed* b = (Bed*)malloc(sizeof(Bed));    // 为单个床位节点分配内存
            b->bedId = (w->id-1)*20 + j;  // 生成床位ID（按病房ID分段）
            b->wardId = w->id;            // 关联所属病房ID
            b->patientId = 0;             // 初始无患者入住
            b->status = 0;                // 床位状态：0=空闲，1=占用
            b->next = w->bedList;
            w->bedList = b;
        }
        w->next = wardHead;
        wardHead = w;
    }
    // 为住院患者分配床位
    Patient* p = patientHead;
    int assigned = 0;                    // 已分配床位的患者计数
    for (p = patientHead; p != NULL && assigned < 30; p = p->next) {
        if (p->type == 1) {
            Bed* freeBed = NULL;
            Ward* ward;
            for (ward = wardHead; ward != NULL; ward = ward->next) { // 遍历所有病房，寻找空闲床位
                Bed* bed;
                for (bed = ward->bedList; bed != NULL; bed = bed->next) {
                    if (bed->status == 0) {
                        freeBed = bed;
                        break;
                    }
                }
                if (freeBed) break;
            }
            if (freeBed) { // 如果找到空闲床位，完成分配
                freeBed->status = 1;        // 标记床位为占用
                freeBed->patientId = p->id; // 关联患者ID
                p->bedId = freeBed->bedId;  // 患者关联床位ID
                assigned++;                 // 已分配计数+1
            }
        }
    }

    // 5. 药品: 20类
    char* commonNames[] = {"阿莫西林", "布洛芬", "维生素C", "头孢克肟", "奥美拉唑", "对乙酰氨基酚", "盐酸二甲双胍", "硝苯地平", "氨溴索", "氯雷他定",
                           "阿奇霉素", "左氧氟沙星", "复方丹参滴丸", "板蓝根", "感冒灵", "止咳糖浆", "蒙脱石散", "益生菌", "胰岛素", "葡萄糖酸钙"};
    for (i = 0; i < 20; i++) {
        Medicine* m = (Medicine*)malloc(sizeof(Medicine));
        m->id = i+1;
        strcpy(m->commonName, commonNames[i]);
        sprintf(m->tradeName, "%s%s", commonNames[i], "片"); // 生成商品名（通用名+片）
        strcpy(m->alias, commonNames[i]);                    // 药品别名（暂同通用名）
         m->stock = 100 + (i*50);    // 库存数量：基础100，每类递增50
        m->price = 5.0 + i*2.5;     // 单价：基础5元，每类递增2.5元
        m->deptId = (i%5)+1;        // 所属科室ID（循环分配5个科室）
        int x = rand()%2+1;         // 随机数1-2，用于分配单位
        if(x==1) strcpy(m->unit, "盒");         // 1=盒
        else strcpy(m->unit, "瓶");             // 2=瓶
        int month = rand()%12+1;
        int day = rand()%28+1;
        sprintf(m->expireDate, "%02d-%02d", month, day);
        m->next = medicineHead;
        medicineHead = m;
    }

    // 6. 医疗记录
    int recId = 1;
    int doctorQueueCounter[21] = {0};
    for (p = patientHead; p != NULL; p = p->next) {
        int docId = 1 + (p->id % 20);    // 分配接诊医生ID（患者ID取模20+1）
        // 挂号记录
        MedicalRecord* rec = (MedicalRecord*)malloc(sizeof(MedicalRecord));
        rec->id = recId++;       // 记录ID自增
        rec->patientId = p->id;
        rec->doctorId = docId;
        if (p->type == 0) {
            strcpy(rec->recordType, "现场挂号");
            strcpy(rec->appointmentTime, "今日就诊");
        } else {
            strcpy(rec->recordType, "预约挂号");
            strcpy(rec->appointmentTime, "2025-04-15");
        }
        strcpy(rec->recordTime, getCurrentTime());
        doctorQueueCounter[docId]++;                  // 对应医生排队号+1
        rec->queueNumber = doctorQueueCounter[docId];
        rec->status = 0;                            // 挂号状态：0=未就诊
        strcpy(rec->diagnosis, "");                 // 诊断结果初始为空
        rec->next = recordHead;
        recordHead = rec;
        // 看诊记录
        MedicalRecord* rec2 = (MedicalRecord*)malloc(sizeof(MedicalRecord));
        rec2->id = recId++;
        rec2->patientId = p->id;
        rec2->doctorId = docId;
        strcpy(rec2->recordType, "看诊");
        strcpy(rec2->recordTime, getCurrentTime());
        strcpy(rec2->appointmentTime, "");
        rec2->queueNumber = 0;
        rec2->status = 1;
        strcpy(rec2->diagnosis, "普通感冒，注意休息");
        rec2->next = recordHead;
        recordHead = rec2;
    }

    // 7. 处方示例
    for (i = 1; i <= 20; i++) {
        Prescription* pre = (Prescription*)malloc(sizeof(Prescription)); // 为单个处方节点分配内存
        pre->id = i;
        pre->recordId = i;
        pre->patientId = i;
        pre->doctorId = (i%20)+1; // 关联医生ID（循环1-20）
        strcpy(pre->issueTime, getCurrentTime());
        PrescriptionItem* item = (PrescriptionItem*)malloc(sizeof(PrescriptionItem)); // 创建处方明细项（单条药品）
        item->medicineId = (i%20)+1;
        item->quantity = 2;
        item->next = NULL;
        pre->items = item;              // 处方关联明细项
        pre->next = prescriptionHead;
        prescriptionHead = pre;
        Medicine* med = findMedicineById(item->medicineId);
        if (med) med->stock -= item->quantity;// 扣减对应药品库存
    }

    // 8. 费用项目初始化
    char* feeNames[] = {"挂号费", "诊查费", "血常规", "CT检查", "床位费普通", "床位费双人", "床位费VIP"};
    float feePrices[] = {10.0, 20.0, 50.0, 300.0, 50.0, 80.0, 200.0};
    for (i = 0; i < 7; i++) {
        FeeItem* item = (FeeItem*)malloc(sizeof(FeeItem));
        item->id = i+1;
        strcpy(item->name, feeNames[i]);  // 拷贝费用项名称
        item->price = feePrices[i];       // 设置单价
        strcpy(item->category, (i<2)?"诊疗":((i<4)?"检查":"住院")); // 分类标记：前2项=诊疗，中间2项=检查，剩余=住院
        item->next = feeItemHead;
        feeItemHead = item;
    }

    // 示例费用数据
    PatientFee* pf1 = (PatientFee*)malloc(sizeof(PatientFee));// 第一条费用明细
    pf1->id = 1;                                // 费用明细ID
    pf1->patientId = 1;                         // 关联患者ID
    pf1->feeItemId = 1;                         // 关联费用项ID（挂号费）
    pf1->quantity = 1;                          // 数量=1
    pf1->unitPrice = 10.0;                      // 单价=10元
    pf1->totalAmount = 10.0;                    // 总金额=10元
    strcpy(pf1->chargeTime, getCurrentTime());  // 计费时间
    pf1->status = 1;                            // 支付状态：1=已支付
    pf1->recordId = 1;                          // 关联医疗记录ID
    pf1->next = patientFeeHead;
    patientFeeHead = pf1;

    PatientFee* pf2 = (PatientFee*)malloc(sizeof(PatientFee));// 第二条
    pf2->id = 2;
    pf2->patientId = 1;
    pf2->feeItemId = 2;
    pf2->quantity = 1;
    pf2->unitPrice = 20.0;
    pf2->totalAmount = 20.0;
    strcpy(pf2->chargeTime, getCurrentTime());
    pf2->status = 0;
    pf2->recordId = 1;
    pf2->next = patientFeeHead;
    patientFeeHead = pf2;

    PatientFee* pf3 = (PatientFee*)malloc(sizeof(PatientFee));// 第三条
    pf3->id = 3;
    pf3->patientId = 2;
    pf3->feeItemId = 999;
    pf3->quantity = 1;
    pf3->unitPrice = 35.5;
    pf3->totalAmount = 35.5;
    strcpy(pf3->chargeTime, getCurrentTime());
    pf3->status = 0;
    pf3->recordId = 2;
    pf3->next = patientFeeHead;
    patientFeeHead = pf3;
        // 初始化默认用户（管理员）
    initDefaultUsers();
}

int loadAllData() {
    FILE* fp;
    char line[1024];
    int loaded = 0;
    
    // 1. 加载科室数据
    fp = fopen("test_data/departments.txt", "r");
    if (fp) {
        printf("[OK] 读取科室数据成功\n");
        fgets(line, sizeof(line), fp);
        while (fgets(line, sizeof(line), fp)) {
            Department* d = (Department*)malloc(sizeof(Department));
            sscanf(line, "%d|%[^|]|%d|%d", &d->id, d->name, &d->wardType, &d->leaderId);
            d->next = deptHead;
            deptHead = d;
            loaded = 1;
        }
        fclose(fp);
    } else {
        printf("[FAIL] 无法打开 test_data/departments.txt\n");
    }
    
    // 2. 加载医生数据
    fp = fopen("test_data/doctors.txt", "r");
    if (fp) {
        printf("[OK] 读取医生数据成功\n");
        fgets(line, sizeof(line), fp);
        while (fgets(line, sizeof(line), fp)) {
            Doctor* doc = (Doctor*)malloc(sizeof(Doctor));
            sscanf(line, "%d|%[^|]|%d|%[^|]|%[^\n]", 
                   &doc->id, doc->name, &doc->deptId, doc->title, doc->workTime);
            doc->next = doctorHead;
            doctorHead = doc;
            loaded = 1;
        }
        fclose(fp);
    } else {
        printf("[FAIL] 无法打开 test_data/doctors.txt\n");
    }
    
    // 3. 加载患者数据
    fp = fopen("test_data/patients.txt", "r");
    if (fp) {
        printf("[OK] 读取患者数据成功\n");
        fgets(line, sizeof(line), fp);
        while (fgets(line, sizeof(line), fp)) {
            Patient* p = (Patient*)malloc(sizeof(Patient));
            sscanf(line, "%d|%[^|]|%c|%d|%[^|]|%d|%d|%[^\n]",
                   &p->id, p->name, &p->gender, &p->age, p->phone, &p->type, &p->bedId, p->creatTime);
            p->next = patientHead;
            patientHead = p;
            loaded = 1;
        }
        fclose(fp);
    } else {
        printf("[FAIL] 无法打开 test_data/patients.txt\n");
    }
    
    // 4. 加载药品数据
    fp = fopen("test_data/medicines.txt", "r");
    if (fp) {
        printf("[OK] 读取药品数据成功\n");
        fgets(line, sizeof(line), fp);
        while (fgets(line, sizeof(line), fp)) {
            Medicine* m = (Medicine*)malloc(sizeof(Medicine));
            sscanf(line, "%d|%[^|]|%[^|]|%[^|]|%d|%f|%d|%[^|]|%[^\n]",
                   &m->id, m->commonName, m->tradeName, m->alias,
                   &m->stock, &m->price, &m->deptId, m->unit, m->expireDate);
            m->next = medicineHead;
            medicineHead = m;
            loaded = 1;
        }
        fclose(fp);
    } else {
        printf("[FAIL] 无法打开 test_data/medicines.txt\n");
    }
    
    // 5. 加载费用项目
    fp = fopen("test_data/fee_items.txt", "r");
    if (fp) {
        printf("[OK] 读取费用项目成功\n");
        fgets(line, sizeof(line), fp);
        while (fgets(line, sizeof(line), fp)) {
            FeeItem* item = (FeeItem*)malloc(sizeof(FeeItem));
            sscanf(line, "%d|%[^|]|%f|%[^\n]", &item->id, item->name, &item->price, item->category);
            item->next = feeItemHead;
            feeItemHead = item;
            loaded = 1;
        }
        fclose(fp);
    } else {
        printf("[FAIL] 无法打开 test_data/fee_items.txt\n");
    }
    
    // 6. 加载医疗记录
    fp = fopen("test_data/medical_records.txt", "r");
    if (fp) {
        printf("[OK] 读取医疗记录成功\n");
        fgets(line, sizeof(line), fp);
        while (fgets(line, sizeof(line), fp)) {
            MedicalRecord* rec = (MedicalRecord*)malloc(sizeof(MedicalRecord));
            sscanf(line, "%d|%d|%d|%[^|]|%[^|]|%[^|]|%d|%d|%[^\n]",
                   &rec->id, &rec->patientId, &rec->doctorId,
                   rec->recordType, rec->recordTime, rec->appointmentTime,
                   &rec->queueNumber, &rec->status, rec->diagnosis);
            rec->next = recordHead;
            recordHead = rec;
            loaded = 1;
        }
        fclose(fp);
    } else {
        printf("[FAIL] 无法打开 test_data/medical_records.txt\n");
    }
    
    // 7. 加载处方数据
    fp = fopen("test_data/prescriptions.txt", "r");
    if (fp) {
        printf("[OK] 读取处方数据成功\n");
        fgets(line, sizeof(line), fp);
        while (fgets(line, sizeof(line), fp)) {
            Prescription* pre = (Prescription*)malloc(sizeof(Prescription));
            char medsStr[512];
            sscanf(line, "%d|%d|%d|%d|%[^|]|%[^\n]",
                   &pre->id, &pre->recordId, &pre->patientId, &pre->doctorId,
                   pre->issueTime, medsStr);
            pre->items = NULL;
            char* token = strtok(medsStr, ";");
            while (token) {
                PrescriptionItem* item = (PrescriptionItem*)malloc(sizeof(PrescriptionItem));
                sscanf(token, "%d:%d", &item->medicineId, &item->quantity);
                item->next = pre->items;
                pre->items = item;
                token = strtok(NULL, ";");
            }
            pre->next = prescriptionHead;
            prescriptionHead = pre;
            loaded = 1;
        }
        fclose(fp);
    } else {
        printf("[FAIL] 无法打开 test_data/prescriptions.txt\n");
    }
    
    // 8. 加载缴费记录
    fp = fopen("test_data/payments.txt", "r");
    if (fp) {
        printf("[OK] 读取缴费记录成功\n");
        fgets(line, sizeof(line), fp);
        while (fgets(line, sizeof(line), fp)) {
            Payment* pay = (Payment*)malloc(sizeof(Payment));
            sscanf(line, "%d|%d|%f|%[^|]|%[^|]|%[^\n]",
                   &pay->id, &pay->patientId, &pay->amount,
                   pay->paymentTime, pay->paymentMethod, pay->remark);
            pay->next = paymentHead;
            paymentHead = pay;
            loaded = 1;
        }
        fclose(fp);
    } else {
        printf("[FAIL] 无法打开 test_data/payments.txt\n");
    }
    
    // 9. 加载病房床位数据
    fp = fopen("test_data/wards_beds.txt", "r");
    if (fp) {
        printf("[OK] 读取病房床位数据成功\n");
        fgets(line, sizeof(line), fp);
        while (fgets(line, sizeof(line), fp)) {
            Ward* w = (Ward*)malloc(sizeof(Ward));
            char bedListStr[256];
            sscanf(line, "%d|%[^|]|%d|%d|%[^\n]",
                   &w->id, w->type, &w->deptId, &w->totalBeds, bedListStr);
            w->bedList = NULL;
            char* token = strtok(bedListStr, ",");
            while (token) {
                Bed* b = (Bed*)malloc(sizeof(Bed));
                b->bedId = atoi(token);
                b->wardId = w->id;
                b->patientId = 0;
                b->status = 0;
                b->next = w->bedList;
                w->bedList = b;
                token = strtok(NULL, ",");
            }
            w->next = wardHead;
            wardHead = w;
            loaded = 1;
        }
        fclose(fp);
    } else {
        printf("[FAIL] 无法打开 test_data/wards_beds.txt\n");
    }
    
    // 10. 加载用户数据
       
       
    fp = fopen("test_data/users.txt", "r");
    if (fp) {
        printf("[OK] 读取用户数据成功\n");
        fgets(line, sizeof(line), fp);
        while (fgets(line, sizeof(line), fp)) {
            User* u = (User*)malloc(sizeof(User));
            char passwordRaw[65];
            int roleInt;
            sscanf(line, "%d|%[^|]|%d|%d|%[^\n]",
                   &u->id, u->username, &roleInt, &u->relatedId, passwordRaw);
            u->role = (UserRole)roleInt;
            sha256(passwordRaw, u->password);
            u->next = userHead;
            userHead = u;
            loaded = 1;
        }
        fclose(fp);
    } else {
        printf("[FAIL] 无法打开 test_data/users.txt\n");
    }
    
    if (loaded) {
        printf("\n===== 所有数据从 txt 文件加载成功！ =====\n");
        printf("按任意键进入系统...\n");
        getchar();
        return 1;
    }
    
    return 0;
}
void saveAllData() {
    // 占位
}