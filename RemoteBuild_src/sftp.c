/*
  SFTP 封装函数

  所有函数必须工作在阻塞模式

  mengxp works
  2013/08/17
*/

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#include "libssh2.h"
#include "libssh2_config.h"
#include "libssh2_sftp.h"

#pragma comment(lib,"libssh2.lib")
#pragma comment(lib,"ws2_32.lib")


BOOL CreateDirRemote(LIBSSH2_SFTP *pSftp,PCHAR lpszRemoteDir)
{
    int nRet;

    nRet = libssh2_sftp_mkdir(pSftp,lpszRemoteDir,LIBSSH2_SFTP_S_IRWXU | LIBSSH2_SFTP_S_IRGRP | LIBSSH2_SFTP_S_IROTH);
    if(!nRet)
        return TRUE;
    return FALSE;
}

BOOL CopyFileToRemote(LIBSSH2_SFTP *pSftp,PCHAR lpszLocalFile,PCHAR lpszRemoteFile)
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    LIBSSH2_SFTP_HANDLE *hSftp = NULL;
    BOOL bRet = FALSE;
    PCHAR pBuffer = NULL;
    DWORD dwBufLen = 4096;

    do {
        int mode;

        pBuffer = LocalAlloc(LPTR,dwBufLen);
        if(!pBuffer)
            break;

        hFile = CreateFile(lpszLocalFile,GENERIC_READ,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            break;

        mode = LIBSSH2_SFTP_S_IRUSR | LIBSSH2_SFTP_S_IWUSR | LIBSSH2_SFTP_S_IRGRP | LIBSSH2_SFTP_S_IROTH;

        if(!_stricmp(&lpszLocalFile[strlen(lpszLocalFile) - 3],".sh"))
        {
            mode |= LIBSSH2_SFTP_S_IXUSR;
        }

        hSftp = libssh2_sftp_open(
            pSftp,
            lpszRemoteFile,
            LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC,
            mode);

        if(!hSftp)
            break;

        while(1)
        {
            DWORD dwRead;
            SSIZE_T dwWritten;

            if(!ReadFile(hFile,pBuffer,dwBufLen,&dwRead,NULL))
                break;

            if(!dwRead)
                break;

            dwWritten = libssh2_sftp_write(hSftp,pBuffer,dwRead);
            if(dwWritten != dwRead)
            {
                printf("!!*** FUCK THAT Read:%d  Write:%d\n",dwRead,dwWritten);
                break;
            }
        }

        bRet = TRUE;
    } while (FALSE);

    if(pBuffer)
        LocalFree(pBuffer);

    if(hFile != INVALID_HANDLE_VALUE)
        CloseHandle(hFile);

    if(hSftp)
        libssh2_sftp_close(hSftp);

    return bRet;
}

VOID CopyDirsToRemote(LIBSSH2_SFTP *pSftp,PCHAR lpszLocalDir,PCHAR lpszRemoteDir)
{
    WIN32_FIND_DATA wfd;
    HANDLE hFind;
    CHAR szPath[MAX_PATH];       //用来存放 "路径\"
    CHAR szSearch[MAX_PATH];     //用来存放 "路径\*.*"
    CHAR szFound[MAX_PATH];      //用来存放 "路径\找到的文件"

    //初始化搜索路径
    strcpy(szPath,lpszLocalDir);

    //在路径后面加上'\'
    if(szPath[strlen(szPath)-1] != '\\')
        strcat(szPath,"\\");

    //搜索目标\*.*
    strcpy(szSearch,szPath);
    strcat(szSearch,"*.*");

    //创建目录
    CreateDirRemote(pSftp,lpszRemoteDir);
    printf("DIR  %s\n",lpszRemoteDir);

    //寻找文件
    hFind = FindFirstFile(szSearch,&wfd);
    while(hFind != INVALID_HANDLE_VALUE)
    {
        strcpy(szFound,szPath);
        strcat(szFound,wfd.cFileName);
        if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if(wfd.cFileName[0] != '.')
            {
                CHAR szLocalDir[MAX_PATH];
                CHAR szRemoteDir[MAX_PATH];

                strcpy(szLocalDir,szPath);
                strcat(szLocalDir,wfd.cFileName);
                strcpy(szRemoteDir,lpszRemoteDir);
                strcat(szRemoteDir,"/");
                strcat(szRemoteDir,wfd.cFileName);

                //继续搜
                CopyDirsToRemote(pSftp,szLocalDir,szRemoteDir);
            }
        }
        else
        {
            BOOL bRet;
            CHAR szLocalFile[MAX_PATH];
            CHAR szRemoteFile[MAX_PATH];

            strcpy(szLocalFile,szPath);
            strcat(szLocalFile,wfd.cFileName);
            strcpy(szRemoteFile,lpszRemoteDir);
            strcat(szRemoteFile,"/");
            strcat(szRemoteFile,wfd.cFileName);

            if(!_stricmp(&wfd.cFileName[strlen(wfd.cFileName) - 4],".sdf"))
                goto _FindNext;

            if(!_stricmp(&wfd.cFileName[strlen(wfd.cFileName) - 8],".opensdf"))
                goto _FindNext;

            //搜到一个文件，复制文件
            printf("FILE %s",szRemoteFile);
            bRet = CopyFileToRemote(pSftp,szLocalFile,szRemoteFile);
            if(bRet)
                printf(" ..Done!\n");
            else
                printf(" ..Failed!\n");
        }

_FindNext:
        if(!FindNextFile(hFind,&wfd))
            break;
    }
    FindClose(hFind);
}