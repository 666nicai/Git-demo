# -*- coding: utf-8 -*-
"""
生成课程设计答辩 PPT：模拟即时通信系统（腾讯立体社交平台）
运行： python make_ppt.py
输出： 文档/答辩PPT_魏荣赫_55250124.pptx
依赖： python-pptx
"""
from pathlib import Path

from pptx import Presentation
from pptx.util import Inches, Pt
from pptx.dml.color import RGBColor
from pptx.enum.text import PP_ALIGN, MSO_ANCHOR
from pptx.enum.shapes import MSO_SHAPE, MSO_CONNECTOR
from pptx.oxml.ns import qn

# ---------------- 主题色 ----------------
DARK = RGBColor(0x1F, 0x38, 0x64)     # 深蓝
BLUE = RGBColor(0x2E, 0x74, 0xB5)     # 主题蓝
LIGHT = RGBColor(0xDE, 0xEB, 0xF7)    # 浅蓝底
GREY = RGBColor(0x40, 0x40, 0x40)
WHITE = RGBColor(0xFF, 0xFF, 0xFF)
ORANGE = RGBColor(0xC5, 0x5A, 0x11)

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / "文档" / "答辩PPT_魏荣赫_55250124.pptx"

HEI = '微软雅黑'
MONO = 'Consolas'

prs = Presentation()
prs.slide_width = Inches(13.333)
prs.slide_height = Inches(7.5)
BLANK = prs.slide_layouts[6]


def style(run, size=18, bold=False, color=GREY, font=HEI, ea=HEI):
    """统一设置字体（含中文字体 a:ea）"""
    run.font.size = Pt(size)
    run.font.bold = bold
    run.font.name = font
    run.font.color.rgb = color
    rPr = run._r.get_or_add_rPr()
    for tag in ('a:ea', 'a:cs'):
        el = rPr.find(qn(tag))
        if el is None:
            el = rPr.makeelement(qn(tag), {'typeface': ea})
            rPr.append(el)
        else:
            el.set('typeface', ea)


def textbox(slide, l, t, w, h, lines, size=18, bold=False, color=GREY,
            font=HEI, align=PP_ALIGN.LEFT, space=6):
    """lines: [str] 或 [(text, size, bold, color, indent_level)]"""
    tb = slide.shapes.add_textbox(Inches(l), Inches(t), Inches(w), Inches(h))
    tf = tb.text_frame
    tf.word_wrap = True
    first = True
    for item in lines:
        p = tf.paragraphs[0] if first else tf.add_paragraph()
        first = False
        p.alignment = align
        p.space_after = Pt(space)
        if isinstance(item, str):
            r = p.add_run(); r.text = item
            style(r, size, bold, color, font)
        else:
            txt, sz, bd, col, lvl = (list(item) + [None] * 5)[:5]
            sz = sz or size; bd = bold if bd is None else bd
            col = col or color
            if lvl:
                p.level = lvl
                txt = ('    ' * lvl) + ('· ' if lvl == 1 else '- ') + txt
            r = p.add_run(); r.text = txt
            style(r, sz, bd, col, font)
    return tb


def slide_title(slide, title, sub=None):
    bar = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, 0, 0, prs.slide_width, Inches(0.12))
    bar.fill.solid(); bar.fill.fore_color.rgb = BLUE; bar.line.fill.background()
    textbox(slide, 0.6, 0.30, 12.2, 0.8, [title], size=30, bold=True, color=DARK)
    if sub:
        textbox(slide, 0.62, 1.02, 12.2, 0.45, [sub], size=14, color=BLUE)


def notes(slide, txt):
    slide.notes_slide.notes_text_frame.text = txt


def box(slide, l, t, w, h, text, fill=LIGHT, line=BLUE, size=12, bold=True,
        color=DARK):
    sp = slide.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE,
                                Inches(l), Inches(t), Inches(w), Inches(h))
    sp.fill.solid(); sp.fill.fore_color.rgb = fill
    sp.line.color.rgb = line; sp.line.width = Pt(1.25)
    tf = sp.text_frame; tf.word_wrap = True
    tf.vertical_anchor = MSO_ANCHOR.MIDDLE
    p = tf.paragraphs[0]; p.alignment = PP_ALIGN.CENTER
    r = p.add_run(); r.text = text
    style(r, size, bold, color, font=HEI)
    return sp


def arrow(slide, x1, y1, x2, y2, color=BLUE):
    conn = slide.shapes.add_connector(MSO_CONNECTOR.STRAIGHT,
                                      Inches(x1), Inches(y1), Inches(x2), Inches(y2))
    conn.line.color.rgb = color; conn.line.width = Pt(1.5)
    ln = conn.line._get_or_add_ln()
    tail = ln.makeelement(qn('a:tailEnd'), {'type': 'triangle', 'w': 'med', 'len': 'med'})
    ln.append(tail)
    return conn


def table(slide, l, t, w, h, data, size=13, header_size=14, col_w=None):
    rows, cols = len(data), len(data[0])
    gf = slide.shapes.add_table(rows, cols, Inches(l), Inches(t), Inches(w), Inches(h))
    tbl = gf.table
    if col_w:
        for i, cw in enumerate(col_w):
            tbl.columns[i].width = Inches(cw)
    for r, row in enumerate(data):
        for c, val in enumerate(row):
            cell = tbl.cell(r, c)
            cell.margin_left = Inches(0.08); cell.margin_right = Inches(0.08)
            cell.margin_top = Inches(0.03); cell.margin_bottom = Inches(0.03)
            cell.vertical_anchor = MSO_ANCHOR.MIDDLE
            tf = cell.text_frame; tf.word_wrap = True
            p = tf.paragraphs[0]
            p.alignment = PP_ALIGN.CENTER if (r == 0 or c == 0) else PP_ALIGN.LEFT
            run = p.add_run(); run.text = str(val)
            if r == 0:
                style(run, header_size, True, WHITE)
                cell.fill.solid(); cell.fill.fore_color.rgb = DARK
            else:
                style(run, size, False, GREY)
                cell.fill.solid()
                cell.fill.fore_color.rgb = WHITE if r % 2 else LIGHT
    return tbl


# =====================================================================
# 1 封面
# =====================================================================
s = prs.slides.add_slide(BLANK)
bg = s.shapes.add_shape(MSO_SHAPE.RECTANGLE, 0, 0, prs.slide_width, Inches(2.35))
bg.fill.solid(); bg.fill.fore_color.rgb = DARK; bg.line.fill.background()
textbox(s, 0.9, 0.55, 11.6, 1.0, ['模拟即时通信系统（腾讯立体社交平台）'],
        size=38, bold=True, color=WHITE)
textbox(s, 0.95, 1.55, 11.6, 0.6, ['面向对象程序设计 课程设计 · 答辩汇报'],
        size=18, color=RGBColor(0xBD, 0xD7, 0xEE))
textbox(s, 1.6, 3.0, 10.2, 2.4, [
    ('班　　级：软件一班（552501班）', 20, True, DARK, None),
    ('学　　号：55250124', 20, True, DARK, None),
    ('姓　　名：魏荣赫', 20, True, DARK, None),
    ('开发语言：C++（面向对象 · 五层次实现）', 20, True, DARK, None),
], align=PP_ALIGN.LEFT, space=14)
textbox(s, 1.6, 5.9, 10.4, 0.9,
        ['类名统一带姓名缩写 WRH　|　共 30 个源文件，每类独立成文件'],
        size=14, color=BLUE)
notes(s, '各位老师好，我是软件一班魏荣赫，学号55250124。'
         '我的课程设计题目是《模拟即时通信系统——腾讯立体社交平台》。'
         '下面从需求分析、总体设计、关键技术、运行演示四个方面汇报。')

# =====================================================================
# 2 汇报提纲
# =====================================================================
s = prs.slides.add_slide(BLANK)
slide_title(s, '汇报提纲')
items = [
    ('01　选题背景与目标', True), ('02　需求分析（题目要点）', True),
    ('03　系统总体架构与核心类图', True), ('04　关键技术实现', True),
    ('05　运行演示与结果', True), ('06　问题与解决 · 总结展望', True),
]
textbox(s, 1.4, 1.9, 10.5, 4.5, items, size=24, color=DARK, space=20)
notes(s, '汇报分六部分：背景、需求、设计、关键技术、演示、总结。重点是设计和关键技术。')

# =====================================================================
# 3 选题背景
# =====================================================================
s = prs.slides.add_slide(BLANK)
slide_title(s, '01　选题背景与目标')
textbox(s, 0.8, 1.7, 11.8, 4.8, [
    ('题目', 22, True, BLUE, None),
    ('模拟即时通信系统（腾讯立体社交平台），用面向对象方法模拟 QQ / 微博 / 微信三个微X产品。', 18, False, GREY, None),
    ('核心难点——“同中有异”', 22, True, BLUE, None),
    ('· 号码策略不同：QQ 与微博共享同一个 ID，微信是独立 ID 但可绑定 QQ；', 18, False, GREY, None),
    ('· 群管理模式不同：QQ 群可申请加入、有管理员、可建临时讨论组；微信群只能被邀请、仅群主有特权。', 18, False, GREY, None),
    ('设计目标', 22, True, BLUE, None),
    ('· 用继承与多态把“共性”提取到基类，把“差异”下沉到子类；', 18, False, GREY, None),
    ('· 完整覆盖课程要求的 5 个技术层次：基本功能 → 对象 → 抽象封装 → OO 多态 → 优化提高。', 18, False, GREY, None),
], space=8)
notes(s, '这个题目的难点在于三个微X产品既相似又不同：号码规则不同、群规则不同。'
         '我的思路是——共性抽到基类，差异用多态表达。')

# =====================================================================
# 4 需求分析
# =====================================================================
s = prs.slides.add_slide(BLANK)
slide_title(s, '02　需求分析（题目要点全覆盖）')
data = [
    ['功能模块', '题目要求', '本设计实现'],
    ['用户基本信息', '号码ID、昵称、出生、T龄、所在地、好友/群列表', 'UserWRH 继承 BaseInfoWRH'],
    ['号码策略', 'QQ与微博共享ID；微信独立ID可绑定QQ', '账号继承族 + 多态 svcNo()'],
    ['好友管理', '增/删/改/查；微X间共同好友；跨服务推荐好友', 'FriendBookWRH 容器，按服务各存一本'],
    ['群管理', '1001~1006群号；加入/退出/踢人/查成员', 'GroupWRH 多态族 + 成员容器'],
    ['　└ QQ群', '可申请加入、有管理员、可建临时讨论组', 'QQGroupWRH 覆写策略虚函数'],
    ['　└ 微信群', '只能推荐加入、仅群主特权、无子群', 'WeChatGroupWRH 覆写策略虚函数'],
    ['开通管理', '用户可开通 N 个微X服务', 'UserWRH::openService/closeService'],
    ['登录管理', '一个服务登录，其它自动登录', 'UserWRH::login() 置全部在线'],
]
table(s, 0.6, 1.55, 12.1, 5.3, data, size=13, header_size=14,
      col_w=[1.9, 5.5, 4.7])
notes(s, '这是题目要求与我实现的一一对照表。可以看到，每一条需求都能在代码里指到具体的类和方法。')

# =====================================================================
# 5 总体架构
# =====================================================================
s = prs.slides.add_slide(BLANK)
slide_title(s, '03　系统总体架构（分层设计）')
data = [
    ['层次', '类', '职责'],
    ['控制层', 'PlatformWRH / main', '数字菜单、业务调度、演示、文件读写'],
    ['多态层', 'MicroAccountWRH 家族；GroupWRH 家族', '账号与群的继承体系，运行时多态'],
    ['抽象层', 'BaseInfoWRH（抽象基础信息）', '昵称/出生/申请时间/所在地，被复用'],
    ['容器层', 'FriendBookWRH；GroupMemberBookWRH', '好友集合、群成员集合的统一管理'],
    ['信息层', 'FriendInfoWRH；GroupMemberWRH', '单条好友记录 / 单条成员记录'],
    ['工具层', 'ConstWRH；MyDateWRH', '枚举常量；日期与T龄计算'],
]
table(s, 0.6, 1.6, 12.1, 4.3, data, size=14, header_size=15,
      col_w=[1.5, 4.6, 6.0])
textbox(s, 0.7, 6.15, 12.0, 0.9,
        ['工程组织：每个类独立 .h/.cpp 文件，共 30 个源文件；用前向声明避免循环包含。'],
        size=15, color=ORANGE, bold=True)
notes(s, '架构自上而下分六层：控制层负责菜单和调度，多态层负责“不同微X产品的差异”，'
         '抽象层提取公共信息，容器层管理集合。每层职责单一，便于扩展。')

# =====================================================================
# 6 核心类图
# =====================================================================
s = prs.slides.add_slide(BLANK)
slide_title(s, '03　核心类图（继承 / 组合关系）')
box(s, 0.55, 1.55, 2.3, 0.62, 'BaseInfoWRH\n(抽象基础信息)', size=11)
box(s, 0.55, 2.55, 2.3, 0.62, 'UserWRH\n平台用户', size=11, fill=RGBColor(0xFF,0xF2,0xCC), line=ORANGE)
box(s, 0.55, 3.55, 2.3, 0.62, 'MicroAccountWRH\n(账号抽象)', size=11)
box(s, 0.20, 4.55, 1.05, 0.62, 'QQ账号', size=10)
box(s, 0.20 + 1.15, 4.55, 1.05, 0.62, '微博账号', size=10)
box(s, 0.20 + 2.30, 4.55, 1.05, 0.62, '微信账号', size=10)
arrow(s, 1.70, 2.17, 1.70, 2.53)
arrow(s, 1.70, 3.17, 1.70, 3.53)
arrow(s, 1.35, 4.17, 0.75, 4.53)
arrow(s, 1.70, 4.17, 1.72, 4.53)
arrow(s, 2.05, 4.17, 2.72, 4.53)
box(s, 3.45, 4.55, 1.75, 0.62, 'FriendBookWRH\n好友容器', size=10, fill=RGBColor(0xE2,0xEF,0xDA), line=RGBColor(0x54,0x82,0x35))
arrow(s, 2.85, 3.86, 3.45, 4.70, RGBColor(0x54,0x82,0x35))

box(s, 6.55, 1.90, 2.3, 0.62, 'GroupWRH\n(群抽象·模板方法)', size=11)
box(s, 5.75, 3.00, 1.85, 0.62, 'QQGroupWRH', size=11, fill=RGBColor(0xFF,0xF2,0xCC), line=ORANGE)
box(s, 7.85, 3.00, 1.85, 0.62, 'WeChatGroupWRH', size=11, fill=RGBColor(0xFF,0xF2,0xCC), line=ORANGE)
arrow(s, 6.90, 2.52, 6.70, 2.98)
arrow(s, 8.15, 2.52, 8.65, 2.98)
box(s, 6.55, 4.10, 2.3, 0.62, 'GroupMemberBookWRH\n成员容器', size=10, fill=RGBColor(0xE2,0xEF,0xDA), line=RGBColor(0x54,0x82,0x35))
arrow(s, 7.70, 3.62, 7.70, 4.08, RGBColor(0x54,0x82,0x35))

box(s, 4.85, 5.60, 4.0, 0.75, 'PlatformWRH  平台总控\n（菜单 / 演示 / 文件读写）', size=13, fill=DARK, line=DARK, color=WHITE)
arrow(s, 5.60, 5.58, 3.60, 5.20, BLUE)
arrow(s, 8.10, 5.58, 7.70, 4.74, BLUE)
textbox(s, 0.35, 6.45, 12.6, 0.6,
        ['图例：实线箭头 = 继承(IS-A)　|　绿色 = 组合/拥有(has-a，容器)　|　1 个用户可以开通 N 个服务账号'],
        size=12, color=BLUE)
notes(s, '左边是用户与账号体系：一个用户组合了多个账号，每个账号各自拥有一本好友通讯录。'
         '右边是群体系：群抽象派生两种群，每个群拥有一个成员容器。底部是总控类把两边管起来。')

# =====================================================================
# 7 关键技术①
# =====================================================================
s = prs.slides.add_slide(BLANK)
slide_title(s, '04　关键技术① 抽象与封装（第3层次）')
textbox(s, 0.8, 1.6, 11.9, 5.0, [
    ('① 抽象基础类并被复用（题目明确要求）', 21, True, BLUE, None),
    ('BaseInfoWRH 归纳了“昵称 / 出生时间 / 号码申请时间(T龄) / 所在地”四个公共字段，', 17, False, GREY, None),
    ('UserWRH 直接继承复用，注册与展示代码无需重复实现。', 17, False, GREY, None),
    ('② 封　　装', 21, True, BLUE, None),
    ('数据成员全部 private / protected，外部只能通过公开接口访问；', 17, False, GREY, None),
    ('例如好友的增删改查只能通过 FriendBookWRH 的方法完成，保证数据一致性。', 17, False, GREY, None),
    ('③ 容器类封装集合', 21, True, BLUE, None),
    ('FriendBookWRH 封装“好友集合”，GroupMemberBookWRH 封装“成员集合”，', 17, False, GREY, None),
    ('对外提供 add / remove / find / list / commonWith 等统一操作。', 17, False, GREY, None),
], space=8)
notes(s, '第3层次要求抽象出基础类并被复用，我用 BaseInfoWRH 承担；'
         '同时用两个容器类把“好友集合”和“群成员集合”封装起来，外部只能通过接口操作。')

# =====================================================================
# 8 关键技术②
# =====================================================================
s = prs.slides.add_slide(BLANK)
slide_title(s, '04　关键技术② 继承与多态：微X账号族（第4层次）')
textbox(s, 0.8, 1.55, 6.4, 5.2, [
    ('基类定义“必须有什么”', 20, True, BLUE, None),
    ('抽象类 MicroAccountWRH 声明 4 个纯虚函数：', 16, False, GREY, None),
    ('svcName() / isSharedID() / isWeChat() / svcNo()', 15, False, ORANGE, None),
    ('子类回答“我具体是什么”', 20, True, BLUE, None),
    ('· QQ 账号：号码 = 主号（共享）', 16, False, GREY, None),
    ('· 微博账号：号码 = 主号（共享）', 16, False, GREY, None),
    ('· 微信账号：号码 = 独立微信号，可绑定 QQ', 16, False, GREY, None),
    ('调用方只认基类指针', 20, True, BLUE, None),
    ('开通、登录、展示、好友等代码统一按 MicroAccountWRH* 编写，', 16, False, GREY, None),
    ('运行时自动调用到对应子类的实现。', 16, False, GREY, None),
], space=7)
code = [
    '// 抽象基类：只声明接口',
    'class MicroAccountWRH {',
    'public:',
    '    virtual std::string svcNo() const = 0;',
    '    virtual bool isSharedID() const = 0;',
    '};',
    '',
    '// QQ / 微博：共享同一主号',
    'std::string QQAccountWRH::svcNo() const {',
    '    return owner_->mainNo();',
    '}',
    '// 微信：返回独立微信号',
    'std::string WeChatAccountWRH::svcNo() const {',
    '    return wxNo_;',
    '}',
]
textbox(s, 7.5, 1.65, 5.4, 5.0, code, size=13, font=MONO, color=RGBColor(0x1F,0x38,0x64), space=0)
notes(s, '这是整个设计的核心之一。基类只声明“必须提供哪些信息”，具体号码怎么算交给子类。'
         '所以 QQ 和微博返回同一个主号，微信返回自己的独立号，而调用这些代码的地方完全不用改。')

# =====================================================================
# 9 关键技术③
# =====================================================================
s = prs.slides.add_slide(BLANK)
slide_title(s, '04　关键技术③ 群多态与模板方法（第4层次）')
textbox(s, 0.7, 1.5, 12.2, 0.75,
        ['GroupWRH 固定“申请加入 / 邀请 / 踢人 / 设管理员 / 建子群”的公共流程，'],
        size=17, color=GREY)
textbox(s, 0.7, 1.92, 12.2, 0.75,
        ['把“谁能申请、谁能踢人、有没有管理员、能不能建子群”交给子类虚函数决定。'],
        size=17, color=GREY)
data = [
    ['群管理行为', 'QQ群（QQGroupWRH）', '微信群（WeChatGroupWRH）'],
    ['加入方式', '可以主动申请加入', '不能申请，只能由成员邀请/推荐'],
    ['管理员制度', '有管理员，群主可设/撤', '没有管理员，仅群主为特权账号'],
    ['踢人权限', '群主可踢任何人，管理员可踢普通成员', '仅群主可踢'],
    ['临时讨论组(子群)', '支持创建', '不支持'],
    ['管理模式', '群主为核心的管理员制度', '仅群主特权'],
]
table(s, 0.7, 2.85, 12.0, 3.3, data, size=14, header_size=15,
      col_w=[2.6, 4.7, 4.7])
textbox(s, 0.75, 6.35, 12.0, 0.7,
        ['同一段群管理代码，靠多态自动遵守两套不同规则 —— 这就是“同中有异”的落地方式。'],
        size=15, color=ORANGE, bold=True)
notes(s, '群管理是最能体现多态的地方：程序只写一份“申请、踢人”的流程代码，'
         'QQ群和微信群各自实现权限判断，运行时会自动按对应规则执行。')

# =====================================================================
# 10 关键技术④
# =====================================================================
s = prs.slides.add_slide(BLANK)
slide_title(s, '04　关键技术④ 共享ID 与 独立ID 的设计（难点）')
textbox(s, 0.8, 1.6, 6.2, 4.9, [
    ('问题', 20, True, BLUE, None),
    ('同一个人要同时拥有 QQ、微博、微信三个号码，', 16, False, GREY, None),
    ('但 QQ 与微博要求“同一个号”，微信要求“另一个号”。', 16, False, GREY, None),
    ('做法', 20, True, BLUE, None),
    ('· UserWRH 保存主号 mainNo_（QQ/微博共用）', 16, False, GREY, None),
    ('· UserWRH 组合一个账号列表，开通几个就有几个账号对象', 16, False, GREY, None),
    ('· QQ/微博 的 svcNo() 返回 mainNo_ （共享）', 16, False, GREY, None),
    ('· 微信 的 svcNo() 返回独立 wxNo_，并额外保存绑定号 boundQQ_', 16, False, GREY, None),
    ('效果', 20, True, BLUE, None),
    ('号码能“一分为二”又互相关联，且全部由多态自动区分。', 16, False, GREY, None),
], space=6)
box(s, 7.35, 1.75, 5.3, 1.35,
    'QQ 号码：10001\n微博号码：10001　（同一个主号）', size=15,
    fill=RGBColor(0xE2,0xEF,0xDA), line=RGBColor(0x54,0x82,0x35), color=DARK)
box(s, 7.35, 3.30, 5.3, 1.35,
    '微信号码：wx10001\n（独立 ID，已绑定 QQ 10001）', size=15,
    fill=RGBColor(0xFF,0xF2,0xCC), line=ORANGE, color=DARK)
box(s, 7.35, 4.85, 5.3, 1.0,
    '好友列表按服务各存一本 → 才能求“共同好友”', size=14,
    fill=LIGHT, line=BLUE, color=DARK)
notes(s, '这是本题最容易做错的地方。我的处理是：主号存在用户身上，由 QQ 和微博共享；'
         '微信有自己的独立号，另外记录它绑定了哪个 QQ。演示里可以看到 QQ 号和微博号完全相同。')

# =====================================================================
# 11 关键技术⑤
# =====================================================================
s = prs.slides.add_slide(BLANK)
slide_title(s, '04　关键技术⑤ 文件 I/O 持久化（第5层次）')
textbox(s, 0.8, 1.6, 6.3, 4.9, [
    ('要求：容器实例化时读入、析构时写回', 19, True, BLUE, None),
    ('· 构造函数：先尝试 load()，有文件就恢复数据；', 16, False, GREY, None),
    ('　 没有文件就用代码内固化的演示数据。', 16, False, GREY, None),
    ('· 析构函数：save() 把内存中的全部对象写回文件。', 16, False, GREY, None),
    ('存了什么（文本格式，一行一条记录）', 19, True, BLUE, None),
    ('U = 用户资料　　O = 开通的服务　B = 微信绑定QQ', 15, False, GREY, None),
    ('F = 好友关系　　G = 群　K = 群成员及角色', 15, False, GREY, None),
    ('实测效果', 19, True, BLUE, None),
    ('菜单 6 可现场演示“保存 → 用新实例读回”，', 16, False, GREY, None),
    ('首次自动演示后为 8 个用户、13 个群，读回后完全一致。', 16, False, GREY, None),
], space=6)
code = [
    'PlatformWRH::PlatformWRH(const string& f, bool autoSave)',
    '    : file_(f), tempNo_(2000), autoSave_(autoSave) {',
    '    if (!load()) buildDemo();   // 读入 or 固化数据',
    '}',
    'PlatformWRH::~PlatformWRH() {',
    '    if (autoSave_) save();      // 析构写回文件',
    '    clearAll();',
    '}',
]
textbox(s, 7.4, 2.3, 5.5, 2.4, code, size=14, font=MONO, color=DARK, space=4)
box(s, 7.4, 4.4, 5.4, 1.6,
    'platform_data.txt\n（程序关闭后再打开，数据仍在）', size=15,
    fill=LIGHT, line=BLUE, color=DARK)
notes(s, '第5层次要求文件I/O。我在平台类的构造里读文件、析构里写文件，'
         '并且专门做了菜单项可以现场验证：保存后用新实例读回，数据完全一致。')

# =====================================================================
# 12 五层次对照
# =====================================================================
s = prs.slides.add_slide(BLANK)
slide_title(s, '04　五个技术层次 一一对照')
data = [
    ['层次', '课程要求', '本设计实现'],
    ['1 基本功能', '顺序/分支/循环/函数', '数字主菜单与子菜单，0 键逐级返回上级'],
    ['2 支持对象', '类与对象、构造析构、this', '30 个源文件全部类化；UserWRH 析构释放账号对象'],
    ['3 抽象封装', '抽象基础类被复用、容器', 'BaseInfoWRH 被继承复用；两个容器类封装集合'],
    ['4 OO 多态', '继承、虚函数、多态', '账号族与群族两套继承体系，纯虚函数覆写'],
    ['5 优化提高', '文件I/O、可扩展、模式动态可变',
     '构造读入/析构写回；新增枚举+子类即可扩展；modeTwin 动态切换群模式'],
]
table(s, 0.6, 1.7, 12.1, 3.65, data, size=14, header_size=15,
      col_w=[1.9, 4.2, 6.0])
box(s, 0.6, 6.05, 12.1, 1.0,
    '可扩展性示例：要新增“微唱”产品，只需 ① 加一个枚举 ② 派生一个账号/群子类 ③ 工厂加一个分支，'
    '平台其余代码无需改动。', size=14, fill=RGBColor(0xFF,0xF2,0xCC), line=ORANGE, color=DARK)
notes(s, '这张表把课程要求的五个层次逐条对应到我的实现上，是评分点的直接对照。')

# =====================================================================
# 13 运行演示
# =====================================================================
s = prs.slides.add_slide(BLANK)
slide_title(s, '05　运行演示：数字化主菜单')
code = [
    '################ 腾讯立体社交平台模拟系统 ################',
    '  当前数据：用户 8 位，群 5 个，文件：platform_data.txt',
    '  1. 一条龙自动演示',
    '  2. 用户管理        3. 好友管理',
    '  4. 群管理          5. 数据总览',
    '  6. 文件读写演示    0. 退出',
    '请选择：',
]
textbox(s, 1.2, 1.9, 11.0, 3.0, code, size=16, font=MONO, color=DARK, space=2)
textbox(s, 1.2, 5.1, 11.0, 1.5, [
    ('· 输入 1 可一键自动演示全部功能；', 16, False, GREY, None),
    ('· 任意子菜单输入 0 返回上一级（数字菜单可返回上级）；', 16, False, GREY, None),
    ('· 退出时自动把数据写回文件。', 16, False, GREY, None),
], space=6)
notes(s, '运行界面是数字菜单。演示时选 1 就能自动跑完全部功能；'
         '每个子菜单输 0 都能返回上一级，符合题目要求。')

# =====================================================================
# 14 运行结果
# =====================================================================
s = prs.slides.add_slide(BLANK)
slide_title(s, '05　运行结果实录（关键输出）')
code = [
    '[共享ID验证] QQ号码 = 10007，微博号码 = 10007（相同）',
    '[独立ID验证] 微信号 = wx10007（与主号不同，可绑定QQ）',
    '[绑定] 微信号 wx10007 已绑定QQ 10007',
    '[登录] 小腾 在 [QQ] 登录，已开通的 QQ、微博、微信 全部自动登录。',
    '[共同好友] QQ 与 微博 的共同好友(2人)：阿讯、阿豪',
    '[推荐] 已把 阿豪(wx10004) 添加为微信好友',
    '[入群] 新手上路 申请成功，已加入[C++程序设计学习群]。   ← QQ群可申请',
    '[拒绝] 微信群不支持主动申请加入，只能由微信成员 推荐/邀请 加入。',
    '[踢人] 阿讯 已将 阿豪 移出群[C++程序设计学习群]。       ← 管理员可踢',
    '[提示] 小腾 无权踢出 被邀新人。微信群规则：仅群主…      ← 微信仅群主',
    '[提示] 微信群不支持创建临时讨论组(子群)。',
    '（镜像群成员数 = 5，与源群 5 一致 → 管理模式动态切换，数据不受伤）',
]
textbox(s, 0.6, 1.6, 12.2, 5.2, code, size=13.5, font=MONO,
        color=RGBColor(0x1F,0x38,0x64), space=2)
notes(s, '这些是实际运行输出。可以看到：共享ID、独立ID与绑定、一次登录全部在线、'
         '共同好友、跨服务推荐好友、QQ群与微信群在申请/邀请/踢人/子群上的差异，全部符合题目要求。')

# =====================================================================
# 15 问题与解决
# =====================================================================
s = prs.slides.add_slide(BLANK)
slide_title(s, '06　开发中遇到的问题与解决')
data = [
    ['问题', '解决办法'],
    ['类之间互相引用导致“循环包含”编译失败', '使用前向声明，只在 .cpp 中包含完整头文件'],
    ['T龄要按天数精确计算，跨闰年容易算错', '采用民用日→儒略日换算算法统一比较天数'],
    ['对象用 new 创建，容易内存泄漏', 'UserWRH 析构释放账号对象；PlatformWRH 析构释放全部用户与群'],
    ['微信群申请入群被拒后如何保证一致性', '由基类模板方法统一校验：未开通服务 / 已是成员 / 权限不足'],
    ['双击 exe 一闪而过无法运行', '原因是缺少 MinGW 运行库 DLL，改用 -static 静态链接编译'],
]
table(s, 0.6, 1.7, 12.1, 4.2, data, size=14, header_size=15, col_w=[5.6, 6.5])
notes(s, '开发中印象最深的两个问题：一是循环包含，用前向声明解决；'
         '二是编译出的 exe 双击闪退，查出来是动态库缺失，改成静态链接就好了。')

# =====================================================================
# 16 总结
# =====================================================================
s = prs.slides.add_slide(BLANK)
bar = s.shapes.add_shape(MSO_SHAPE.RECTANGLE, 0, 0, prs.slide_width, Inches(1.5))
bar.fill.solid(); bar.fill.fore_color.rgb = DARK; bar.line.fill.background()
textbox(s, 0.9, 0.42, 11.5, 0.8, ['06　总结与展望'], size=32, bold=True, color=WHITE)
textbox(s, 0.9, 2.0, 11.6, 4.0, [
    ('完成情况', 21, True, BLUE, None),
    ('题目要求全部实现，并覆盖 5 个技术层次；30 个源文件，编译零警告，可完整运行演示。', 17, False, GREY, None),
    ('收获', 21, True, BLUE, None),
    ('真正把“抽象、封装、继承、多态”用在一个完整系统里，体会到“共性上提、差异下沉”的设计方法。', 17, False, GREY, None),
    ('展望', 21, True, BLUE, None),
    ('可扩展方向：新增更多微X产品（只需加枚举与子类）、增加聊天消息记录、加入图形界面。', 17, False, GREY, None),
], space=10)
box(s, 3.6, 6.05, 6.1, 0.85, '谢谢老师！请老师批评指正', size=19,
    fill=DARK, line=DARK, color=WHITE)
notes(s, '最后总结：这次课程设计让我把面向对象的四个特性真正落地到一个完整系统里，'
         '也学会了用分层和接口来组织代码。汇报完毕，谢谢老师。')

prs.save(str(OUT))
print('已生成 PPT，共 %d 页' % len(prs.slides._sldIdLst))
