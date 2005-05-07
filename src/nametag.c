/* -*- mode: c++ -*-
 * $Id: nametag.c,v 1.1 2005/05/07 12:19:50 tfuruka1 Exp $
 * $Name:  $
 *
 * CVSのNameキーワードの置換
 *
 * $Log: nametag.c,v $
 * Revision 1.1  2005/05/07 12:19:50  tfuruka1
 * CVSのNameキーワードを和名に変換する処理を作ってみた。
 *
 */
#include <windows.h>
#include <stdio.h>
#include <mbstring.h>

#define MAX_SIZE 1024

static struct {
    LPCTSTR lpszName;
    LPCTSTR lpszJapaneseName;
} repTable[] = {
    {"KOMA", "好摩(こうま)"},
    {"HIGASHI-OBUKE", "東大更(ひがしおおぶけ)"},
    {"OBUKE", "大更(おおぶけ)"},
    {"TAIRADATE", "平館(たいらだて)"},
    {"KITAMORI", "北森(きたもり)"},
    {"MATSUO-HACHIMANTAI", "松尾八幡平(まつおはちまんたい)"},
    {"APPI-KOGEN", "安比高原(あっぴこうげん)"},
    {"AKASAKATA", "赤坂田(あかさかた)"},
    {"KOYANOHATA", "小屋の畑(こやのはた)"},
    {"ARAYASHIMMACHI", "荒屋新町(あらやしんまち)"},
    {"YOKOMA", "横間(よこま)"},
    {"TAYAMA", "田山(たやま)"},
    {"ANIHATA", "兄畑(あにはた)"},
    {"YUZE-ONSEN", "湯瀬温泉(ゆぜおんせん)"},
    {"HACHIMANTAI", "八幡平(はちまんたい)"},
    {"RIKUCHU-OSATO", "陸中大里(りくちゅうおおさと)"},
    {"KAZUNO-HANAWA", "鹿角花輪(かづのはなわ)"},
    {"SHIBAHARA", "柴平(しばひら)"},
    {"TOWADA-MINAMI", "十和田南(とわだみなみ)"},
    {"SUEHIRO", "末広(すえひろ)"},
    {"DOBUKAI", "土深井(どぶかい)"},
    {"SAWAJIRI", "沢尻(さわじり)"},
    {"JUNISHO", "十二所(じゅうにしょ)"},
    {"OTAKI-ONSEN", "大滝温泉(おおたきおんせん)"},
    {"OGITA", "扇田(おおぎた)"},
    {"HIGASHI-ODATE", "東大館(ひがしおおだて)"},
    {"ODATE", "大館(おおだて)"},
    {NULL, NULL}
};

static LPCTSTR
NewFormat(
    LPCTSTR lpszStr,                            // 元の文字列
    LPCTSTR lpszReplaceStart,                   // 置換え開始位置
    LPCTSTR lpszEndStart,                       // 残りの開始位置
    LPCTSTR lpszReplaceStr                      // 置換え文字列
    )
{
    static TCHAR szReturn[MAX_SIZE];
    size_t len = lpszReplaceStart - lpszStr;

    strncpy(szReturn, lpszStr, MAX_SIZE);
    _snprintf(szReturn + len, MAX_SIZE - len,
              "%s%s", lpszReplaceStr, lpszEndStart);
    return szReturn;
}

/*
 * CVSのNameキーワードを含んだ文字列を和名に変換して返却します。元の文
 * 字列には変更を加えません。
 */
LPCTSTR WINAPI
ReplaceNameTag(LPCTSTR lpszNameTag)
{
    int i;
    TCHAR szFrom[256];
    LPTSTR p;

    for (i = 0; repTable[i].lpszName; i++) {
        _snprintf(szFrom, 255, "$" "Name: %s" " $", repTable[i].lpszName);
        if (p = _mbsstr(lpszNameTag, szFrom)) {
            return NewFormat(lpszNameTag, p, p + strlen(szFrom),
                             repTable[i].lpszJapaneseName);
        }
        if (p = _mbsstr(lpszNameTag, repTable[i].lpszName)) {
            return NewFormat(lpszNameTag, p, p + strlen(repTable[i].lpszName),
                             repTable[i].lpszJapaneseName);
        }
    }
    return lpszNameTag;
}

#if 0            
int main(int argc, char *argv[])
{
    printf("%s\n", ReplaceNameTag(argv[1]));
}
#endif
