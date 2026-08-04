// ============================================================
// 文件名: user.h
// 描述: 用户管理模块 (注册、登录)
// ============================================================

#ifndef USER_H
#define USER_H

#include "his.h"

// 用户角色枚举
typedef enum {
    ROLE_ADMIN,
    ROLE_DOCTOR,
    ROLE_PATIENT
} UserRole;

// 用户结构体
typedef struct User {
    int id;                 // 用户唯一ID
    char username[MAX_NAME_LEN];
    char password[65];      // SHA256 哈希值 (64字符 + '\0')
    UserRole role;
    int relatedId;          // 关联的医生ID或患者ID (0表示仅管理员)
    struct User* next;
} User;

// 全局用户链表头指针
extern User* userHead;

// 用户管理函数
void initDefaultUsers();                // 初始化默认管理员账户
int registerUser(const char* username, const char* password, UserRole role, int relatedId);
int loginUser(const char* username, const char* password, UserRole* outRole, int* outRelatedId);
User* findUserByUsername(const char* username);

// 哈希函数 (简化版 SHA256，实际可调用外部库)
void sha256(const char* str, char outputBuffer[65]);

// JSON 输出函数 (命令行模式)
void outputRegisterJson(const char* username, const char* password, int role, int relatedId);
void outputLoginJson(const char* username, const char* password);

#endif