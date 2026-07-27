// ============================================================
// 文件名: user.h
// 描述: 用户管理模块 (注册、登录)
//        UserRole 定义已移至 his.h，此处不再重复定义
// ============================================================

#ifndef USER_H
#define USER_H

#include "his.h"

// 用户结构体（使用 his.h 中定义的 UserRole）
typedef struct User {
    int id;
    char username[MAX_NAME_LEN];
    char password[65];      // SHA256 哈希值 (64字符 + '\0')
    UserRole role;          // 角色类型在 his.h 中已定义
    int relatedId;
    struct User* next;
} User;

// 全局用户链表头指针
extern User* userHead;

// 用户管理函数
void initDefaultUsers();
int registerUser(const char* username, const char* password, UserRole role, int relatedId);
int loginUser(const char* username, const char* password, UserRole* outRole, int* outRelatedId);
User* findUserByUsername(const char* username);

// 哈希函数
void sha256(const char* str, char outputBuffer[65]);

// JSON 输出函数 (命令行模式)
void outputRegisterJson(const char* username, const char* password, int role, int relatedId);
void outputLoginJson(const char* username, const char* password);

#endif