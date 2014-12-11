/*
  GccFilter - 将GCC输出转换成Visual Studio可以识别的

  mengxp
  2013/08/16
*/

#include <windows.h>
#include <stdio.h>
#include "express.h"

#pragma comment(lib,"express.lib")

#define FORMAT_HEAP_ALLOC_SIZE      4096    //递增的堆长度
HANDLE hHeapFormat;

CHAR szExpRowCol[] = "^{?+[:]}:{[0-9]+}:{[0-9]+}:{?+<#0A>}";    //表达式: 文件名:行号:列号:错误信息
CHAR szExpRow[] = "^{?+[:]}:{[0-9]+}:{?+<#0A>}";                //表达式: 文件名:行号:错误信息

//向堆缓冲写字符，缓冲不够则加大申请。
BOOL WriteCharToBuf(PCHAR *ppBuf,ULONG *pBufLen,PCHAR *ppCurrPtr,ULONG *pBufLenRemain,CHAR c)
{
    PCHAR pNewBuf;

    if(*pBufLenRemain == 1)
    {
        //空间不足，ReAlloc
        pNewBuf = HeapReAlloc(hHeapFormat,HEAP_NO_SERIALIZE | HEAP_ZERO_MEMORY,*ppBuf,*pBufLen + FORMAT_HEAP_ALLOC_SIZE);
        if(!pNewBuf)
            return FALSE;

        //修正指针、长度、计数器
        *ppCurrPtr = *ppCurrPtr - *ppBuf + pNewBuf;
        *pBufLenRemain += FORMAT_HEAP_ALLOC_SIZE;
        *ppBuf = pNewBuf;
        *pBufLen += FORMAT_HEAP_ALLOC_SIZE;
    }

    //写字符
    *((*ppCurrPtr)++) = c;
    (*pBufLenRemain)--;
    return TRUE;
}

//格式化搜索结果。最多10个参数
PCHAR FormatVSOutput(PCHAR lpszFormat,PRET_OFFSET pRetOff,PCHAR lpszLocalWorkDir,PCHAR lpszRemoteWorkDir)
{
    PCHAR pBuf;
    ULONG dwBufLen = FORMAT_HEAP_ALLOC_SIZE;
    PCHAR pCurrPtr;
    ULONG dwBufLenRemain = FORMAT_HEAP_ALLOC_SIZE;

    //创建堆
    if(!hHeapFormat)
    {
        hHeapFormat = HeapCreate(HEAP_NO_SERIALIZE,dwBufLen,0);
        if(!hHeapFormat)
            return NULL;
    }

    //申请内存
    pBuf = HeapAlloc(hHeapFormat,HEAP_NO_SERIALIZE | HEAP_ZERO_MEMORY,dwBufLen);
    if(!pBuf)
        return NULL;
    pCurrPtr = pBuf;

    //遍历格式化字符串
    while(*lpszFormat)
    {
        if( lpszFormat[0] == '{' &&
            lpszFormat[1] >= '0' &&
            lpszFormat[1] <= '9' &&
            lpszFormat[2] == '}')
        {
            ULONG dwId = lpszFormat[1] - '0';               //只支持0-9，共10个参数
            PCHAR lpszStart = pRetOff[dwId].lpszStart;      //数组ID

            //复制查找结果
            if(lpszStart)
            {
                if(dwId == 1)       //正在格式化路径，复制工作目录
                {
                    PCHAR lpszDir = lpszLocalWorkDir;

                    //复制本地目录
                    while(*lpszDir)
                    {
                        if(!WriteCharToBuf(&pBuf,&dwBufLen,&pCurrPtr,&dwBufLenRemain,*(lpszDir++)))
                        {
                            HeapFree(hHeapFormat,HEAP_NO_SERIALIZE,pBuf);
                            return NULL;
                        }
                    }

                    if(*lpszStart != '/')   //Gcc返回的是相对路径
                    {
                        //追加一个 "\"
                        if(!WriteCharToBuf(&pBuf,&dwBufLen,&pCurrPtr,&dwBufLenRemain,'\\'))
                        {
                            HeapFree(hHeapFormat,HEAP_NO_SERIALIZE,pBuf);
                            return NULL;
                        }
                    }
                    else                    //Gcc返回的是绝对路径
                    {
                        if(!strncmp(lpszStart,lpszRemoteWorkDir,strlen(lpszRemoteWorkDir)))
                            lpszStart += strlen(lpszRemoteWorkDir);
                    }
                }

                while(lpszStart < pRetOff[dwId].lpszEnd)
                {
                    CHAR c = *(lpszStart++);

                    if(dwId == 1)   //路径，将/转换
                        if(c == '/')
                            c = '\\';

                    if(!WriteCharToBuf(&pBuf,&dwBufLen,&pCurrPtr,&dwBufLenRemain,c))
                    {
                        HeapFree(hHeapFormat,HEAP_NO_SERIALIZE,pBuf);
                        return NULL;
                    }
                }
                lpszFormat += 3;
            }
        }
        else
        {
            //非格式化信息，直接复制
            if(!WriteCharToBuf(&pBuf,&dwBufLen,&pCurrPtr,&dwBufLenRemain,*(lpszFormat++)))
            {
                HeapFree(hHeapFormat,HEAP_NO_SERIALIZE,pBuf);
                return NULL;
            }
        }
    }
    return pBuf;
}

VOID FormatFree(PCHAR lpszBuf)
{
    HeapFree(hHeapFormat,HEAP_NO_SERIALIZE,lpszBuf);
}

//返回缓冲区需要FormatFree
PCHAR GccFilter(PCHAR lpszLine,PCHAR lpszLocalWorkDir,PCHAR lpszRemoteWorkDir)
{
    PCHAR lpszStart = lpszLine;                     //源指针
    PCHAR lpszEnd = lpszLine + strlen(lpszLine);    //源结束指针
    RET_OFFSET RetOff[10];                      //查找结果数组
    DWORD dwRetCount;                           //查找结果数组长度
    int iRet;

    memset(RetOff,0,sizeof(RetOff));

    //搜索 文件名:行号:列号: xxx
    dwRetCount = 10;
    iRet = ExpressSearch(NULL,lpszStart,lpszEnd,szExpRowCol,RetOff,&dwRetCount,EF_USEEXPRESS);
    if(iRet > 0)
    {
        PCHAR pBuffer = FormatVSOutput("{1}({2},{3}):{4}",RetOff,lpszLocalWorkDir,lpszRemoteWorkDir);   //格式化为Visual Studio可以识别的 文件名(行号,列号):错误信息

        if(pBuffer)
            return pBuffer;
    }
    else
    {
        //没找到，继续寻找 文件名:行号: xxx
        dwRetCount = 10;
        iRet = ExpressSearch(NULL,lpszStart,lpszEnd,szExpRow,RetOff,&dwRetCount,EF_USEEXPRESS);
        if(iRet > 0)
        {
            PCHAR pBuffer = FormatVSOutput("{1}({2}):{3}",RetOff,lpszLocalWorkDir,lpszRemoteWorkDir);   //格式化为Visual Studio可以识别的 文件名(行号):错误信息

            if(pBuffer)
                return pBuffer;
        }
    }
    return NULL;
}
