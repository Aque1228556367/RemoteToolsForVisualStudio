/*
  GccFilter - ��GCC���ת����Visual Studio����ʶ���

  mengxp
  2013/08/16
*/

#include <windows.h>
#include <stdio.h>
#include "express.h"

#pragma comment(lib,"express.lib")

#define FORMAT_HEAP_ALLOC_SIZE      4096    //�����Ķѳ���
HANDLE hHeapFormat;

CHAR szExpRowCol[] = "^{?+[:]}:{[0-9]+}:{[0-9]+}:{?+<#0A>}";    //���ʽ: �ļ���:�к�:�к�:������Ϣ
CHAR szExpRow[] = "^{?+[:]}:{[0-9]+}:{?+<#0A>}";                //���ʽ: �ļ���:�к�:������Ϣ

//��ѻ���д�ַ������岻����Ӵ����롣
BOOL WriteCharToBuf(PCHAR *ppBuf,ULONG *pBufLen,PCHAR *ppCurrPtr,ULONG *pBufLenRemain,CHAR c)
{
    PCHAR pNewBuf;

    if(*pBufLenRemain == 1)
    {
        //�ռ䲻�㣬ReAlloc
        pNewBuf = HeapReAlloc(hHeapFormat,HEAP_NO_SERIALIZE | HEAP_ZERO_MEMORY,*ppBuf,*pBufLen + FORMAT_HEAP_ALLOC_SIZE);
        if(!pNewBuf)
            return FALSE;

        //����ָ�롢���ȡ�������
        *ppCurrPtr = *ppCurrPtr - *ppBuf + pNewBuf;
        *pBufLenRemain += FORMAT_HEAP_ALLOC_SIZE;
        *ppBuf = pNewBuf;
        *pBufLen += FORMAT_HEAP_ALLOC_SIZE;
    }

    //д�ַ�
    *((*ppCurrPtr)++) = c;
    (*pBufLenRemain)--;
    return TRUE;
}

//��ʽ��������������10������
PCHAR FormatVSOutput(PCHAR lpszFormat,PRET_OFFSET pRetOff,PCHAR lpszLocalWorkDir,PCHAR lpszRemoteWorkDir)
{
    PCHAR pBuf;
    ULONG dwBufLen = FORMAT_HEAP_ALLOC_SIZE;
    PCHAR pCurrPtr;
    ULONG dwBufLenRemain = FORMAT_HEAP_ALLOC_SIZE;

    //������
    if(!hHeapFormat)
    {
        hHeapFormat = HeapCreate(HEAP_NO_SERIALIZE,dwBufLen,0);
        if(!hHeapFormat)
            return NULL;
    }

    //�����ڴ�
    pBuf = HeapAlloc(hHeapFormat,HEAP_NO_SERIALIZE | HEAP_ZERO_MEMORY,dwBufLen);
    if(!pBuf)
        return NULL;
    pCurrPtr = pBuf;

    //������ʽ���ַ���
    while(*lpszFormat)
    {
        if( lpszFormat[0] == '{' &&
            lpszFormat[1] >= '0' &&
            lpszFormat[1] <= '9' &&
            lpszFormat[2] == '}')
        {
            ULONG dwId = lpszFormat[1] - '0';               //ֻ֧��0-9����10������
            PCHAR lpszStart = pRetOff[dwId].lpszStart;      //����ID

            //���Ʋ��ҽ��
            if(lpszStart)
            {
                if(dwId == 1)       //���ڸ�ʽ��·�������ƹ���Ŀ¼
                {
                    PCHAR lpszDir = lpszLocalWorkDir;

                    //���Ʊ���Ŀ¼
                    while(*lpszDir)
                    {
                        if(!WriteCharToBuf(&pBuf,&dwBufLen,&pCurrPtr,&dwBufLenRemain,*(lpszDir++)))
                        {
                            HeapFree(hHeapFormat,HEAP_NO_SERIALIZE,pBuf);
                            return NULL;
                        }
                    }

                    if(*lpszStart != '/')   //Gcc���ص������·��
                    {
                        //׷��һ�� "\"
                        if(!WriteCharToBuf(&pBuf,&dwBufLen,&pCurrPtr,&dwBufLenRemain,'\\'))
                        {
                            HeapFree(hHeapFormat,HEAP_NO_SERIALIZE,pBuf);
                            return NULL;
                        }
                    }
                    else                    //Gcc���ص��Ǿ���·��
                    {
                        if(!strncmp(lpszStart,lpszRemoteWorkDir,strlen(lpszRemoteWorkDir)))
                            lpszStart += strlen(lpszRemoteWorkDir);
                    }
                }

                while(lpszStart < pRetOff[dwId].lpszEnd)
                {
                    CHAR c = *(lpszStart++);

                    if(dwId == 1)   //·������/ת��
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
            //�Ǹ�ʽ����Ϣ��ֱ�Ӹ���
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

//���ػ�������ҪFormatFree
PCHAR GccFilter(PCHAR lpszLine,PCHAR lpszLocalWorkDir,PCHAR lpszRemoteWorkDir)
{
    PCHAR lpszStart = lpszLine;                     //Դָ��
    PCHAR lpszEnd = lpszLine + strlen(lpszLine);    //Դ����ָ��
    RET_OFFSET RetOff[10];                      //���ҽ������
    DWORD dwRetCount;                           //���ҽ�����鳤��
    int iRet;

    memset(RetOff,0,sizeof(RetOff));

    //���� �ļ���:�к�:�к�: xxx
    dwRetCount = 10;
    iRet = ExpressSearch(NULL,lpszStart,lpszEnd,szExpRowCol,RetOff,&dwRetCount,EF_USEEXPRESS);
    if(iRet > 0)
    {
        PCHAR pBuffer = FormatVSOutput("{1}({2},{3}):{4}",RetOff,lpszLocalWorkDir,lpszRemoteWorkDir);   //��ʽ��ΪVisual Studio����ʶ��� �ļ���(�к�,�к�):������Ϣ

        if(pBuffer)
            return pBuffer;
    }
    else
    {
        //û�ҵ�������Ѱ�� �ļ���:�к�: xxx
        dwRetCount = 10;
        iRet = ExpressSearch(NULL,lpszStart,lpszEnd,szExpRow,RetOff,&dwRetCount,EF_USEEXPRESS);
        if(iRet > 0)
        {
            PCHAR pBuffer = FormatVSOutput("{1}({2}):{3}",RetOff,lpszLocalWorkDir,lpszRemoteWorkDir);   //��ʽ��ΪVisual Studio����ʶ��� �ļ���(�к�):������Ϣ

            if(pBuffer)
                return pBuffer;
        }
    }
    return NULL;
}
