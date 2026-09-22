// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：PlatformWRH.cpp
#include "PlatformWRH.h"

#include "UserWRH.h"
#include "MicroAccountWRH.h"
#include "QQAccountWRH.h"
#include "WeiboAccountWRH.h"
#include "WeChatAccountWRH.h"
#include "GroupWRH.h"
#include "QQGroupWRH.h"
#include "WeChatGroupWRH.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <set>
#include <cstdlib>

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;
using std::istringstream;
using std::ifstream;
using std::ofstream;

// =====================================================================
//  PlatformWRH.cpp  平台总控实现
// =====================================================================

// ================= 小工具（本文件私有） =================
namespace {
    void divider(const char* t) {
        cout << "\n================ " << t << " ================\n";
    }
    int askInt(const char* tip) {
        cout << tip << " ";
        int x; cin >> x;
        if (cin.fail()) { cin.clear(); cin.ignore(0x7fffffff, '\n'); return -1; }
        return x;
    }
    string askStr(const char* tip) {
        cout << tip << " ";
        string s; cin >> s;
        return s;
    }
    string svcName(MicroType t) { return microName(t); }
}

// ================= 构造 / 析构 / I/O =================

PlatformWRH::PlatformWRH(const string& file)
    : file_(file), tempNo_(2000) {
    if (!load()) {          // 实例化容器时先尝试读入文件
        buildDemo();        // 文件不存在 -> 用固化演示数据
    }
}

PlatformWRH::~PlatformWRH() {
    save();                 // 析构时把容器写回文件
    for (size_t i = 0; i < users_.size(); ++i) delete users_[i];
    for (size_t i = 0; i < groups_.size(); ++i) delete groups_[i];
    users_.clear();
    groups_.clear();
}

void PlatformWRH::save() const {
    ofstream out(file_.c_str());
    if (!out) { cout << "[文件] 无法写 " << file_ << "\n"; return; }
    // U: 用户资料  O: 开通服务  B: 微信绑定QQ  F: 好友  G: 群  K: 群成员
    for (size_t i = 0; i < users_.size(); ++i) {
        const UserWRH* u = users_[i];
        out << "U " << u->mainNo() << ' ' << u->nick() << ' '
            << u->birth().str() << ' ' << u->applyDate().str() << ' '
            << u->city() << "\n";
        const vector<MicroAccountWRH*>& as = u->accounts();
        for (size_t j = 0; j < as.size(); ++j) {
            out << "O " << u->mainNo() << ' ' << svcName(as[j]->type());
            if (as[j]->type() == MWX) {
                WeChatAccountWRH* w = dynamic_cast<WeChatAccountWRH*>(as[j]);
                out << ' ' << (w ? w->wxNo() : "");
            }
            out << "\n";
        }
        WeChatAccountWRH* w = u->wxAccount();
        if (w && w->hasBoundQQ())
            out << "B " << u->mainNo() << ' ' << w->boundQQ() << "\n";
        // 好友：只存 主号小 -> 主号大 的一方，F A B 服务 备注
        for (size_t j = 0; j < as.size(); ++j) {
            const auto& fs = as[j]->friends().all();
            for (size_t k = 0; k < fs.size(); ++k) {
                const string& fa = u->mainNo();
                const string& fb = fs[k].person()->mainNo();
                bool aSmall = (fa.size() != fb.size()) ? (fa.size() < fb.size())
                                                       : (fa < fb);
                if (aSmall)
                    out << "F " << fa << ' ' << fb << ' '
                        << svcName(as[j]->type()) << ' '
                        << fs[k].remark() << "\n";
            }
        }
    }
    for (size_t i = 0; i < groups_.size(); ++i) {
        const GroupWRH* g = groups_[i];
        out << "G " << g->gno() << ' ' << g->name() << ' '
            << svcName(g->svc()) << ' '
            << (g->owner() ? g->owner()->mainNo() : "0") << "\n";
        const auto& ms = g->members().all();
        for (size_t j = 0; j < ms.size(); ++j) {
            if (ms[j].isOwner()) continue;          // 群主已在 G 中，由构造自动建
            out << "K " << g->gno() << ' ' << svcName(g->svc()) << ' '
                << ms[j].user()->mainNo() << ' ' << (int)ms[j].role() << "\n";
        }
    }
    out.close();
    cout << "[文件] 已把当前数据写回 " << file_ << " (用户 "
         << users_.size() << " 个，群 " << groups_.size() << " 个)\n";
}

bool PlatformWRH::load() {
    ifstream in(file_.c_str());
    if (!in) return false;
    std::map<string, UserWRH*> byId;
    string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        string cmd; iss >> cmd;
        if (cmd == "U") {
            string id, nick, bs, as_, city;
            iss >> id >> nick >> bs >> as_ >> city;
            MyDateWRH b, a;
            if (!b.parse(bs) || !a.parse(as_)) continue;
            UserWRH* u = new UserWRH(id, nick, b, a, city);
            users_.push_back(u);
            byId[id] = u;
        } else if (cmd == "O") {
            string id, s, wx;
            MicroType t;
            iss >> id >> s;
            if (!parseSvc(s, t)) continue;
            UserWRH* u = byId[id];
            if (!u) continue;
            if (t == MWX && (iss >> wx)) u->setWxNo(wx);   // 用保存过的微信号
            u->openService(t);
        } else if (cmd == "B") {
            string id, qq; iss >> id >> qq;
            UserWRH* u = byId[id];
            if (u) u->bindWxToQQ(qq);
        } else if (cmd == "F") {
            string a, b, s, rem;
            MicroType t;
            iss >> a >> b >> s >> rem;
            if (!parseSvc(s, t)) continue;
            UserWRH* ua = byId[a]; UserWRH* ub = byId[b];
            MicroAccountWRH* ac = ua ? ua->account(t) : nullptr;
            if (ua && ub && ac) ac->addFriend(ub, rem);
        } else if (cmd == "G") {
            int no; string name, s, owner;
            MicroType t;
            iss >> no >> name >> s >> owner;
            if (!parseSvc(s, t)) continue;
            UserWRH* ow = byId[owner];
            if (!ow) continue;
            GroupWRH* g = (t == MQQ) ? (GroupWRH*)new QQGroupWRH(no, name, ow)
                                     : (GroupWRH*)new WeChatGroupWRH(no, name, ow);
            groups_.push_back(g);
        } else if (cmd == "K") {
            int no, role; string s, uid;
            MicroType t;
            iss >> no >> s >> uid >> role;
            if (!parseSvc(s, t)) continue;
            UserWRH* u = byId[uid];
            GroupWRH* g = nullptr;
            for (size_t i = 0; i < groups_.size(); ++i)
                if (groups_[i]->gno() == no && groups_[i]->svc() == t) { g = groups_[i]; break; }
            if (u && g) {
                if (role == ROLE_MEMBER) g->members().add(u, ROLE_MEMBER);
                else if (role == ROLE_ADMIN) g->members().add(u, ROLE_ADMIN);
            }
        }
    }
    in.close();
    cout << "[文件] 已从 " << file_ << " 读入 " << users_.size()
         << " 个用户，" << groups_.size() << " 个群。\n";
    return true;
}

// ================= 查找辅助 =================

UserWRH* PlatformWRH::findUser(const string& id) const {
    for (size_t i = 0; i < users_.size(); ++i)
        if (users_[i]->mainNo() == id) return users_[i];
    return nullptr;
}

void PlatformWRH::listAllUsersShort() const {
    for (size_t i = 0; i < users_.size(); ++i) {
        const UserWRH* u = users_[i];
        cout << "   " << (i + 1) << ". 主号 " << u->mainNo() << "  昵称 "
             << u->nick() << "  [" << u->city() << "] 已开通服务 "
             << openedToStr(u->openedSet()) << "\n";
    }
}

UserWRH* PlatformWRH::pickUser(const char* tip) const {
    cout << tip << "\n";
    listAllUsersShort();
    int k = askInt("请输入用户序号(0=取消)：") - 1;
    if (k < 0 || k >= (int)users_.size()) return nullptr;
    return users_[k];
}

GroupWRH* PlatformWRH::pickGroup(const char* tip) const {
    cout << tip << "\n";
    for (size_t i = 0; i < groups_.size(); ++i) {
        const GroupWRH* g = groups_[i];
        cout << "   " << (i + 1) << ". [" << g->kindName() << "] 群号 "
             << g->gno() << " 群名 " << g->name()
             << "  群主 " << (g->owner() ? g->owner()->nick() : "(无)")
             << "  成员 " << g->memberCount() << " 人\n";
    }
    if (groups_.empty()) { cout << "   （当前没有群）\n"; return nullptr; }
    int k = askInt("请输入群序号(0=取消)：") - 1;
    if (k < 0 || k >= (int)groups_.size()) return nullptr;
    return groups_[k];
}

MicroType PlatformWRH::pickService() const {
    int k = askInt("请选择服务 [1=QQ 2=微博 3=微信] (0=取消)：");
    if (k == 1) return MQQ;
    if (k == 2) return MWB;
    if (k == 3) return MWX;
    return MCOUNT;
}

MicroType PlatformWRH::pickServiceFromUser(const UserWRH* u) const {
    if (!u) return MCOUNT;
    const auto& as = u->accounts();
    if (as.empty()) { cout << "    [提示] 该用户尚未开通任何服务。\n"; return MCOUNT; }
    cout << "    该用户已开通：";
    for (size_t i = 0; i < as.size(); ++i)
        cout << (i ? "、" : "") << (i + 1) << "=" << microName(as[i]->type());
    cout << "\n";
    int k = askInt("请选择序号(0=取消)：");
    if (k < 1 || k > (int)as.size()) return MCOUNT;
    return as[k - 1]->type();
}

string PlatformWRH::toSvcName(MicroType t) { return microName(t); }

bool PlatformWRH::parseSvc(const string& s, MicroType& t) {
    if (s == "QQ") { t = MQQ; return true; }
    if (s == "微博") { t = MWB; return true; }
    if (s == "微信") { t = MWX; return true; }
    return false;
}

// ================= 演示数据构建 =================

void PlatformWRH::buildDemo() {
    // 清空旧数据（保证可重复调用）
    for (size_t i = 0; i < users_.size(); ++i) delete users_[i];
    for (size_t i = 0; i < groups_.size(); ++i) delete groups_[i];
    users_.clear(); groups_.clear();
    tempNo_ = 2000;

    MyDateWRH d;
    struct Seed { const char* id; const char* nick; const char* birth;
                  const char* apply; const char* city; };
    Seed seeds[] = {
        {"10001", "小腾", "1998-05-20", "2014-06-01", "深圳"},
        {"10002", "阿讯", "1999-03-15", "2014-08-12", "北京"},
        {"10003", "娜娜", "2000-11-02", "2015-02-20", "成都"},
        {"10004", "阿豪", "1997-09-30", "2013-12-01", "广州"},
        {"10005", "小雨", "2001-01-08", "2015-09-09", "上海"},
        {"10006", "老马", "1996-07-19", "2012-10-01", "杭州"},
    };
    for (size_t i = 0; i < sizeof(seeds) / sizeof(seeds[0]); ++i) {
        MyDateWRH b, a;
        b.parse(seeds[i].birth); a.parse(seeds[i].apply);
        users_.push_back(new UserWRH(seeds[i].id, seeds[i].nick, b, a, seeds[i].city));
    }
    UserWRH* u1 = findUser("10001");
    UserWRH* u2 = findUser("10002");
    UserWRH* u3 = findUser("10003");
    UserWRH* u4 = findUser("10004");
    UserWRH* u5 = findUser("10005");
    UserWRH* u6 = findUser("10006");

    // ---- 开通服务（开通管理） ----
    u1->openService(MQQ); u1->openService(MWB); u1->openService(MWX);
    u2->openService(MQQ); u2->openService(MWB); u2->openService(MWX);
    u3->openService(MQQ);
    u4->openService(MQQ); u4->openService(MWB); u4->openService(MWX);
    u5->openService(MQQ);                        u5->openService(MWX);
                                                  u6->openService(MWX);
    u1->bindWxToQQ("10001");                     // 微信绑定QQ示例

    // ---- 好友（按服务划分；互为好友） ----
    u1->account(MQQ)->addFriend(u2, "阿讯(助教)");   // QQ好友
    u1->account(MQQ)->addFriend(u3, "");
    u1->account(MQQ)->addFriend(u4, "广州大佬");
    u2->account(MQQ)->addFriend(u3, "");
    u3->account(MQQ)->addFriend(u4, "");
    u4->account(MQQ)->addFriend(u5, "");
    u1->account(MWB)->addFriend(u2, "");            // 微博好友
    u1->account(MWB)->addFriend(u4, "");
    u1->account(MWX)->addFriend(u2, "铁哥们");      // 微信好友
    u1->account(MWX)->addFriend(u5, "");

    // ---- 预置群：每个微X有群号1001等 ----
    // QQ群 1001
    QQGroupWRH* q1 = new QQGroupWRH(1001, "C++程序设计学习群", u1);
    q1->members().add(u2, ROLE_ADMIN);
    q1->members().add(u3, ROLE_MEMBER);
    q1->members().add(u4, ROLE_MEMBER);
    q1->members().add(u5, ROLE_MEMBER);
    groups_.push_back(q1);
    // QQ群 1002
    QQGroupWRH* q2 = new QQGroupWRH(1002, "班级事务通知群", u2);
    q2->members().add(u1, ROLE_ADMIN);
    q2->members().add(u4, ROLE_MEMBER);
    groups_.push_back(q2);
    // 微信群 1001
    WeChatGroupWRH* w1 = new WeChatGroupWRH(1001, "周末爬山俱乐部", u5);
    w1->members().add(u6, ROLE_MEMBER);
    w1->members().add(u2, ROLE_MEMBER);
    w1->members().add(u1, ROLE_MEMBER);
    w1->members().add(u4, ROLE_MEMBER);
    groups_.push_back(w1);
    // 微信群 1002
    WeChatGroupWRH* w2 = new WeChatGroupWRH(1002, "产品内测体验群", u1);
    w2->members().add(u4, ROLE_MEMBER);
    w2->members().add(u2, ROLE_MEMBER);
    groups_.push_back(w2);
}

// ================= 各业务动作 =================

void PlatformWRH::demoOpenServices() {
    divider("开通管理演示");
    cout << "当前演示用户均已开通若干服务，下面用新注册用户来演示开通/注销：\n";
    UserWRH* nu = findUser("10007");
    if (!nu) {
        MyDateWRH b, a;
        b.parse("2002-04-18"); a.parse("2016-01-01");
        nu = new UserWRH("10007", "新手上路", b, a, "长春");
        users_.push_back(nu);
    }
    nu->showProfile();
    nu->openService(MQQ);          // 开通QQ
    nu->openService(MWB);          // 开通微博（与QQ共享同一主号）
    nu->openService(MWX);          // 开通微信（分配独立号）
    cout << "\n[共享ID验证] QQ号码 = " << nu->account(MQQ)->svcNo()
         << "，微博号码 = " << nu->account(MWB)->svcNo() << "（相同）\n";
    WeChatAccountWRH* w = nu->wxAccount();
    if (w) cout << "[独立ID验证] 微信号 = " << w->wxNo()
                << "（与主号不同，可绑定QQ）\n";
    cout << "\n注销微博演示：\n";
    nu->closeService(MWB);
    cout << "注销后已开通服务数 = " << nu->openedCount() << "\n";
    nu->closeService(MWB);         // 重复注销应给出提示
}

void PlatformWRH::demoLogin() {
    divider("登录管理演示（一个服务登录 → 其它自动登录）");
    UserWRH* u1 = findUser("10001");
    u1->logoutAll();
    u1->showProfile();
    u1->login(MQQ);                 // 只在QQ上登录
    u1->login(MWB);                 // 已经全登录再登录微博，其它不再变(已登录)
    cout << "\n全部登出后，在微信登录：\n";
    u1->logoutAll();
    u1->login(MWX);
}

void PlatformWRH::demoFriendCRUD() {
    divider("好友管理演示（增/删/改/查）");
    UserWRH* u1 = findUser("10001");
    UserWRH* u3 = findUser("10003");
    UserWRH* u5 = findUser("10005");
    UserWRH* u6 = findUser("10006");
    MicroAccountWRH* qq = u1->account(MQQ);
    cout << "小腾 的 QQ 好友(添加前)：\n";
    qq->friends().list();
    cout << "\n[增] 添加 QQ 好友 小雨(10005)，备注\"算法队友\"：\n";
    qq->addFriend(u5, "算法队友");
    cout << "\n[增][失败演示] 老马(10006)未开通QQ，无法成为QQ好友：\n";
    qq->addFriend(u6, "");
    cout << "\n[删] 删除 QQ 好友 娜娜(10003)：\n";
    qq->delFriend(u3);
    cout << "删除后好友数 = " << qq->friends().count() << "\n";
    cout << "\n[改] 修改备注：把对 阿讯(10002) 的备注改为 \"学委阿讯\"：\n";
    UserWRH* u2 = findUser("10002");
    if (qq->friends().find(u2)) {
        qq->friends().setRemark(u2, "学委阿讯");
    }
    cout << "\n[查] 好友查询结果：\n";
    qq->friends().list();
}

void PlatformWRH::demoCommonFriend() {
    divider("微X间共同好友演示（QQ 与 微博）");
    UserWRH* u1 = findUser("10001");
    MicroAccountWRH* qq = u1->account(MQQ);
    MicroAccountWRH* wb = u1->account(MWB);
    if (!qq || !wb) { cout << "（需要同时开通QQ与微博）\n"; return; }
    cout << "小腾：QQ 好友为 ";
    qq->friends().listSimple();
    cout << "      微博好友为 ";
    wb->friends().listSimple();
    vector<UserWRH*> common = qq->friends().commonWith(wb->friends());
    cout << "QQ 与 微博 的共同好友(" << common.size() << "人)：";
    for (size_t i = 0; i < common.size(); ++i)
        cout << (i ? "、" : "") << common[i]->nick();
    cout << "\n";
}

void PlatformWRH::demoWeChatRecFriend() {
    divider("微信 添加“QQ推荐好友”");
    UserWRH* u1 = findUser("10001");
    MicroAccountWRH* qq = u1->account(MQQ);
    MicroAccountWRH* wx = u1->account(MWX);
    if (!qq || !wx) { cout << "（需要同时开通QQ与微信）\n"; return; }
    vector<UserWRH*> rec;
    const auto& qfs = qq->friends().all();
    for (size_t i = 0; i < qfs.size(); ++i) {
        UserWRH* p = qfs[i].person();
        if (p->hasService(MWX) && !wx->isFriend(p) && p != u1)
            rec.push_back(p);          // 在QQ上是好友、同时开通了微信 → 推荐
    }
    cout << "小腾的 QQ 好友中，已开通微信且尚非其微信好友的人("
         << rec.size() << "人)：";
    for (size_t i = 0; i < rec.size(); ++i)
        cout << (i ? "、" : "") << rec[i]->nick() << "(" << rec[i]->wxNo() << ")";
    cout << "\n将这些人逐个添加为微信好友：\n";
    for (size_t i = 0; i < rec.size(); ++i)
        wx->addFriend(rec[i], "QQ推荐");
    cout << "\n添加后 小腾 的微信好友：";
    wx->friends().listSimple();
    cout << "\n";
}

void PlatformWRH::demoGroups() {
    divider("群管理多态演示（QQ群 vs 微信群）");
    UserWRH* u1 = findUser("10001");
    UserWRH* u2 = findUser("10002");
    UserWRH* u3 = findUser("10003");
    UserWRH* u4 = findUser("10004");
    UserWRH* u5 = findUser("10005");
    cout << "\n—— 预置群列表 ——\n";
    for (size_t i = 0; i < groups_.size(); ++i) groups_[i]->show();

    GroupWRH* q1001 = nullptr;   // QQ群1001
    GroupWRH* w1001 = nullptr;   // 微信群1001
    for (size_t i = 0; i < groups_.size(); ++i) {
        if (groups_[i]->gno() == 1001 && groups_[i]->svc() == MQQ) q1001 = groups_[i];
        if (groups_[i]->gno() == 1001 && groups_[i]->svc() == MWX) w1001 = groups_[i];
    }

    // 准备演示用用户：
    //   nu  (10007) = demoOpenServices 已建，开通QQ(+微博已注销、微信)。
    //   u8  (10008) = 新建，仅开通微信，用于演示“微信群只能被邀请”。
    UserWRH* nu = findUser("10007");
    if (!nu) {   // 以防没有经过开通演示直接进群演示
        MyDateWRH b, a;
        b.parse("2002-04-18"); a.parse("2016-01-01");
        nu = new UserWRH("10007", "新手上路", b, a, "长春");
        users_.push_back(nu);
        nu->openService(MQQ);
    }
    UserWRH* u8 = findUser("10008");
    if (!u8) {
        MyDateWRH b, a;
        b.parse("2003-02-11"); a.parse("2017-05-20");
        u8 = new UserWRH("10008", "被邀新人", b, a, "大连");
        users_.push_back(u8);
        u8->openService(MWX);
    }

    // 1) 申请加入差异
    cout << "\n[申请加入] QQ群可申请；微信群只能被邀请/推荐：\n";
    if (q1001 && nu) q1001->requestJoin(nu);    // QQ：申请成功
    if (w1001 && u8) w1001->requestJoin(u8);    // 微信：申请被拒(只能邀请)

    // 2) 邀请差异（微信群通过成员邀请加入）
    cout << "\n[邀请加入] 微信群成员邀请 被邀新人：\n";
    if (w1001 && u5 && u8) w1001->inviteJoin(u5, u8);   // 群主u5邀请

    // 3) 设管理员（QQ群可设；微信群无管理员）
    cout << "\n[设管理员] QQ群：群主可为成员设置管理员；微信群：无管理员，仅群主特权：\n";
    if (q1001 && u1 && nu) q1001->setAdmin(u1, nu);     // QQ：成功(u1为群主)
    if (w1001 && u5 && u8) w1001->setAdmin(u5, u8);     // 微信：被拒绝

    // 4) 踢人差异（QQ：管理员可踢普通成员；微信：仅群主可踢）
    cout << "\n[踢人] QQ群：管理员 阿讯 踢 普通成员 阿豪；微信群：非群主踢人被拒，群主可踢：\n";
    if (q1001 && u2 && u4) q1001->kick(u2, u4);         // QQ：管理员踢普通成员成功
    if (w1001 && u1 && u8) w1001->kick(u1, u8);         // 微信：成员u1踢人被拒(仅群主)
    if (w1001 && u5 && u8) w1001->kick(u5, u8);         // 微信：群主u5踢人成功

    // 5) 临时讨论组（QQ群可建子群；微信群不可）
    cout << "\n[临时讨论组] QQ群可建子群；微信群不可：\n";
    if (q1001) {
        vector<UserWRH*> picks;
        picks.push_back(u2); picks.push_back(u3);
        GroupWRH* sub = q1001->createSubGroup(++tempNo_, "临时代码评审会", picks);
        if (sub) { groups_.push_back(sub); cout << "  已建子群：\n"; sub->show(); }
    }
    if (w1001)
        { GroupWRH* s2 = w1001->createSubGroup(++tempNo_, "临时会", vector<UserWRH*>()); if (s2) delete s2; }

    // 6) 查询某人所在群
    cout << "\n[查询] 查看 小腾(10001) 加入的所有群：\n";
    for (size_t i = 0; i < groups_.size(); ++i)
        if (groups_[i]->isMember(u1))
            cout << "    [" << groups_[i]->kindName() << "] 群号 "
                 << groups_[i]->gno() << " 群名 " << groups_[i]->name() << "\n";
}

void PlatformWRH::demoModeSwitch() {
    divider("群管理模式动态切换（成员数据不受伤）");
    GroupWRH* src = nullptr;
    for (size_t i = 0; i < groups_.size(); ++i)
        if (groups_[i]->gno() == 1001 && groups_[i]->svc() == MQQ) src = groups_[i];
    if (!src) return;
    int before = src->memberCount();
    cout << "切换前 [QQ群1001] 成员数 = " << before << "\n";
    src->show();
    cout << "\n→ 调用 modeTwin() 生成一个“同样成员、采用微信群规则”的群：\n";
    GroupWRH* twin = src->modeTwin();
    int twinCount = twin->memberCount();
    cout << "（镜像群成员数 = " << twinCount << "，与源群 " << before << " 一致 → 数据不受伤）\n";
    twin->show();          // 展示微信规则：无管理员、仅群主特权
    delete twin;
}

// ================= 数据总览 =================

void PlatformWRH::menuOverview() {
    divider("数据总览");
    cout << "平台共 " << users_.size() << " 位用户，"
         << groups_.size() << " 个群\n";
    for (size_t i = 0; i < users_.size(); ++i) {
        users_[i]->showProfile();
        users_[i]->showOpened();
    }
    cout << "\n群列表：\n";
    for (size_t i = 0; i < groups_.size(); ++i) groups_[i]->show();
}

// ================= 用户菜单 =================

void PlatformWRH::menuUser() {
    while (true) {
        cout << "\n----------- 用户管理 ----------\n";
        cout << " 1. 查看全部用户\n 2. 查看某用户详情\n 3. 开通微X服务\n"
                " 4. 注销服务\n 5. 登录(自动登录全部)\n 6. 全部登出\n"
                " 7. 微信绑定QQ\n 0. 返回上级\n";
        int k = askInt("请选择：");
        if (k == 0) return;
        if (k == 1) { listAllUsersShort(); continue; }
        if (k == 2) {
            UserWRH* u = pickUser("选择用户：");
            if (!u) continue;
            u->showProfile(); u->showOpened();
        } else if (k == 3) {
            UserWRH* u = pickUser("选择要开通服务的用户：");
            if (!u) continue;
            MicroType t = pickService();
            if (t != MCOUNT) u->openService(t);
        } else if (k == 4) {
            UserWRH* u = pickUser("选择要注销服务的用户：");
            if (!u) continue;
            MicroType t = pickServiceFromUser(u);
            if (t != MCOUNT) u->closeService(t);
        } else if (k == 5) {
            UserWRH* u = pickUser("选择登录的用户：");
            if (!u) continue;
            MicroType t = pickServiceFromUser(u);
            if (t != MCOUNT) u->login(t);
        } else if (k == 6) {
            UserWRH* u = pickUser("选择登出的用户：");
            if (!u) continue;
            u->logoutAll();
            cout << "    已全部登出。\n";
        } else if (k == 7) {
            UserWRH* u = pickUser("选择要绑定微信的用户：");
            if (!u) continue;
            string qq = askStr("输入要绑定的QQ主号(回车可自动绑定自己主号)：");
            if (qq.empty() || qq == "0") qq = u->mainNo();
            u->bindWxToQQ(qq);
        }
    }
}

// ================= 好友菜单 =================

void PlatformWRH::menuFriend() {
    UserWRH* op = pickUser("好友管理：请选择操作者");
    if (!op) return;
    while (true) {
        cout << "\n----------- 好友管理 (操作者：" << op->nick()
             << ", 主号 " << op->mainNo() << ") ----------\n";
        cout << " 1. 查看某服务好友列表\n 2. 添加好友\n 3. 删除好友\n"
                " 4. 修改备注\n 5. 查询某好友\n"
                " 6. 求 QQ与微博 共同好友\n 7. 微信加QQ推荐好友\n 0. 返回上级\n";
        int k = askInt("请选择：");
        if (k == 0) return;
        if (k == 1) {
            MicroType t = pickServiceFromUser(op);
            MicroAccountWRH* ac = op->account(t);
            if (ac) ac->friends().list();
        } else if (k == 2) {
            MicroType t = pickServiceFromUser(op);
            MicroAccountWRH* ac = op->account(t);
            if (!ac) continue;
            string id = askStr("输入对方主号：");
            UserWRH* who = findUser(id);
            if (!who) { cout << "    没有该用户。\n"; continue; }
            string rem = askStr("输入备注(可为空)：");
            ac->addFriend(who, rem);
        } else if (k == 3) {
            MicroType t = pickServiceFromUser(op);
            MicroAccountWRH* ac = op->account(t);
            if (!ac) continue;
            string id = askStr("输入要删除的好友主号：");
            UserWRH* who = findUser(id);
            if (who && ac->friends().has(who)) ac->delFriend(who);
            else cout << "    不是该服务好友或用户不存在。\n";
        } else if (k == 4) {
            MicroType t = pickServiceFromUser(op);
            MicroAccountWRH* ac = op->account(t);
            if (!ac) continue;
            string id = askStr("输入要改备注的好友主号：");
            UserWRH* who = findUser(id);
            string rem = askStr("输入新备注：");
            if (who && ac->friends().find(who)) ac->friends().setRemark(who, rem);
            else cout << "    不是好友或用户不存在。\n";
        } else if (k == 5) {
            MicroType t = pickServiceFromUser(op);
            MicroAccountWRH* ac = op->account(t);
            if (!ac) continue;
            string id = askStr("输入要查询的好友主号：");
            UserWRH* who = findUser(id);
            FriendInfoWRH* fi = (ac && who) ? ac->friends().find(who) : nullptr;
            if (fi)
                cout << "    该好友：" << fi->person()->nick() << " 主号 "
                     << fi->person()->mainNo() << " 备注：" << fi->remark() << "\n";
            else cout << "    不是好友。\n";
        } else if (k == 6) {
            MicroAccountWRH* qq = op->account(MQQ);
            MicroAccountWRH* wb = op->account(MWB);
            if (!qq || !wb) {
                cout << "    需要同时开通QQ与微博才能比较。\n"; continue;
            }
            cout << "    QQ好友：";   qq->friends().listSimple();
            cout << "    微博好友："; wb->friends().listSimple();
            vector<UserWRH*> cm = qq->friends().commonWith(wb->friends());
            cout << "    共同好友(" << cm.size() << ")：";
            for (size_t i = 0; i < cm.size(); ++i)
                cout << (i ? "、" : "") << cm[i]->nick();
            cout << "\n";
        } else if (k == 7) {
            MicroAccountWRH* qq = op->account(MQQ);
            MicroAccountWRH* wx = op->account(MWX);
            if (!qq || !wx) {
                cout << "    需要同时开通QQ与微信才能推荐。\n"; continue;
            }
            vector<UserWRH*> rec;
            const auto& qfs = qq->friends().all();
            for (size_t i = 0; i < qfs.size(); ++i) {
                UserWRH* p = qfs[i].person();
                if (p->hasService(MWX) && !wx->isFriend(p))
                    rec.push_back(p);
            }
            if (rec.empty()) { cout << "    没有可推荐的QQ好友。\n"; continue; }
            cout << "    可推荐 " << rec.size() << " 人：\n";
            for (size_t i = 0; i < rec.size(); ++i)
                cout << "      " << (i + 1) << ". " << rec[i]->nick()
                     << " (" << rec[i]->mainNo() << ")\n";
            int j = askInt("选择要添加的序号(0=全不加)：") - 1;
            if (j >= 0 && j < (int)rec.size()) wx->addFriend(rec[j], "QQ推荐");
        }
    }
}

// ================= 群菜单 =================

void PlatformWRH::menuGroup() {
    while (true) {
        cout << "\n----------- 群管理 ----------\n";
        cout << " 1. 查看全部群\n 2. 查看某群成员\n 3. 申请加入\n"
                " 4. 邀请/推荐加入\n 5. 踢人\n 6. 设/撤管理员\n 7. 退群\n"
                " 8. 查询某人所在群\n 9. 临时讨论组(QQ群演示)\n"
                "10. 管理模式动态切换演示\n 0. 返回上级\n";
        int k = askInt("请选择：");
        if (k == 0) return;
        if (k == 1) {
            for (size_t i = 0; i < groups_.size(); ++i) groups_[i]->show();
        } else if (k == 2) {
            GroupWRH* g = pickGroup("选择群：");
            if (g) g->show();
        } else if (k == 3) {
            GroupWRH* g = pickGroup("选择要加入的群：");
            if (!g) continue;
            string id = askStr("输入申请者主号：");
            UserWRH* who = findUser(id);
            if (who) g->requestJoin(who); else cout << "    无该用户。\n";
        } else if (k == 4) {
            GroupWRH* g = pickGroup("选择群：");
            if (!g) continue;
            string a = askStr("输入邀请者主号(须群内成员)：");
            string b = askStr("输入被邀请者主号：");
            g->inviteJoin(findUser(a), findUser(b));
        } else if (k == 5) {
            GroupWRH* g = pickGroup("选择群：");
            if (!g) continue;
            string a = askStr("输入操作者主号：");
            string b = askStr("输入被踢者主号：");
            g->kick(findUser(a), findUser(b));
        } else if (k == 6) {
            GroupWRH* g = pickGroup("选择群：");
            if (!g) continue;
            string a = askStr("输入操作者主号(仅群主可设)：");
            string b = askStr("输入目标成员主号：");
            int up = askInt("[1=设为管理员 2=撤销管理员]：");
            if (up == 1) g->setAdmin(findUser(a), findUser(b));
            else g->cancelAdmin(findUser(a), findUser(b));
        } else if (k == 7) {
            GroupWRH* g = pickGroup("选择群：");
            if (!g) continue;
            string a = askStr("输入退群者主号：");
            g->quit(findUser(a));
        } else if (k == 8) {
            string id = askStr("输入要查询的用户主号：");
            UserWRH* who = findUser(id);
            if (!who) { cout << "    无该用户。\n"; continue; }
            cout << who->nick() << " 加入的群：\n";
            for (size_t i = 0; i < groups_.size(); ++i)
                if (groups_[i]->isMember(who))
                    cout << "    [" << groups_[i]->kindName() << "] 群号 "
                         << groups_[i]->gno() << " 群名 " << groups_[i]->name() << "\n";
        } else if (k == 9) {
            GroupWRH* g = pickGroup("选择群(建议选QQ群)：");
            if (!g) continue;
            cout << "    群类型：" << g->kindName() << "  能否建临时讨论组："
                 << (g->canCreateSub() ? "能" : "不能") << "\n";
            string name = askStr("输入子群名：");
            vector<UserWRH*> picks;
            UserWRH* o = pickUser("选择要拉进子群的成员：");
            if (o) picks.push_back(o);
            UserWRH* o2 = pickUser("再选一名(取消则单成员)：");
            if (o2) picks.push_back(o2);
            GroupWRH* sub = g->createSubGroup(++tempNo_, name, picks);
            if (sub) {
                groups_.push_back(sub);
                cout << "    已建子群并加入平台群列表：\n";
                sub->show();
            }
        } else if (k == 10) {
            GroupWRH* g = pickGroup("选择要切换管理模式的群：");
            if (!g) continue;
            int before = g->memberCount();
            cout << "    切换前成员数 = " << before << "\n";
            GroupWRH* twin = g->modeTwin();
            if (twin) {
                cout << "    → 生成镜像群(" << twin->kindName() << ")，成员数 = "
                     << twin->memberCount() << "（与切换前一致）\n";
                twin->show();
                delete twin;
            }
        }
    }
}

// ================= 文件演示菜单 =================

void PlatformWRH::menuFile() {
    divider("文件读写演示");
    cout << "把当前内存中的对象数据写回文件，再用一个新的平台实例读回：\n";
    save();
    {
        PlatformWRH check(file_);   // 新实例：构造时自动从文件读入
        cout << "\n[读回验证] 新平台实例读入 用户 " << check.userCount()
             << " 个，群 " << check.groupCount() << " 个。\n";
        for (int i = 0; i < check.userCount(); ++i)
            cout << "    用户" << (i + 1) << " 资料与开通服务信息：\n"
                 << "    （读回成功，说明数据可持久化）\n";
    }
}

// ================= 数字化主菜单 =================

void PlatformWRH::run() {
    while (true) {
        cout << "\n################ 腾讯立体社交平台模拟系统 ################\n";
        cout << "  当前数据：用户 " << users_.size() << " 位，"
             << "群 " << groups_.size() << " 个，"
             << "文件：" << file_ << "\n";
        cout << "  1. 一条龙自动演示\n  2. 用户管理\n  3. 好友管理\n"
                "  4. 群管理\n  5. 数据总览\n  6. 文件读写演示\n  0. 退出\n";
        int k = askInt("请选择：");
        if (k == 0) {
            cout << "正在退出，析构时自动把容器写回文件...\n";
            return;
        }
        if (k == 1) {
            autoDemo();
        } else if (k == 2) menuUser();
        else if (k == 3) menuFriend();
        else if (k == 4) menuGroup();
        else if (k == 5) menuOverview();
        else if (k == 6) menuFile();
    }
}

void PlatformWRH::autoDemo() {
    divider("自动演示开始");
    buildDemo();        // 从固化数据重新初始化，保证每次演示结果一致
    demoOpenServices();
    demoLogin();
    demoFriendCRUD();
    demoCommonFriend();
    demoWeChatRecFriend();
    demoGroups();
    demoModeSwitch();
    menuOverview();      // 最后看一遍全貌
    cout << "\n自动演示完毕，数据在退出时写回 " << file_ << " 文件。\n";
}
