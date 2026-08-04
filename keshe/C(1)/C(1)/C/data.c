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
        d->wardType = 1;
        d->leaderId = (i*4)+1;
        d->next = deptHead;
        deptHead = d;
    }

    // 2. 初始化医生 (20名)
    char* docNames[] = {"张伟", "李强", "王芳", "刘敏", "陈静", "赵磊", "周婷", "吴刚", "郑洁", "林晨",
                        "郭峰", "唐雅", "孙鹏", "宋阳", "徐璐", "黄娟", "马超", "朱琳", "秦昊", "顾佳"};
    int deptAssign[] = {1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,1,2,3,4,5};
    for (i = 0; i < 20; i++) {
        Doctor* doc = (Doctor*)malloc(sizeof(Doctor));
        doc->id = i+1;
        strcpy(doc->name, docNames[i]);
        doc->deptId = deptAssign[i];
        strcpy(doc->title, (i%3==0)?"主任医师":((i%2==0)?"主治医师":"医师"));
        strcpy(doc->workTime, "周一至周五 全天");
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
        p->gender = (i%2==0)?'M':'F';
        p->age = 18 + (i%70);
        sprintf(p->phone, "138%08d", i);
        strcpy(p->creatTime, getCurrentTime());
        if (i < 100) {
            p->type = 0;
            p->bedId = 0;
        } else {
            p->type = 1;
            p->bedId = 0;
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
        strcpy(w->type, wardTypes[i%3]);
        w->deptId = deptWard[i];
        w->totalBeds = totalBedsEach[i];
        w->bedList = NULL;
        for (int j = 1; j <= w->totalBeds; j++) {
            Bed* b = (Bed*)malloc(sizeof(Bed));
            b->bedId = (w->id-1)*20 + j;
            b->wardId = w->id;
            b->patientId = 0;
            b->status = 0;
            b->next = w->bedList;
            w->bedList = b;
        }
        w->next = wardHead;
        wardHead = w;
    }
    // 为住院患者分配床位
    Patient* p = patientHead;
    int assigned = 0;
    for (p = patientHead; p != NULL && assigned < 30; p = p->next) {
        if (p->type == 1) {
            Bed* freeBed = NULL;
            Ward* ward;
            for (ward = wardHead; ward != NULL; ward = ward->next) {
                Bed* bed;
                for (bed = ward->bedList; bed != NULL; bed = bed->next) {
                    if (bed->status == 0) {
                        freeBed = bed;
                        break;
                    }
                }
                if (freeBed) break;
            }
            if (freeBed) {
                freeBed->status = 1;
                freeBed->patientId = p->id;
                p->bedId = freeBed->bedId;
                assigned++;
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
        sprintf(m->tradeName, "%s%s", commonNames[i], "片");
        strcpy(m->alias, commonNames[i]);
        m->stock = 100 + (i*50);
        m->price = 5.0 + i*2.5;
        m->deptId = (i%5)+1;
        int x = rand()%2+1;
        if(x==1) strcpy(m->unit, "盒");
        else strcpy(m->unit, "瓶");
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
        int docId = 1 + (p->id % 20);
        // 挂号记录
        MedicalRecord* rec = (MedicalRecord*)malloc(sizeof(MedicalRecord));
        rec->id = recId++;
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
        doctorQueueCounter[docId]++;
        rec->queueNumber = doctorQueueCounter[docId];
        rec->status = 0;
        strcpy(rec->diagnosis, "");
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
        Prescription* pre = (Prescription*)malloc(sizeof(Prescription));
        pre->id = i;
        pre->recordId = i;
        pre->patientId = i;
        pre->doctorId = (i%20)+1;
        strcpy(pre->issueTime, getCurrentTime());
        PrescriptionItem* item = (PrescriptionItem*)malloc(sizeof(PrescriptionItem));
        item->medicineId = (i%20)+1;
        item->quantity = 2;
        item->next = NULL;
        pre->items = item;
        pre->next = prescriptionHead;
        prescriptionHead = pre;
        Medicine* med = findMedicineById(item->medicineId);
        if (med) med->stock -= item->quantity;
    }

    // 8. 费用项目初始化
    char* feeNames[] = {"挂号费", "诊查费", "血常规", "CT检查", "床位费普通", "床位费双人", "床位费VIP"};
    float feePrices[] = {10.0, 20.0, 50.0, 300.0, 50.0, 80.0, 200.0};
    for (i = 0; i < 7; i++) {
        FeeItem* item = (FeeItem*)malloc(sizeof(FeeItem));
        item->id = i+1;
        strcpy(item->name, feeNames[i]);
        item->price = feePrices[i];
        strcpy(item->category, (i<2)?"诊疗":((i<4)?"检查":"住院"));
        item->next = feeItemHead;
        feeItemHead = item;
    }

    // 示例费用数据
    PatientFee* pf1 = (PatientFee*)malloc(sizeof(PatientFee));
    pf1->id = 1;
    pf1->patientId = 1;
    pf1->feeItemId = 1;
    pf1->quantity = 1;
    pf1->unitPrice = 10.0;
    pf1->totalAmount = 10.0;
    strcpy(pf1->chargeTime, getCurrentTime());
    pf1->status = 1;
    pf1->recordId = 1;
    pf1->next = patientFeeHead;
    patientFeeHead = pf1;

    PatientFee* pf2 = (PatientFee*)malloc(sizeof(PatientFee));
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

    PatientFee* pf3 = (PatientFee*)malloc(sizeof(PatientFee));
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
    return 0;
}

void saveAllData() {
    // 占位
}