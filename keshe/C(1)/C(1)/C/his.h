// ============================================================
// 文件名: his.h
// 描述: 医疗管理系统全局头文件（终极美化版）
// ============================================================

#ifndef HIS_H
#define HIS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 常量定义
#define MAX_NAME_LEN 50
#define MAX_PHONE_LEN 20
#define MAX_ADDR_LEN 100
#define MAX_DESC_LEN 200
#define MAX_DRUG_NAME_LEN 80

// ========== 控制台颜色常量 ==========
#define COLOR_DEFAULT    0x07   // 默认灰底白字（根据实际背景调整）
#define COLOR_TITLE      0x0B   // 亮青色
#define COLOR_SUCCESS    0x0A   // 绿色
#define COLOR_ERROR      0x0C   // 红色
#define COLOR_WARNING    0x0E   // 黄色
#define COLOR_BORDER     0x08   // 灰色边框
#define COLOR_HEADER     0x09   // 蓝色表头
#define COLOR_ROW_EVEN   0x07   // 偶数行颜色
#define COLOR_ROW_ODD    0x08   // 奇数行颜色
#define COLOR_INPUT_BG   0x70   // 灰底黑字输入框背景

// 科室结构体
typedef struct Department {
    int id;
    char name[MAX_NAME_LEN];
    int wardType;
    int leaderId;
    struct Department* next;
} Department;

// 医生结构体
typedef struct Doctor {
    int id;
    char name[MAX_NAME_LEN];
    int deptId;
    char title[MAX_NAME_LEN];
    char workTime[MAX_NAME_LEN];
    struct Doctor* next;
} Doctor;

// 患者结构体
typedef struct Patient {
    int id;
    char name[MAX_NAME_LEN];
    char gender;
    int age;
    char phone[MAX_PHONE_LEN];
    int type;
    int bedId;
    char creatTime[MAX_NAME_LEN];
    struct Patient* next;
} Patient;

// 医疗记录结构体
typedef struct MedicalRecord {
    int id;
    int patientId;
    int doctorId;
    char recordType[20];
    char recordTime[30];
    char appointmentTime[30];
    int queueNumber;
    int status;
    char diagnosis[MAX_DESC_LEN];
    struct MedicalRecord* next;
} MedicalRecord;

// 床位结构体
typedef struct Bed {
    int bedId;
    int wardId;
    int patientId;
    int status;
    struct Bed* next;
} Bed;

// 病房结构体
typedef struct Ward {
    int id;
    char type[20];
    int deptId;
    int totalBeds;
    Bed* bedList;
    struct Ward* next;
} Ward;

// 药品结构体
typedef struct Medicine {
    int id;
    char commonName[MAX_DRUG_NAME_LEN];
    char tradeName[MAX_DRUG_NAME_LEN];
    char alias[MAX_DRUG_NAME_LEN];
    int stock;
    float price;
    int deptId;
    char unit[10];
    char expireDate[20];
    struct Medicine* next;
} Medicine;

// 处方明细项
typedef struct PrescriptionItem {
    int medicineId;
    int quantity;
    struct PrescriptionItem* next;
} PrescriptionItem;

// 处方结构体
typedef struct Prescription {
    int id;
    int recordId;
    int patientId;
    int doctorId;
    char issueTime[30];
    PrescriptionItem* items;
    struct Prescription* next;
} Prescription;

// 费用管理相关结构体
typedef struct FeeItem {
    int id;
    char name[MAX_NAME_LEN];
    float price;
    char category[20];
    struct FeeItem* next;
} FeeItem;

typedef struct PatientFee {
    int id;
    int patientId;
    int feeItemId;
    int quantity;
    float unitPrice;
    float totalAmount;
    char chargeTime[30];
    int status;
    int recordId;
    struct PatientFee* next;
} PatientFee;

typedef struct Payment {
    int id;
    int patientId;
    float amount;
    char paymentTime[30];
    char paymentMethod[20];
    char remark[MAX_DESC_LEN];
    struct Payment* next;
} Payment;

// 全局链表头指针
extern Department* deptHead;
extern Doctor* doctorHead;
extern Patient* patientHead;
extern MedicalRecord* recordHead;
extern Ward* wardHead;
extern Medicine* medicineHead;
extern Prescription* prescriptionHead;
extern FeeItem* feeItemHead;
extern PatientFee* patientFeeHead;
extern Payment* paymentHead;

// ========== 控制台美化函数 ==========
void setColor(int color);
void gotoxy(int x, int y);
void hideCursor();
void showCursor();
void drawModernBox(int x, int y, int w, int h, const char* title);
int getPassword(char* pass, int maxLen);
void printTitle();
void printTableHeader(const char* headers[], int widths[], int count);
void printTableRow(const char* values[], int widths[], int count, int rowColor);
void pressAnyKey();
void clearScreen();
char* getCurrentTime();

// 安全输入函数（限制长度，不破坏布局）
void safeInput(char* buffer, int maxLen, int x, int y);

// 数据初始化
void initDefaultData();
int loadAllData();
void saveAllData();

// 科室管理
void addDepartment();
void listDepartments();
Department* findDepartmentById(int id);
void deleteDepartment();
void updateDepartment();

// 医生管理
void addDoctor();
void listDoctors();
Doctor* findDoctorById(int id);
void listDoctorsByDept(int deptId);
void updateDoctor();
void deleteDoctor();

// 患者管理
void addPatient();
void listPatients();
Patient* findPatientById(int id);
void updatePatient();
void deletePatient();

// 医疗记录管理
void addMedicalRecord();
void listMedicalRecords();
void listRecordsByPatient(int patientId);
MedicalRecord* findRecordById(int id);

// 病房床位管理
void addWard();
void listWards();
Bed* findFreeBed(int wardId);
int allocateBed(int patientId, int wardId);
void freeBed(int bedId);
void manageBeds();

// 药品管理
void addMedicine();
void listMedicines();
Medicine* findMedicineById(int id);
void updateMedicineStock();
void deleteMedicine();

// 处方管理
void addPrescription();
void listPrescriptions();
Prescription* findPrescriptionById(int id);
void showPrescriptionDetail(Prescription* p);

// 统计报表
void reportPatientStats();
void reportDoctorWorkload();
void reportDeptStats();
void reportMedicineStock();
void reportWardUtilization();
void reportComprehensive();
void reportPatientView();

// 查询功能
void searchPatientByName();
void searchDoctorByName();
void searchMedicineByName();

// 费用管理
void addFeeItem();
void listFeeItems();
void updateFeeItem();
void deleteFeeItem();
FeeItem* findFeeItemById(int id);
void addPatientFee();
void addPrescriptionFee();
void addWardFee();
void listPatientFees();
void listFeesByPatient();
void listUnpaidFeesByPatient();
void makePayment();
void listPayments();
void listPaymentsByPatient();
void showPatientBalance();
void settlePatientAccount();
void printInvoice();

// 挂号与候诊管理
void registerAppointment();
void listQueueByDoctor();
void callNextPatient();
void completeCurrentPatient();
void showMyQueue();

// 带患者ID参数的版本
void listUnpaidFeesByPatientId(int patientId);
void makePaymentForPatient(int patientId);

// JSON 输出函数
void outputStatsJson();
void outputPatientsJson();
void outputDoctorsJson();
void outputQueueJson(int doctorId);
void outputFeesJson(int patientId);
void outputUnpaidFeesJson(int patientId);
void outputInvoiceJson(int patientId);
void outputCallNextJson(int doctorId);

// 用户管理
#include "user.h"

#endif