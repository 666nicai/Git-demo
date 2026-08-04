// ============================================================
// 文件名: user.c
// 描述: 用户管理模块实现 (注册、登录)
// ============================================================

#include "user.h"
#include <stdio.h>

#include <stdlib.h>
#include <string.h>

// 简单哈希函数 (模拟，生产环境请替换为真正的 SHA256)
static void simpleHash(const char* str, char output[65]) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    sprintf(output, "%016lx%016lx%016lx%016lx", hash, hash ^ 0xABCDEF, hash ^ 0x12345678, hash ^ 0x9ABCDEF0);
    output[64] = '\0';
}

void sha256(const char* str, char outputBuffer[65]) {
    simpleHash(str, outputBuffer);
}

User* userHead = NULL;

// 初始化默认管理员账户
void initDefaultUsers() {
    if (userHead != NULL) return;

    User* admin = (User*)malloc(sizeof(User));
    admin->id = 1;
    strcpy(admin->username, "admin");
    sha256("admin123", admin->password);
    admin->role = ROLE_ADMIN;
    admin->relatedId = 0;
    admin->next = NULL;
    userHead = admin;
}

// 查找用户
User* findUserByUsername(const char* username) {
    User* u = userHead;
    while (u) {
        if (strcmp(u->username, username) == 0)
            return u;
        u = u->next;
    }
    return NULL;
}

// 注册新用户
int registerUser(const char* username, const char* password, UserRole role, int relatedId) {
    if (findUserByUsername(username) != NULL)
        return 0;   // 用户名已存在

    if (role == ROLE_DOCTOR && findDoctorById(relatedId) == NULL)
        return -1;  // 医生ID不存在
    if (role == ROLE_PATIENT && findPatientById(relatedId) == NULL)
        return -2;  // 患者ID不存在

    User* newUser = (User*)malloc(sizeof(User));
    newUser->id = (userHead == NULL) ? 1 : userHead->id + 1;
    strcpy(newUser->username, username);
    sha256(password, newUser->password);
    newUser->role = role;
    newUser->relatedId = relatedId;
    newUser->next = userHead;
    userHead = newUser;
    return 1;   // 注册成功
}

// 登录验证
int loginUser(const char* username, const char* password, UserRole* outRole, int* outRelatedId) {
    User* u = findUserByUsername(username);
    if (u == NULL) return 0;    // 用户不存在

    char hashed[65];
    sha256(password, hashed);
    if (strcmp(u->password, hashed) != 0)
        return -1;  // 密码错误

    if (outRole) *outRole = u->role;
    if (outRelatedId) *outRelatedId = u->relatedId;
    return 1;   // 登录成功
}

// ========== JSON 输出辅助 ==========
static void printJsonString(const char* str) {
    putchar('"');
    while (*str) {
        switch (*str) {
            case '"':  printf("\\\""); break;
            case '\\': printf("\\\\"); break;
            case '\n': printf("\\n"); break;
            case '\r': printf("\\r"); break;
            case '\t': printf("\\t"); break;
            default:   putchar(*str);
        }
        str++;
    }
    putchar('"');
}

void outputRegisterJson(const char* username, const char* password, int role, int relatedId) {
    if (username == NULL || password == NULL) {
        printf("{\"success\":false,\"error\":\"参数错误\"}\n");
        return;
    }

    if (userHead == NULL) initDefaultUsers();

    int result = registerUser(username, password, (UserRole)role, relatedId);
    if (result == 1) {
        printf("{\"success\":true,\"message\":\"注册成功\"}\n");
    } else if (result == 0) {
        printf("{\"success\":false,\"error\":\"用户名已存在\"}\n");
    } else if (result == -1) {
        printf("{\"success\":false,\"error\":\"关联医生ID不存在\"}\n");
    } else if (result == -2) {
        printf("{\"success\":false,\"error\":\"关联患者ID不存在\"}\n");
    } else {
        printf("{\"success\":false,\"error\":\"注册失败\"}\n");
    }
}

void outputLoginJson(const char* username, const char* password) {
    if (username == NULL || password == NULL) {
        printf("{\"success\":false,\"error\":\"参数错误\"}\n");
        return;
    }

    if (userHead == NULL) initDefaultUsers();

    UserRole role;
    int relatedId;
    int result = loginUser(username, password, &role, &relatedId);
    if (result == 1) {
        const char* roleStr = (role == ROLE_ADMIN) ? "admin" :
                              (role == ROLE_DOCTOR) ? "doctor" : "patient";
        printf("{\"success\":true,\"role\":\"%s\",\"relatedId\":%d}\n", roleStr, relatedId);
    } else if (result == 0) {
        printf("{\"success\":false,\"error\":\"用户不存在\"}\n");
    } else {
        printf("{\"success\":false,\"error\":\"密码错误\"}\n");
    }
}