// ============================================================
// 文件名: PlatformWRH.cpp
// 描述  : 平台总控实现——持久化、菜单、自动演示
// 作者  : 魏荣赫  学号 55250124  软件一班(552401班)
// ============================================================
#include "PlatformWRH.h"
#include "ShortTypes.h"

#include "GroupWRH.h"
#include "MicroAccountWRH.h"
#include "QQAccountWRH.h"
#include "QQGroupWRH.h"
#include "UserWRH.h"
#include "WeChatAccountWRH.h"
#include "WeChatGroupWRH.h"
#include "WeiboAccountWRH.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <thread>

using namespace std;

// ==================== 本文件的小工具 ====================
namespace {

void divider(const char* t) {
    cout << "\n================ " << t << " ================\n";
}

// 自动演示的节奏（毫秒）。
// 觉得整个演示太快：把 DEMO_PAUSE_MS 调大（比如 1500、2000）；
// 觉得太慢：调小（比如 700、500）。另外两个常量会跟着一起变。
const int DEMO_PAUSE_MS   = 1000;                    // 一个普通步骤
const int DEMO_SECTION_MS = DEMO_PAUSE_MS * 3 / 2;   // 两个大段之间
const int DEMO_LINE_MS    = DEMO_PAUSE_MS / 8;       // 逐行打印的列表

// 先 flush 再睡：不然 cout 还憋在缓冲区里，停完才一下全吐出来
void demoPause(int ms = DEMO_PAUSE_MS) {
    cout.flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// 去掉首尾空白（备注允许为空，也允许中间带空格）
string trim(const string& s) {
    size_t l = s.find_first_not_of(" \t\r\n");
    if (l == string::npos) return "";
    size_t r = s.find_last_not_of(" \t\r\n");
    return s.substr(l, r - l + 1);
}

// 读整数。
// 返回值：>=0 正常；-1 输入非法；-2 输入流结束(EOF)。
// 读完把本行剩余字符吃掉，后面的 askLine 才不会读到空行。
int askInt(const char* tip) {
    cout << tip << " ";
    int x;
    if (!(cin >> x)) {
        if (cin.eof()) {
            cout << "\n";
            return -2;
        }
        cin.clear();
        cin.ignore(0x7fffffff, '\n');
        return -1;
    }
    cin.ignore(0x7fffffff, '\n');
    return x;
}

// 读一整行，允许空串。备注、绑定QQ的“直接回车”都用它。
string askLine(const char* tip) {
    cout << tip << " ";
    string s;
    getline(cin, s);
    return trim(s);
}

// 读一个不含空格的短串：主号、群名等。
string askStr(const char* tip) {
    cout << tip << " ";
    string s;
    if (!(cin >> s)) return "";
    cin.ignore(0x7fffffff, '\n');
    return s;
}

string svcName(MicroType t) {
    return microName(t);
}

}  // namespace

// ==================== 构造 / 析构 / 清空 ====================

PlatformWRH::PlatformWRH(const string& file, bool autoSave)
    : file_(file), tempNo_(2000), autoSave_(autoSave) {
    if (!load()) buildDemo();   // 文件不存在或没数据 -> 用演示数据
}

PlatformWRH::~PlatformWRH() {
    if (autoSave_) save();      // 析构时写回；只读验证实例不写
    clearAll();
}

void Platform::clearAll() {
    for (size_t i = 0; i < users_.size(); ++i)
        delete users_[i];
    for (size_t i = 0; i < groups_.size(); ++i)
        delete groups_[i];
    users_.clear();
    groups_.clear();
}

// ==================== 文件持久化 ====================

// 文件格式（每行一条，空格分隔）：
//   U 主号 昵称 出生日期 申请日期 所在地
//   O 主号 服务 [微信号]
//   B 主号 绑定的QQ
//   F 主号A 主号B 服务 备注
//   G 群号 群名 服务 群主主号
//   K 群号 服务 成员主号 角色
void Platform::save() const {
    ofstream out(file_.c_str());
    if (!out) {
        cout << "[文件] 无法写入 " << file_ << "\n";
        return;
    }

    // ---- 第 1 段：用户资料、开通的服务、微信绑定 ----
    for (size_t i = 0; i < users_.size(); ++i) {
        User* u = users_[i];
        out << "U " << u->mainNo() << ' ' << u->nick() << ' '
            << u->birth().str() << ' ' << u->applyDate().str() << ' '
            << u->city() << "\n";

        const vector<Account*>& accs = u->accounts();
        for (size_t j = 0; j < accs.size(); ++j) {
            out << "O " << u->mainNo() << ' ' << svcName(accs[j]->type());
            if (accs[j]->type() == MWX) {
                WeChatAccount* w =
                    dynamic_cast<WeChatAccount*>(accs[j]);
                out << ' ' << (w ? w->wxNo() : "");
            }
            out << "\n";
        }

        WeChatAccount* w = u->wxAccount();
        if (w && w->hasBoundQQ())
            out << "B " << u->mainNo() << ' ' << w->boundQQ() << "\n";
    }

    // ---- 第 2 段：好友关系 ----
    // 每个方向各写一条。这样 A 对 B 的备注和 B 对 A 的备注都能保住，
    // 不会出现“只有主号小的一方存了备注”的丢数据问题。
    for (size_t i = 0; i < users_.size(); ++i) {
        User* u = users_[i];
        const vector<Account*>& accs = u->accounts();
        for (size_t j = 0; j < accs.size(); ++j) {
            const vector<FriendInfo>& fs = accs[j]->friends().all();
            for (size_t k = 0; k < fs.size(); ++k) {
                out << "F " << u->mainNo() << ' '
                    << fs[k].person()->mainNo() << ' '
                    << svcName(accs[j]->type()) << ' '
                    << fs[k].remark() << "\n";
            }
        }
    }

    // ---- 第 3 段：群与群成员 ----
    for (size_t i = 0; i < groups_.size(); ++i) {
        Group* g = groups_[i];
        out << "G " << g->gno() << ' ' << g->name() << ' '
            << svcName(g->svc()) << ' '
            << (g->owner() ? g->owner()->mainNo() : "0") << "\n";

        const vector<GroupMember>& ms = g->members().all();
        for (size_t j = 0; j < ms.size(); ++j) {
            if (ms[j].isOwner()) continue;   // 群主由 G 行恢复
            out << "K " << g->gno() << ' ' << svcName(g->svc()) << ' '
                << ms[j].user()->mainNo() << ' ' << (int)ms[j].role() << "\n";
        }
    }

    cout << "[文件] 已写回 " << file_ << "（用户 " << users_.size()
         << " 位，群 " << groups_.size() << " 个）\n";
}

bool Platform::load() {
    ifstream in(file_.c_str());
    if (!in) return false;

    clearAll();
    vector<string> lines;
    string line;
    while (getline(in, line)) {
        if (!line.empty()) lines.push_back(line);
    }
    in.close();

    map<string, User*> byId;

    // ---- 第 1 遍：U 行，先把所有用户建出来 ----
    // 这一步必须先做：F 行里会引用“对方”，如果边读边建，
    // 对方可能还没出现，好友关系就会被丢掉（旧版的 bug 就在这里）。
    for (size_t i = 0; i < lines.size(); ++i) {
        istringstream iss(lines[i]);
        string cmd;
        iss >> cmd;
        if (cmd != "U") continue;

        string id, nick, birth, apply, city;
        if (!(iss >> id >> nick >> birth >> apply >> city)) continue;

        Date b, a;
        if (!b.parse(birth) || !a.parse(apply)) continue;
        User* u = new User(id, nick, b, a, city);
        users_.push_back(u);
        byId[id] = u;
    }

    // ---- 第 2 遍：O 行，开通服务 ----
    for (size_t i = 0; i < lines.size(); ++i) {
        istringstream iss(lines[i]);
        string cmd;
        iss >> cmd;
        if (cmd != "O") continue;

        string id, s, wx;
        MicroType t;
        iss >> id >> s;
        if (!parseSvc(s, t)) continue;

        User* u = byId[id];
        if (!u) continue;
        if (t == MWX && (iss >> wx)) u->setWxNo(wx);
        u->openService(t, true);   // 读文件时安静恢复，不打印“开通成功”
    }

    // ---- 第 3 遍：B 行，微信绑定QQ ----
    // 放在 O 行之后，保证微信账号已经建好。
    for (size_t i = 0; i < lines.size(); ++i) {
        istringstream iss(lines[i]);
        string cmd;
        iss >> cmd;
        if (cmd != "B") continue;

        string id, qq;
        iss >> id >> qq;
        User* u = byId[id];
        if (u) u->bindWxToQQ(qq, true);
    }

    // ---- 第 4 遍：F 行，好友 ----
    // 文件里 A->B、B->A 各一条：
    //   第一条把双向关系建起来，并写入 A 的备注；
    //   第二条发现已经是好友，只更新 B 的备注。
    for (size_t i = 0; i < lines.size(); ++i) {
        istringstream iss(lines[i]);
        string cmd;
        iss >> cmd;
        if (cmd != "F") continue;

        string a, b, s, remark;
        MicroType t;
        if (!(iss >> a >> b >> s)) continue;
        getline(iss, remark);   // 备注可以带空格，读到行尾
        remark = trim(remark);
        if (!parseSvc(s, t)) continue;

        User* ua = byId[a];
        User* ub = byId[b];
        if (!ua || !ub) continue;

        Account* aa = ua->account(t);
        Account* ab = ub->account(t);
        if (!aa || !ab) continue;

        if (!aa->friends().has(ub)) {
            aa->friends().add(ub, remark);   // 直接操作容器，避免读文件时打印
            ab->friends().add(ua, "");
        }
        aa->friends().setRemark(ub, remark);
    }

    // ---- 第 5 遍：G 行，建群 ----
    // 群主必须已经存在，而且开通了对应服务。
    for (size_t i = 0; i < lines.size(); ++i) {
        istringstream iss(lines[i]);
        string cmd;
        iss >> cmd;
        if (cmd != "G") continue;

        int no;
        string name, s, ownerId;
        MicroType t;
        if (!(iss >> no >> name >> s >> ownerId)) continue;
        if (!parseSvc(s, t)) continue;
        if (t != MQQ && t != MWX) continue;   // 只支持QQ群、微信群

        // 这里只要求群主用户存在。旧文件里可能有“群主已注销该服务”的群，
        // 读文件时宁可保留群历史，也不要静默把整个群丢掉。
        User* ow = byId[ownerId];
        if (!ow) continue;

        Group* g = nullptr;
        if (t == MQQ) g = new QQGroup(no, name, ow);
        else          g = new WeChatGroup(no, name, ow);
        groups_.push_back(g);
    }

    // ---- 第 6 遍：K 行，群成员与角色 ----
    for (size_t i = 0; i < lines.size(); ++i) {
        istringstream iss(lines[i]);
        string cmd;
        iss >> cmd;
        if (cmd != "K") continue;

        int no, role;
        string s, uid;
        MicroType t;
        if (!(iss >> no >> s >> uid >> role)) continue;
        if (!parseSvc(s, t)) continue;

        // 同上：成员用户存在就恢复，尽量不因为服务注销而丢成员记录
        User* u = byId[uid];
        if (!u) continue;

        Group* g = nullptr;
        for (size_t j = 0; j < groups_.size(); ++j) {
            if (groups_[j]->gno() == no && groups_[j]->svc() == t) {
                g = groups_[j];
                break;
            }
        }
        if (!g) continue;

        if (role == ROLE_ADMIN)       g->members().add(u, ROLE_ADMIN);
        else if (role == ROLE_MEMBER) g->members().add(u, ROLE_MEMBER);
    }

    // 空文件（比如写到一半中断）当作没数据，让构造函数去建演示数据
    if (users_.empty()) {
        cout << "[文件] " << file_ << " 中没有用户数据，改用演示数据。\n";
        clearAll();
        return false;
    }

    // 临时讨论组号接着已有最大群号往后排，避免重启后从 2000 重新撞号
    tempNo_ = 2000;
    for (size_t i = 0; i < groups_.size(); ++i)
        if (groups_[i]->gno() > tempNo_) tempNo_ = groups_[i]->gno();

    cout << "[文件] 已从 " << file_ << " 读入 " << users_.size()
         << " 位用户，" << groups_.size() << " 个群。\n";
    return true;
}

// ==================== 查找与选择 ====================

User* Platform::findUser(const string& id) const {
    for (size_t i = 0; i < users_.size(); ++i)
        if (users_[i]->mainNo() == id) return users_[i];
    return nullptr;
}

void Platform::listAllUsersShort() const {
    for (size_t i = 0; i < users_.size(); ++i) {
        const User* u = users_[i];
        cout << "   " << (i + 1) << ". 主号 " << u->mainNo()
             << "  昵称 " << u->nick() << "  [" << u->city()
             << "]  已开通 " << openedToStr(u->openedSet()) << "\n";
    }
}

User* Platform::pickUser(const char* tip) const {
    cout << tip << "\n";
    listAllUsersShort();
    int k = askInt("请输入用户序号(0=取消)：") - 1;
    if (k < 0 || k >= (int)users_.size()) return nullptr;
    return users_[k];
}

Group* Platform::pickGroup(const char* tip) const {
    cout << tip << "\n";
    if (groups_.empty()) {
        cout << "   （当前没有群）\n";
        return nullptr;
    }
    for (size_t i = 0; i < groups_.size(); ++i) {
        const Group* g = groups_[i];
        cout << "   " << (i + 1) << ". [" << g->kindName() << "] 群号 "
             << g->gno() << " 群名 " << g->name()
             << "  群主 " << (g->owner() ? g->owner()->nick() : "(无)")
             << "  成员 " << g->memberCount() << " 人\n";
    }
    int k = askInt("请输入群序号(0=取消)：") - 1;
    if (k < 0 || k >= (int)groups_.size()) return nullptr;
    return groups_[k];
}

MicroType Platform::pickService() const {
    int k = askInt("请选择服务 [1=QQ 2=微博 3=微信] (0=取消)：");
    if (k == 1) return MQQ;
    if (k == 2) return MWB;
    if (k == 3) return MWX;
    return MCOUNT;
}

MicroType Platform::pickServiceFromUser(const User* u) const {
    if (!u) return MCOUNT;
    const vector<Account*>& as = u->accounts();
    if (as.empty()) {
        cout << "    [提示] 该用户尚未开通任何服务。\n";
        return MCOUNT;
    }

    cout << "    该用户已开通：";
    for (size_t i = 0; i < as.size(); ++i)
        cout << (i ? "、" : "") << (i + 1) << "=" << microName(as[i]->type());
    cout << "\n";

    int k = askInt("请选择序号(0=取消)：");
    if (k < 1 || k > (int)as.size()) return MCOUNT;
    return as[k - 1]->type();
}

string Platform::toSvcName(MicroType t) {
    return microName(t);
}

bool Platform::parseSvc(const string& s, MicroType& t) {
    if (s == "QQ")   { t = MQQ; return true; }
    if (s == "微博") { t = MWB; return true; }
    if (s == "微信") { t = MWX; return true; }
    return false;
}

// 跨服务推荐：from 好友里，有 to 服务账号、但还没加为 to 好友的人
vector<User*> Platform::recommendFriends(const User* op,
                                               MicroType from,
                                               MicroType to) const {
    vector<User*> rec;
    if (!op || from == to) return rec;

    const Account* src = op->account(from);
    const Account* dst = op->account(to);
    if (!src || !dst) return rec;

    const vector<FriendInfo>& fs = src->friends().all();
    for (size_t i = 0; i < fs.size(); ++i) {
        User* p = fs[i].person();
        if (!p || p == op) continue;
        if (p->hasService(to) && !dst->isFriend(p))
            rec.push_back(p);
    }
    return rec;
}

// 注销服务后的群清理：
//   · 普通成员：直接从该服务的群里移出
//   · 群主：先把群主转给管理员（没有管理员就转给第一个成员），再移出自己
//   · 群里只剩他一个人：群没有意义了，删除
void Platform::cleanGroupsAfterClose(User* u, MicroType t) {
    for (size_t i = 0; i < groups_.size(); ) {
        Group* g = groups_[i];
        if (g->svc() != t || !g->isMember(u)) {
            ++i;
            continue;
        }

        if (g->owner() == u) {
            User* next = nullptr;
            const vector<GroupMember>& ms = g->members().all();

            // 优先找管理员接任
            for (size_t j = 0; j < ms.size(); ++j) {
                if (ms[j].user() != u && ms[j].isAdmin()) {
                    next = ms[j].user();
                    break;
                }
            }
            // 没有管理员，就找第一个普通成员
            if (!next) {
                for (size_t j = 0; j < ms.size(); ++j) {
                    if (ms[j].user() != u) {
                        next = ms[j].user();
                        break;
                    }
                }
            }

            if (!next) {
                cout << "    [群清理] 群[" << g->name()
                     << "]没有其他成员，已删除。\n";
                delete g;
                groups_.erase(groups_.begin() + i);
                continue;   // 删除后当前位置就是下一个元素，不要再 ++i
            }

            g->members().setRole(next, ROLE_OWNER);
            cout << "    [群主转让] 群[" << g->name() << "] 群主转给 "
                 << next->nick() << "。\n";
        }

        g->members().remove(u);
        ++i;
    }
}

// ==================== 演示数据 ====================

void Platform::buildDemo() {
    clearAll();          // 保证可以重复调用，不会叠数据
    tempNo_ = 2000;

    // ---- 6 位基础用户 ----
    struct Seed {
        const char* id;
        const char* nick;
        const char* birth;
        const char* apply;
        const char* city;
    };
    Seed seeds[] = {
        {"10001", "小腾", "1998-05-20", "2014-06-01", "深圳"},
        {"10002", "阿讯", "1999-03-15", "2014-08-12", "北京"},
        {"10003", "娜娜", "2000-11-02", "2015-02-20", "成都"},
        {"10004", "阿豪", "1997-09-30", "2013-12-01", "广州"},
        {"10005", "小雨", "2001-01-08", "2015-09-09", "上海"},
        {"10006", "老马", "1996-07-19", "2012-10-01", "杭州"},
    };
    for (size_t i = 0; i < sizeof(seeds) / sizeof(seeds[0]); ++i) {
        Date b, a;
        b.parse(seeds[i].birth);
        a.parse(seeds[i].apply);
        users_.push_back(new User(seeds[i].id, seeds[i].nick,
                                     b, a, seeds[i].city));
    }

    User* u1 = findUser("10001");
    User* u2 = findUser("10002");
    User* u3 = findUser("10003");
    User* u4 = findUser("10004");
    User* u5 = findUser("10005");
    User* u6 = findUser("10006");
    if (!u1 || !u2 || !u3 || !u4 || !u5 || !u6) return;

    // ---- 开通服务（安静开通，具体演示放在 demoOpenServices）----
    u1->openService(MQQ, true);
    u1->openService(MWB, true);
    u1->openService(MWX, true);
    u2->openService(MQQ, true);
    u2->openService(MWB, true);
    u2->openService(MWX, true);
    u3->openService(MQQ, true);
    u4->openService(MQQ, true);
    u4->openService(MWB, true);
    u4->openService(MWX, true);
    u5->openService(MQQ, true);
    u5->openService(MWX, true);
    u6->openService(MWX, true);
    u1->bindWxToQQ("10001", true);   // 微信绑定QQ示例

    // ---- 好友（按服务分开；addFriend 会自动加双向）----
    u1->account(MQQ)->addFriend(u2, "阿讯(助教)");
    u1->account(MQQ)->addFriend(u3, "");
    u1->account(MQQ)->addFriend(u4, "广州大佬");
    u2->account(MQQ)->addFriend(u3, "");
    u3->account(MQQ)->addFriend(u4, "");
    u4->account(MQQ)->addFriend(u5, "");

    u1->account(MWB)->addFriend(u2, "");
    u1->account(MWB)->addFriend(u4, "");

    u1->account(MWX)->addFriend(u2, "铁哥们");
    u1->account(MWX)->addFriend(u5, "");

    // ---- 预置群：每个服务都有 1001~1006 ----
    // QQ群 1001~1006
    QQGroup* q1 = new QQGroup(1001, "C++程序设计学习群", u1);
    q1->members().add(u2, ROLE_ADMIN);
    q1->members().add(u3, ROLE_MEMBER);
    q1->members().add(u4, ROLE_MEMBER);
    q1->members().add(u5, ROLE_MEMBER);
    groups_.push_back(q1);

    QQGroup* q2 = new QQGroup(1002, "班级事务通知群", u2);
    q2->members().add(u1, ROLE_ADMIN);
    q2->members().add(u4, ROLE_MEMBER);
    groups_.push_back(q2);

    QQGroup* q3 = new QQGroup(1003, "考研自习室", u3);
    q3->members().add(u1, ROLE_MEMBER);
    q3->members().add(u4, ROLE_ADMIN);
    groups_.push_back(q3);

    QQGroup* q4 = new QQGroup(1004, "前端技术交流群", u4);
    q4->members().add(u2, ROLE_ADMIN);
    q4->members().add(u5, ROLE_MEMBER);
    groups_.push_back(q4);

    QQGroup* q5 = new QQGroup(1005, "算法打卡群", u5);
    q5->members().add(u1, ROLE_MEMBER);
    q5->members().add(u3, ROLE_ADMIN);
    groups_.push_back(q5);

    QQGroup* q6 = new QQGroup(1006, "校园二手交易", u2);
    q6->members().add(u3, ROLE_MEMBER);
    q6->members().add(u4, ROLE_MEMBER);
    groups_.push_back(q6);

    // 微信群 1001~1006（微信群没有管理员，全部是普通成员）
    WeChatGroup* w1 = new WeChatGroup(1001, "周末爬山俱乐部", u5);
    w1->members().add(u6, ROLE_MEMBER);
    w1->members().add(u2, ROLE_MEMBER);
    w1->members().add(u1, ROLE_MEMBER);
    w1->members().add(u4, ROLE_MEMBER);
    groups_.push_back(w1);

    WeChatGroup* w2 = new WeChatGroup(1002, "产品内测体验群", u1);
    w2->members().add(u4, ROLE_MEMBER);
    w2->members().add(u2, ROLE_MEMBER);
    groups_.push_back(w2);

    WeChatGroup* w3 = new WeChatGroup(1003, "长春吃喝玩乐", u2);
    w3->members().add(u1, ROLE_MEMBER);
    w3->members().add(u4, ROLE_MEMBER);
    w3->members().add(u6, ROLE_MEMBER);
    groups_.push_back(w3);

    WeChatGroup* w4 = new WeChatGroup(1004, "家人群", u6);
    w4->members().add(u1, ROLE_MEMBER);
    w4->members().add(u2, ROLE_MEMBER);
    groups_.push_back(w4);

    WeChatGroup* w5 = new WeChatGroup(1005, "项目讨论组", u4);
    w5->members().add(u2, ROLE_MEMBER);
    w5->members().add(u5, ROLE_MEMBER);
    groups_.push_back(w5);

    WeChatGroup* w6 = new WeChatGroup(1006, "读书分享会", u5);
    w6->members().add(u1, ROLE_MEMBER);
    w6->members().add(u6, ROLE_MEMBER);
    groups_.push_back(w6);
}

// ==================== 自动演示 ====================

void Platform::demoOpenServices() {
    divider("开通管理演示");
    demoPause();
    cout << "已有演示用户都开通了若干服务，下面用一个新用户演示开通/注销：\n";

    User* nu = findUser("10007");
    if (!nu) {
        Date b, a;
        b.parse("2002-04-18");
        a.parse("2016-01-01");
        nu = new User("10007", "新手上路", b, a, "长春");
        users_.push_back(nu);
    }
    nu->showProfile();
    demoPause(DEMO_PAUSE_MS / 2);

    nu->openService(MQQ);   // QQ：共享主号
    nu->openService(MWB);   // 微博：与QQ同一个号
    nu->openService(MWX);   // 微信：独立微信号

    cout << "\n[共享ID验证] QQ号码 = " << nu->account(MQQ)->svcNo()
         << "，微博号码 = " << nu->account(MWB)->svcNo() << "（相同）\n";
    WeChatAccount* w = nu->wxAccount();
    if (w)
        cout << "[独立ID验证] 微信号 = " << w->wxNo()
             << "（与主号不同，可绑定QQ）\n";
    demoPause();

    cout << "\n绑定演示：把新用户的微信绑定到QQ：\n";
    nu->bindWxToQQ(nu->mainNo());
    demoPause();

    cout << "\n注销微博演示：\n";
    nu->closeService(MWB);
    cout << "注销后已开通服务数 = " << nu->openedCount() << "\n";
    demoPause(DEMO_PAUSE_MS / 2);
    nu->closeService(MWB);   // 重复注销：应给出提示
    demoPause(DEMO_PAUSE_MS / 2);
}

void Platform::demoLogin() {
    divider("登录管理演示（一个服务登录 → 其它自动登录）");
    demoPause();
    User* u1 = findUser("10001");
    if (!u1) return;

    u1->logoutAll();
    u1->showProfile();
    demoPause(DEMO_PAUSE_MS / 2);
    u1->login(MQQ);   // 只在QQ登录，微博/微信会自动登录
    demoPause();
    u1->login(MWB);   // 已经全登录，再登录微博只是重复打印
    demoPause(DEMO_PAUSE_MS / 2);

    cout << "\n全部登出后，改在微信登录：\n";
    u1->logoutAll();
    u1->login(MWX);
    demoPause();
}

void Platform::demoFriendCRUD() {
    divider("好友管理演示（增 / 删 / 改 / 查）");
    demoPause();
    User* u1 = findUser("10001");
    User* u2 = findUser("10002");
    User* u3 = findUser("10003");
    User* u5 = findUser("10005");
    User* u6 = findUser("10006");
    if (!u1 || !u2 || !u3 || !u5 || !u6) return;

    Account* qq = u1->account(MQQ);
    if (!qq) return;

    cout << "小腾 的 QQ 好友（添加前）：\n";
    qq->friends().list();
    demoPause();

    cout << "\n[增] 添加 QQ 好友 小雨(10005)，备注“算法队友”：\n";
    qq->addFriend(u5, "算法队友");
    demoPause(DEMO_PAUSE_MS / 2);

    cout << "\n[增][失败演示] 老马(10006)未开通QQ，不能成为QQ好友：\n";
    qq->addFriend(u6, "");
    demoPause(DEMO_PAUSE_MS / 2);

    cout << "\n[删] 删除 QQ 好友 娜娜(10003)：\n";
    qq->delFriend(u3);
    cout << "删除后好友数 = " << qq->friends().count() << "\n";
    demoPause(DEMO_PAUSE_MS / 2);

    cout << "\n[改] 把对 阿讯(10002) 的备注改为“学委阿讯”：\n";
    if (qq->friends().find(u2))
        qq->friends().setRemark(u2, "学委阿讯");
    demoPause(DEMO_PAUSE_MS / 2);

    cout << "\n[查] 好友查询结果：\n";
    qq->friends().list();
    demoPause();
}

void Platform::demoCommonFriend() {
    divider("微X间共同好友演示（QQ 与 微博）");
    demoPause();
    User* u1 = findUser("10001");
    if (!u1) return;

    Account* qq = u1->account(MQQ);
    Account* wb = u1->account(MWB);
    if (!qq || !wb) {
        cout << "（需要同时开通QQ与微博）\n";
        return;
    }

    cout << "小腾：QQ 好友为 ";
    qq->friends().listSimple();
    cout << "      微博好友为 ";
    wb->friends().listSimple();
    demoPause();

    vector<User*> common = qq->friends().commonWith(wb->friends());
    cout << "QQ 与 微博 的共同好友(" << common.size() << "人)：";
    for (size_t i = 0; i < common.size(); ++i)
        cout << (i ? "、" : "") << common[i]->nick();
    cout << "\n";
    demoPause();
}

void Platform::demoWeChatRecFriend() {
    divider("微信 添加“QQ推荐好友”");
    demoPause();
    User* u1 = findUser("10001");
    if (!u1) return;

    Account* wx = u1->account(MWX);
    if (!wx) {
        cout << "（需要同时开通QQ与微信）\n";
        return;
    }

    vector<User*> rec = recommendFriends(u1, MQQ, MWX);
    cout << "小腾的 QQ 好友中，已开通微信且还不是微信好友的人("
         << rec.size() << "人)：";
    for (size_t i = 0; i < rec.size(); ++i)
        cout << (i ? "、" : "") << rec[i]->nick()
             << "(" << rec[i]->wxNo() << ")";
    cout << "\n将这些人逐个添加为微信好友：\n";
    demoPause();

    for (size_t i = 0; i < rec.size(); ++i) {
        if (wx->addFriend(rec[i], "来自QQ推荐"))
            cout << "    [推荐] 已把 " << rec[i]->nick()
                 << "(" << rec[i]->wxNo() << ") 添加为微信好友\n";
    }
    demoPause();

    cout << "\n添加后 小腾 的微信好友：";
    wx->friends().listSimple();
    demoPause();
}

void Platform::demoGroups() {
    divider("群管理多态演示（QQ群 vs 微信群）");
    demoPause();
    User* u1 = findUser("10001");
    User* u2 = findUser("10002");
    User* u3 = findUser("10003");
    User* u4 = findUser("10004");
    User* u5 = findUser("10005");
    if (!u1 || !u2 || !u3 || !u4 || !u5) return;

    // 预置群有 12 个，先列清单，避免把整个屏幕刷满
    cout << "\n—— 预置群列表（每个服务 1001~1006）——\n";
    for (size_t i = 0; i < groups_.size(); ++i) {
        Group* g = groups_[i];
        User* ow = g->owner();
        cout << "  [" << g->kindName() << "] " << g->gno() << " "
             << g->name() << "  群主：" << (ow ? ow->nick() : string("(无)"))
             << "  成员：" << g->memberCount() << " 人\n";
        demoPause(DEMO_LINE_MS);
    }

    Group* q1001 = nullptr;   // QQ群 1001
    Group* w1001 = nullptr;   // 微信群 1001
    for (size_t i = 0; i < groups_.size(); ++i) {
        if (groups_[i]->gno() == 1001 && groups_[i]->svc() == MQQ)
            q1001 = groups_[i];
        if (groups_[i]->gno() == 1001 && groups_[i]->svc() == MWX)
            w1001 = groups_[i];
    }

    cout << "\n—— 两个代表性群的详情 ——\n";
    if (q1001) q1001->show();
    if (w1001) w1001->show();
    demoPause();

    // 准备演示用户：
    //   nu  (10007)：demoOpenServices 已建，开通了 QQ + 微信
    //   u8  (10008)：新建，只开通微信，用来演示“微信群只能被邀请”
    User* nu = findUser("10007");
    if (!nu) {
        Date b, a;
        b.parse("2002-04-18");
        a.parse("2016-01-01");
        nu = new User("10007", "新手上路", b, a, "长春");
        users_.push_back(nu);
        nu->openService(MQQ, true);
    }
    User* u8 = findUser("10008");
    if (!u8) {
        Date b, a;
        b.parse("2003-02-11");
        a.parse("2017-05-20");
        u8 = new User("10008", "被邀新人", b, a, "大连");
        users_.push_back(u8);
        u8->openService(MWX, true);
    }
    demoPause(DEMO_PAUSE_MS / 2);

    // 1) 申请加入：QQ群可以，微信群不行
    cout << "\n[申请加入] QQ群可申请；微信群只能被邀请/推荐：\n";
    if (q1001 && nu) q1001->requestJoin(nu);
    if (w1001 && u8) w1001->requestJoin(u8);
    demoPause();

    // 2) 邀请加入：成员邀请，两种群都允许
    cout << "\n[邀请加入] 微信群群主邀请 被邀新人：\n";
    if (w1001 && u5 && u8) w1001->inviteJoin(u5, u8);
    demoPause();

    // 3) 设管理员：QQ群可以，微信群没有管理员
    cout << "\n[设管理员] QQ群群主可设；微信群无管理员：\n";
    if (q1001 && u1 && nu) q1001->setAdmin(u1, nu);
    if (w1001 && u5 && u8) w1001->setAdmin(u5, u8);
    demoPause();

    // 4) 踢人：QQ群管理员可踢普通成员；微信群只有群主能踢
    cout << "\n[踢人] QQ群管理员踢普通成员；微信群成员踢人被拒、群主可踢：\n";
    if (q1001 && u2 && u4) q1001->kick(u2, u4);
    if (w1001 && u1 && u8) w1001->kick(u1, u8);
    if (w1001 && u5 && u8) w1001->kick(u5, u8);
    demoPause();

    // 5) 临时讨论组：QQ群可以，微信群不可以
    cout << "\n[临时讨论组] QQ群可建子群；微信群不可：\n";
    if (q1001) {
        vector<User*> picks;
        picks.push_back(u2);
        picks.push_back(u3);
        Group* sub = q1001->createSubGroup(tempNo_ + 1,
                                              "临时代码评审会", picks);
        if (sub) {
            ++tempNo_;
            groups_.push_back(sub);
            cout << "  已建子群：\n";
            sub->show();
        }
    }
    if (w1001) {
        Group* s2 = w1001->createSubGroup(tempNo_ + 1, "临时会",
                                             vector<User*>());
        if (s2) {
            ++tempNo_;
            groups_.push_back(s2);
        }
    }
    demoPause();

    // 6) 查询某人所在群
    cout << "\n[查询] 查看 小腾(10001) 加入的所有群：\n";
    for (size_t i = 0; i < groups_.size(); ++i) {
        if (!groups_[i]->isMember(u1)) continue;
        cout << "    [" << groups_[i]->kindName() << "] 群号 "
             << groups_[i]->gno() << " 群名 " << groups_[i]->name() << "\n";
    }
    demoPause();
}

void Platform::demoModeSwitch() {
    divider("群管理模式动态切换（成员数据不受伤）");
    demoPause();
    Group* src = nullptr;
    for (size_t i = 0; i < groups_.size(); ++i) {
        if (groups_[i]->gno() == 1001 && groups_[i]->svc() == MQQ) {
            src = groups_[i];
            break;
        }
    }
    if (!src) return;

    int before = src->memberCount();
    cout << "切换前 [QQ群1001] 成员数 = " << before << "\n";
    src->show();
    demoPause();

    cout << "\n→ 调用 modeTwin() 生成“同样成员、微信群规则”的镜像群：\n";
    Group* twin = src->modeTwin();
    if (!twin) return;
    demoPause(DEMO_PAUSE_MS / 2);

    cout << "（镜像群成员数 = " << twin->memberCount()
         << "，与源群 " << before << " 一致 → 数据不受伤）\n";
    twin->show();
    demoPause();
    delete twin;
}

// ==================== 数据总览 ====================

void Platform::menuOverview() {
    divider("数据总览");
    cout << "平台共 " << users_.size() << " 位用户，"
         << groups_.size() << " 个群\n";

    for (size_t i = 0; i < users_.size(); ++i) {
        users_[i]->showProfile();
        users_[i]->showOpened();
        cout << "\n";
    }

    cout << "群列表：\n";
    for (size_t i = 0; i < groups_.size(); ++i) {
        groups_[i]->show();
        cout << "\n";
    }
}

// 精简总览：每行一个用户/一个群，文件读回验证时用
void Platform::showBrief() const {
    cout << "  用户（" << users_.size() << " 位）：\n";
    for (size_t i = 0; i < users_.size(); ++i) {
        const User* u = users_[i];
        cout << "    " << u->mainNo() << " " << u->nick()
             << "  开通：" << openedToStr(u->openedSet());

        const vector<Account*>& accs = u->accounts();
        for (size_t j = 0; j < accs.size(); ++j)
            cout << "  " << microName(accs[j]->type())
                 << "好友" << accs[j]->friends().count();
        cout << "\n";
    }

    cout << "  群（" << groups_.size() << " 个）：\n";
    for (size_t i = 0; i < groups_.size(); ++i) {
        const Group* g = groups_[i];
        User* ow = g->owner();
        cout << "    [" << g->kindName() << "] " << g->gno() << " "
             << g->name() << "  群主：" << (ow ? ow->nick() : string("(无)"))
             << "  成员：" << g->memberCount() << " 人\n";
    }
}

// ==================== 用户菜单 ====================

void Platform::menuUser() {
    while (true) {
        cout << "\n----------- 用户管理 ----------\n";
        cout << " 1. 查看全部用户\n"
                " 2. 查看某用户详情\n"
                " 3. 开通微X服务\n"
                " 4. 注销服务\n"
                " 5. 登录（自动登录全部）\n"
                " 6. 全部登出\n"
                " 7. 微信绑定QQ\n"
                " 0. 返回上级\n";

        int k = askInt("请选择：");
        if (k == 0 || k == -2) return;

        if (k == 1) {
            listAllUsersShort();
            continue;
        }

        if (k == 2) {
            User* u = pickUser("选择用户：");
            if (!u) continue;
            u->showProfile();
            u->showOpened();
        } else if (k == 3) {
            User* u = pickUser("选择要开通服务的用户：");
            if (!u) continue;
            MicroType t = pickService();
            if (t != MCOUNT) u->openService(t);
        } else if (k == 4) {
            User* u = pickUser("选择要注销服务的用户：");
            if (!u) continue;
            MicroType t = pickServiceFromUser(u);
            if (t != MCOUNT) {
                bool had = u->hasService(t);
                u->closeService(t);
                if (had) cleanGroupsAfterClose(u, t);   // 群里的记录也要一起清
            }
        } else if (k == 5) {
            User* u = pickUser("选择登录的用户：");
            if (!u) continue;
            MicroType t = pickServiceFromUser(u);
            if (t != MCOUNT) u->login(t);
        } else if (k == 6) {
            User* u = pickUser("选择登出的用户：");
            if (!u) continue;
            u->logoutAll();
            cout << "    已全部登出。\n";
        } else if (k == 7) {
            User* u = pickUser("选择要绑定微信的用户：");
            if (!u) continue;
            string qq = askLine("输入要绑定的QQ主号（直接回车=绑定自己的主号）：");
            if (qq.empty() || qq == "0") qq = u->mainNo();
            u->bindWxToQQ(qq);
        } else {
            cout << "    没有这个选项，请重新输入。\n";
        }
    }
}

// ==================== 好友菜单 ====================

void Platform::menuFriend() {
    User* op = pickUser("好友管理：请选择操作者");
    if (!op) return;

    while (true) {
        cout << "\n----------- 好友管理（操作者：" << op->nick()
             << "，主号 " << op->mainNo() << "） ----------\n";
        cout << " 1. 查看某服务好友列表\n"
                " 2. 添加好友\n"
                " 3. 删除好友\n"
                " 4. 修改备注\n"
                " 5. 查询某好友\n"
                " 6. 求两个服务的共同好友\n"
                " 7. 从其它服务推荐好友\n"
                " 0. 返回上级\n";

        int k = askInt("请选择：");
        if (k == 0 || k == -2) return;

        if (k == 1) {
            MicroType t = pickServiceFromUser(op);
            Account* ac = op->account(t);
            if (ac) ac->friends().list();
        } else if (k == 2) {
            MicroType t = pickServiceFromUser(op);
            Account* ac = op->account(t);
            if (!ac) continue;

            string id = askStr("输入对方主号：");
            User* who = findUser(id);
            if (!who) {
                cout << "    没有该用户。\n";
                continue;
            }
            string rem = askLine("输入备注（可为空，直接回车跳过）：");
            ac->addFriend(who, rem);
        } else if (k == 3) {
            MicroType t = pickServiceFromUser(op);
            Account* ac = op->account(t);
            if (!ac) continue;

            string id = askStr("输入要删除的好友主号：");
            User* who = findUser(id);
            if (who && ac->friends().has(who)) ac->delFriend(who);
            else cout << "    不是该服务好友，或用户不存在。\n";
        } else if (k == 4) {
            MicroType t = pickServiceFromUser(op);
            Account* ac = op->account(t);
            if (!ac) continue;

            string id = askStr("输入要改备注的好友主号：");
            User* who = findUser(id);
            string rem = askLine("输入新备注（可为空）：");
            if (who && ac->friends().find(who))
                ac->friends().setRemark(who, rem);
            else
                cout << "    不是好友，或用户不存在。\n";
        } else if (k == 5) {
            MicroType t = pickServiceFromUser(op);
            Account* ac = op->account(t);
            if (!ac) continue;

            string id = askStr("输入要查询的好友主号：");
            User* who = findUser(id);
            FriendInfo* fi = who ? ac->friends().find(who) : nullptr;
            if (fi)
                cout << "    该好友：" << fi->person()->nick()
                     << " 主号 " << fi->person()->mainNo()
                     << " 备注：" << fi->remark() << "\n";
            else
                cout << "    不是好友。\n";
        } else if (k == 6) {
            cout << "    先选第一个服务，再选第二个服务：\n";
            MicroType a = pickServiceFromUser(op);
            if (a == MCOUNT) continue;
            MicroType b = pickServiceFromUser(op);
            if (b == MCOUNT) continue;
            if (a == b) {
                cout << "    需要选两个不同的服务。\n";
                continue;
            }

            Account* aa = op->account(a);
            Account* bb = op->account(b);
            cout << "    " << microName(a) << "好友：";
            aa->friends().listSimple();
            cout << "    " << microName(b) << "好友：";
            bb->friends().listSimple();

            vector<User*> cm = aa->friends().commonWith(bb->friends());
            cout << "    共同好友(" << cm.size() << ")：";
            for (size_t i = 0; i < cm.size(); ++i)
                cout << (i ? "、" : "") << cm[i]->nick();
            cout << "\n";
        } else if (k == 7) {
            cout << "    先选推荐来源，再选要添加到的服务：\n";
            MicroType from = pickServiceFromUser(op);
            if (from == MCOUNT) continue;
            MicroType to = pickServiceFromUser(op);
            if (to == MCOUNT) continue;
            if (to == from) {
                cout << "    两个服务不能相同。\n";
                continue;
            }

            vector<User*> rec = recommendFriends(op, from, to);
            if (rec.empty()) {
                cout << "    没有可推荐的好友。\n";
                continue;
            }

            cout << "    可从 " << microName(from) << " 好友中推荐 "
                 << rec.size() << " 人到 " << microName(to) << "：\n";
            for (size_t i = 0; i < rec.size(); ++i)
                cout << "      " << (i + 1) << ". " << rec[i]->nick()
                     << "（主号 " << rec[i]->mainNo() << "）\n";

            int j = askInt("选择要添加的序号(0=全不加)：") - 1;
            if (j >= 0 && j < (int)rec.size()) {
                string remark = string("来自") + microName(from) + "推荐";
                op->account(to)->addFriend(rec[j], remark);
            }
        } else {
            cout << "    没有这个选项，请重新输入。\n";
        }
    }
}

// ==================== 群菜单 ====================

void Platform::menuGroup() {
    while (true) {
        cout << "\n----------- 群管理 ----------\n";
        cout << " 1. 查看全部群\n"
                " 2. 查看某群成员\n"
                " 3. 申请加入\n"
                " 4. 邀请/推荐加入\n"
                " 5. 踢人\n"
                " 6. 设/撤管理员\n"
                " 7. 退群\n"
                " 8. 查询某人所在群\n"
                " 9. 临时讨论组（QQ群演示）\n"
                "10. 管理模式动态切换演示\n"
                " 0. 返回上级\n";

        int k = askInt("请选择：");
        if (k == 0 || k == -2) return;

        if (k == 1) {
            for (size_t i = 0; i < groups_.size(); ++i)
                groups_[i]->show();
        } else if (k == 2) {
            Group* g = pickGroup("选择群：");
            if (g) g->show();
        } else if (k == 3) {
            Group* g = pickGroup("选择要加入的群：");
            if (!g) continue;
            string id = askStr("输入申请者主号：");
            User* who = findUser(id);
            if (who) g->requestJoin(who);
            else cout << "    无该用户。\n";
        } else if (k == 4) {
            Group* g = pickGroup("选择群：");
            if (!g) continue;
            string a = askStr("输入邀请者主号（须群内成员）：");
            string b = askStr("输入被邀请者主号：");
            User* actor = findUser(a);
            User* who = findUser(b);
            if (!actor || !who) {
                cout << "    主号不存在。\n";
                continue;
            }
            g->inviteJoin(actor, who);
        } else if (k == 5) {
            Group* g = pickGroup("选择群：");
            if (!g) continue;
            string a = askStr("输入操作者主号：");
            string b = askStr("输入被踢者主号：");
            User* actor = findUser(a);
            User* who = findUser(b);
            if (!actor || !who) {
                cout << "    主号不存在。\n";
                continue;
            }
            g->kick(actor, who);
        } else if (k == 6) {
            Group* g = pickGroup("选择群：");
            if (!g) continue;
            string a = askStr("输入操作者主号（仅群主可设）：");
            string b = askStr("输入目标成员主号：");
            User* actor = findUser(a);
            User* who = findUser(b);
            if (!actor || !who) {
                cout << "    主号不存在。\n";
                continue;
            }
            int up = askInt("[1=设为管理员 2=撤销管理员]：");
            if (up == 1) g->setAdmin(actor, who);
            else if (up == 2) g->cancelAdmin(actor, who);
        } else if (k == 7) {
            Group* g = pickGroup("选择群：");
            if (!g) continue;
            string a = askStr("输入退群者主号：");
            User* who = findUser(a);
            if (who) g->quit(who);
            else cout << "    无该用户。\n";
        } else if (k == 8) {
            string id = askStr("输入要查询的用户主号：");
            User* who = findUser(id);
            if (!who) {
                cout << "    无该用户。\n";
                continue;
            }
            cout << who->nick() << " 加入的群：\n";
            for (size_t i = 0; i < groups_.size(); ++i) {
                if (!groups_[i]->isMember(who)) continue;
                cout << "    [" << groups_[i]->kindName() << "] 群号 "
                     << groups_[i]->gno() << " 群名 "
                     << groups_[i]->name() << "\n";
            }
        } else if (k == 9) {
            Group* g = pickGroup("选择群（建议选QQ群）：");
            if (!g) continue;
            cout << "    群类型：" << g->kindName()
                 << "  能否建临时讨论组："
                 << (g->canCreateSub() ? "能" : "不能") << "\n";

            string name = askStr("输入子群名（不含空格）：");
            if (name.empty()) {
                cout << "    子群名不能为空。\n";
                continue;
            }

            vector<User*> picks;
            User* o = pickUser("选择要拉进子群的成员：");
            if (o) picks.push_back(o);
            User* o2 = pickUser("再选一名（取消则只有群主）：");
            if (o2) picks.push_back(o2);

            Group* sub = g->createSubGroup(tempNo_ + 1, name, picks);
            if (sub) {
                ++tempNo_;
                groups_.push_back(sub);
                cout << "    已建子群并加入平台群列表：\n";
                sub->show();
            }
        } else if (k == 10) {
            Group* g = pickGroup("选择要切换管理模式的群：");
            if (!g) continue;
            int before = g->memberCount();
            cout << "    切换前成员数 = " << before << "\n";

            Group* twin = g->modeTwin();
            if (twin) {
                cout << "    → 生成镜像群(" << twin->kindName()
                     << ")，成员数 = " << twin->memberCount()
                     << "（与切换前一致）\n";
                twin->show();
                delete twin;
            }
        } else {
            cout << "    没有这个选项，请重新输入。\n";
        }
    }
}

// ==================== 文件演示菜单 ====================

void Platform::menuFile() {
    divider("文件读写演示");
    cout << "先保存，再用一个新的平台实例把文件读回来验证：\n";
    save();

    // autoSave = false：这个实例只负责读回验证，析构时不要再写一遍
    Platform check(file_, false);
    cout << "\n[读回验证] 新实例：用户 " << check.userCount()
         << " 位，群 " << check.groupCount() << " 个\n";
    check.showBrief();
}

// ==================== 主菜单 ====================

void Platform::run() {
    while (true) {
        cout << "\n################ 腾讯立体社交平台模拟系统 ################\n";
        cout << "  当前数据：用户 " << users_.size() << " 位，群 "
             << groups_.size() << " 个，文件：" << file_ << "\n";
        cout << "  1. 一条龙自动演示\n"
                "  2. 用户管理\n"
                "  3. 好友管理\n"
                "  4. 群管理\n"
                "  5. 数据总览\n"
                "  6. 文件读写演示\n"
                "  0. 退出\n";

        int k = askInt("请选择：");
        if (k == 0 || k == -2) {
            cout << "正在退出，析构时自动把数据写回文件...\n";
            return;
        }

        if (k == 1) autoDemo();
        else if (k == 2) menuUser();
        else if (k == 3) menuFriend();
        else if (k == 4) menuGroup();
        else if (k == 5) menuOverview();
        else if (k == 6) menuFile();
        else cout << "    没有这个选项，请重新输入。\n";
    }
}

void Platform::autoDemo() {
    divider("自动演示开始");
    buildDemo();   // 每次都从同一份固化数据开始，保证演示结果可重复
    demoPause(DEMO_PAUSE_MS / 2);

    demoOpenServices();
    demoPause(DEMO_SECTION_MS);
    demoLogin();
    demoPause(DEMO_SECTION_MS);
    demoFriendCRUD();
    demoPause(DEMO_SECTION_MS);
    demoCommonFriend();
    demoPause(DEMO_SECTION_MS);
    demoWeChatRecFriend();
    demoPause(DEMO_SECTION_MS);
    demoGroups();
    demoPause(DEMO_SECTION_MS);
    demoModeSwitch();
    demoPause(DEMO_SECTION_MS);

    divider("演示后的数据总览");
    demoPause(DEMO_PAUSE_MS / 2);
    showBrief();
    demoPause(DEMO_SECTION_MS);

    cout << "\n自动演示完毕，退出时会把数据写回 " << file_ << "。\n";
}
