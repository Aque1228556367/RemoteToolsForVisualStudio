// ;正则表达式 函数库
// ;版权所有(C): AoGo 2005+
// ;http://www.aogosoft.com
// ;非商业软件可直接在代码中使用,商业软件请征得Aogosoft同意,Aogosoft保留追究权力
// ;详细使用方法请参考 MASMPlus 帮助文档中"正则表达式"一节
// ;最后更新 2007年1月7日
// ;=====================================================================================
// ;!()		条件取反,取反条件必须使用表达式包含,如!(\.),不是制表符或空格,!(.)*表示跳过所有非空格与制表符(如果有)
// ; ?		一个任意字符
// ; .		一个空格或制表符
// ; ,		一个字词分隔符(包含.)
// ; %		一个单词字符,字符是否是一个单词以字符表来定义
// ;*<>|[]	0或更多,如果有结束条件,是直到[结束条件],如:?*<=>,直到碰到=号
// ;+<>|[]	1或更多,如果有结束条件,是直到[结束条件],如:?+<=>,直到碰到=号
// 			;<>与[]的区别在于,<>是表示如果匹配结束是<结束条件>起的使用,结束条件也是匹配目标,如:
// 			;在"abcd"中查找"abc*<d>",匹配目标为abcd,如果使用abc*[d],则匹配目标为abc
// 			;*号为可忽略符号,带条件的*表达式可理解为如果没有则忽略继续,如果有则跳到匹配之后继续.
// ; #		转义符,后面跟十六进制数,如#0D表达回车符号,#09表示制表符
// 			;#?表示中文字符,众所周知的原因,向后查找中文字符时请在表达式中加上行首的条件,否则将会有错误
// ;			如向后查找^.*#?+,查找行首开始的第一个中文词组,如果中文词组前有空格制表符跳过,向下查找时没有限制
// ; \		字符,中间的字符是实际符号 如.表示空格制表符\.表示一个"."
// ;[-]		在范围内 [a-b],或者清单,[abcdef],表达式符号仍然使用\来转换,如()/<>/{}/[]
// ;" "		" "内包含的必须完全一样,区分大小写,表达式符号仍然使用\来转换,如()/<>/{}/[]
// ;' '		' '与" " 的区别在于' '不区分大小写,表达式符号仍然使用\来转换,如()/<>/{}/[]
// ;			" "与' '均不会理会是否包含EF_WORDCASE
// ;( )		表达式,可嵌套,嵌套层次数无限制
// ;{ }		标记并返回{}所包含的区域,可用在任何表达式内.
// ; |		或者,支持任意表达式成员组合,如:(.)|(a)|((.*<%>)|(%+))
// ; ^		行首
// ; $		行尾
// ;脚本折分表达式例子:^.*{%+}(.+((\"+<{?+[\"]}">)|({?+[.|($)]})))*[.*$]
// ;如:dir "c:\pro file\1.asm" 4.asm /a /c,表达式查找此行后,返回:
// ;[dir] [c:\pro file\1.asm] [4.asm] [/a] [/c].
// ;("+<{?+["]}">)的意思是如果有引号,把引号开始到另一个引号之间的内容做为匹配条件
// ;所有符号除特别说明均可随意组合.所有符号均支持*与+.均可设置条件.
// ;(*/+)-<>/[]与|的组合能实现很复杂的多条件复合查找.只要是文本,就可以按自己的意思进行查找.

#define EF_DEC                  0x1         //向上查找,默认向下
#define EF_USEEXPRESS           0x2         //使用表达式
#define EF_WORDCASE             0x4         //区分大小写
#define EF_MATCHWORD            0x8         //匹配整个单词,包含EF_USEEXPRESS时此标志无效
#define EF_BEGINNOTLINEBEGIN    0x10        //开始地址不是行首
#define EF_ENDNOTLINEEND        0x20        //结束地址不是行尾
#define EF_ENDISSIZE            0x40        //结束地址是一个长度(对EF_DEC无效)
#define EF_FINDONCE             0x80        //只查找一遍,只对EF_USEEXPRESS有效并且对EF_DEC无效)
#define EF_MODIFLINESTARTEND    0x100       //修正行首与行尾
#define EF_DOUBLEISWORD         0x200       //中文字符也是单词
#define EF_RANGENOTCASE         0x400       //符号[]内的字符总是不大小写
#define EF_RESERVED             0x800       //保留符号,请不要设置,它用于二次查找,由内部自动确认
#define EF_NOTABSMATCH          0x1000      //没有引号处理"/'

typedef struct _RET_OFFSET{
    PCHAR lpszStart;
    PCHAR lpszEnd;
}RET_OFFSET,*PRET_OFFSET;

INT WINAPI ExpressSearch(
    PCHAR lpszCharTab,          //字符表 NULL表示使用内置默认
    PCHAR lpszStart,            //源字符串开始
    PCHAR lpszEnd,              //源字符串结束
    PCHAR lpszExp,              //表达式
    PRET_OFFSET lpRetOffset,    //查找结果
    ULONG *lpdwRetCount,        //查找结果数组长度 IN OUT
    ULONG dwFlags               //查找标志
    );

#define EFERR_NOTCONST              -1  //没有常数定义
#define EFERR_EXPNOTSUPPORT         -2  //不能这样使用符号
#define EFERR_NOTCLOSEBRACKET       -3  //没有关闭括号
#define EFERR_EMPTYEXPRESS          -4  //空的表达式,()/[]/{}/<>
#define EFERR_NOTEXISTEXP           -5  //多余的表达式结束符,请使用\],\),\>等等
#define EFERR_NOTBYONSELF           -6  //表达式符号不能单独使用
#define EFERR_NOTLOOPEND            -7  //对于?*/?+必须设置结束条件,同时,*/+对于?来说是一样的
#define EFERR_DEADLOOP              -8  //在使用*/+的表达式里面的整个表达式均是可忽略的,如(.*)+,因为.*总是成功,后面的+要求成功时继续,这是一个死循环.

extern PCHAR DefCharTable;

#define CF_WORD                 0x1             //单词
#define CF_NUMBER               0x2             //数字
#define CF_HEX                  0x4             //十六进制
#define CF_BIN                  0x8             //二进制
#define CF_UPCHAR               0x10            //大写字符
#define CF_LOWCHAR              0x20            //小写字符
#define CF_COMMENT              0x40            //注释字符
#define CF_COMMENTEND           0x80            //注释第二个字符
#define CF_TAB                  0x100           //语法线对齐符
#define CF_SPACE                0x200           //语法线对齐符
#define CF_ENTER                0x400           //空格
#define CF_WARP                 0x800           //回车
#define CF_MATCH                0x1000          //换行符
#define CF_SELFMATCH            0x2000          //包含符,如单引号
#define CF_INVALID              0x40000000      //无效/无意义字符字符
#define CF_DOUBLEBYTE           0x80000000      //中文字符首
