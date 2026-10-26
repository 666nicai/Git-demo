// ============================================================
// 文件名: ShortTypes.h
// 描述  : 类名不变，给实现代码用的短类型别名
// 作者  : 魏荣赫  学号 55250124  软件一班(552401班)
// ============================================================
#ifndef SHORT_TYPES_H
#define SHORT_TYPES_H

// 题目要求类名统一带姓名缩写 WRH，所以 .h 里的类名保持 UserWRH 等不动。
// 这个头文件只给 .cpp 用：给常用类起短别名，避免实现代码满屏 WRH。
// 例如 User 就是 UserWRH，Account 就是 MicroAccountWRH，两者是同一个类型。
class BaseInfoWRH;
class UserWRH;
class MicroAccountWRH;
class QQAccountWRH;
class WeiboAccountWRH;
class WeChatAccountWRH;
class FriendInfoWRH;
class FriendBookWRH;
class GroupMemberWRH;
class GroupMemberBookWRH;
class GroupWRH;
class QQGroupWRH;
class WeChatGroupWRH;
class MyDateWRH;
class PlatformWRH;

using BaseInfo      = BaseInfoWRH;
using User          = UserWRH;
using Account       = MicroAccountWRH;
using QQAccount     = QQAccountWRH;
using WeiboAccount  = WeiboAccountWRH;
using WeChatAccount = WeChatAccountWRH;
using FriendInfo    = FriendInfoWRH;
using FriendBook    = FriendBookWRH;
using GroupMember   = GroupMemberWRH;
using MemberBook    = GroupMemberBookWRH;
using Group         = GroupWRH;
using QQGroup       = QQGroupWRH;
using WeChatGroup   = WeChatGroupWRH;
using Date          = MyDateWRH;
using Platform      = PlatformWRH;

#endif
