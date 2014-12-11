/*
  VS2012 远程编译程序

  VS2012 SSH远程编译Linux程序，自动识别是否是UTF8编码

  mengxp works
  2013/08/17
*/
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#include "libssh2.h"
#include "libssh2_config.h"
#include "libssh2_sftp.h"

#include "sftp.h"
#include "gccflt.h"

#pragma comment(lib,"libssh2.lib")
#pragma comment(lib,"ws2_32.lib")

CHAR szRemoteIp[50];
ULONG dwRemotePort;
CHAR szLocalWorkDir[128];
CHAR szRemoteWorkDir[128];
CHAR szUsername[64];
CHAR szPassword[64];
PCHAR lpszMakeCmd;


SOCKET hSocket;
LIBSSH2_SESSION *pSession;
LIBSSH2_CHANNEL *pChannel;
LIBSSH2_SFTP *pSftp;
LIBSSH2_SFTP_HANDLE *hSftp;

BOOL IsProbablyUTF8(PCHAR pszSrc)
{
    BOOL bRet = FALSE;
    BOOL bFirstByte = TRUE;
    ULONG dwCharBytes = 1;
    PUCHAR p = (PUCHAR)pszSrc;

    while(*p)
    {
        if(bFirstByte)
        {
            if((p[0] >> 7) == 0)            // 最高位为0, ANSI 兼容的.
                dwCharBytes = 1; 
            else if((p[0] >> 5) == 0x06)    // 右移5位后是 110 -> 2字节编码的UTF8字符的首字节
                dwCharBytes = 2; 
            else if((p[0] >> 4) == 0x0E)    // 右移4位后是 1110 -> 3字节编码的UTF8字符的首字节
                dwCharBytes = 3; 
            else if((p[0] >> 3) == 0x1E)    // 右移3位后是 11110 -> 4字节编码的UTF8字符的首字节
                dwCharBytes = 4; 
            else if((p[0] >> 2) == 0x3E)    // 右移2位后是 111110 -> 5字节编码的UTF8字符的首字节
                dwCharBytes = 5; 
            else if((p[0] >> 1) == 0x7E)    // 右移1位后是 1111110 -> 6字节编码的UTF8字符的首字节
                dwCharBytes = 6; 
            else
                dwCharBytes = -1;           // 非法UTF8首字节

            if(dwCharBytes != 1)
            {
                bRet = TRUE;                // 有可能是UTF8
                bFirstByte = FALSE;
            }

            if(dwCharBytes == -1)           // 非法UTF8首字节
            {
                bRet = FALSE;
                break;
            }
        }
        else
        {
            if((p[0] >> 6) == 0x02)         // 后续字节必须以 10xxx 开头
            {
                dwCharBytes--;
                if(dwCharBytes == 1) 
                    bFirstByte = TRUE;      // 下一个字节是下一个字符的开始
            }
            else
            {
                bRet = FALSE;               // 后续字节不符要求
                break;
            }
        }
        p++;
    }

    if(dwCharBytes != 1)
        bRet = FALSE;

    return bRet;
}

BOOL ReadBuildSetting()
{
	const char* tpPath = ".\\build.ini";
	//const char* tpPath = "build.ini";
    GetCurrentDirectory(sizeof(szLocalWorkDir),szLocalWorkDir);
    if(!GetPrivateProfileString("Setting","RemoteIp","",szRemoteIp,sizeof(szRemoteIp),tpPath))
        return FALSE;
    if((dwRemotePort = GetPrivateProfileInt("Setting","RemotePort",0,tpPath)) == 0)
        return FALSE;
    if(!GetPrivateProfileString("Setting","RemoteWorkDir","",szRemoteWorkDir,sizeof(szRemoteWorkDir),tpPath))
        return FALSE;
    if(!GetPrivateProfileString("Setting","Username","",szUsername,sizeof(szUsername),tpPath))
        return FALSE;
    if(!GetPrivateProfileString("Setting","Password","",szPassword,sizeof(szPassword),tpPath))
        return FALSE;
//     if(!GetPrivateProfileString("Setting","LocalWorkDir","",szLocalWorkDir,sizeof(szLocalWorkDir),".\\build.ini"))
//         return TRUE;
    return TRUE;
}

ULONG ToAnsi(PCHAR pszDst,ULONG dwDstLen,PCHAR pszSrc)
{
    WCHAR szUnicodeBuf[1024];

    if(strlen(pszSrc) > 512)
        return 0;

    MultiByteToWideChar(CP_UTF8,0,pszSrc,-1,szUnicodeBuf,sizeof(szUnicodeBuf) / 2);
    return WideCharToMultiByte(CP_ACP,0,szUnicodeBuf,-1,pszDst,dwDstLen,NULL,FALSE);
}

BOOL ConnectToRemote()
{
    BOOL bRet = FALSE;
    int nRet;

    do {
        WSADATA wsadata;
        SOCKADDR_IN sin;

        WSAStartup(MAKEWORD(2,0), &wsadata);

        nRet = libssh2_init(0);
        if(nRet)
        {
            printf("libssh2_init failed. %d\n",nRet);
            break;
        }

        //连接远程服务器
        printf("Connecting to remote host...\n");
        hSocket = socket(AF_INET,SOCK_STREAM,0);
        memset(&sin,0,sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_port = htons((USHORT)dwRemotePort);
        sin.sin_addr.s_addr = inet_addr(szRemoteIp);
        nRet = connect(hSocket,(struct sockaddr*)&sin,sizeof(sin));
        if(nRet)
        {
            printf(" Connect %s:%d failed. %d\n",szRemoteIp,dwRemotePort,nRet);
            break;
        }

        //初始化SSH
        printf("Initializing SSH session...\n");
        pSession = libssh2_session_init();
        if(!pSession)
        {
            printf(" failed\n");
            break;
        }
        libssh2_session_set_blocking(pSession,TRUE);

        //握手
        nRet = libssh2_session_handshake(pSession,hSocket);
        if(nRet)
        {
            printf(" handshake failed. %d\n",nRet);
            break;
        }

        //登陆
        printf("Login...\n");
        nRet = libssh2_userauth_password(pSession,szUsername,szPassword);
        if(nRet)
        {
            printf(" failed. %d\n",nRet);
            break;
        }

        //初始化SFTP
        printf("Initializing SFTP...\n");
        pSftp = libssh2_sftp_init(pSession);
        if(!pSftp)
        {
            printf(" failed\n");
            break;
        }

        //初始化通道
        printf("Open channel...\n");
        pChannel = libssh2_channel_open_session(pSession);
        if(!pChannel)
        {
            printf(" failed\n");
            break;
        }

        //打开SHELL
        nRet = libssh2_channel_shell(pChannel);
        if(nRet)
        {
            printf(" Shell failed. %d\n",nRet);
            break;
        }

        bRet = TRUE;
    } while(FALSE);

    return bRet;
}

VOID RemoteCompile()
{
    int nRet;
    CHAR szCmd[512];
    CHAR szLineBuf[512];
    PCHAR pszLineBuf;
    ULONG dwLineBufLen = 0;
    CHAR szBuf[512];

    //执行命令
    sprintf(szCmd,"cd %s && %s\n",szRemoteWorkDir,lpszMakeCmd);
    nRet = libssh2_channel_write(pChannel,szCmd,strlen(szCmd));
    strcpy(szCmd,"echo **FINISHED** >&2\n");
    nRet = libssh2_channel_write(pChannel,szCmd,strlen(szCmd));

    pszLineBuf = szLineBuf;
    dwLineBufLen = 0;

    //设置为异步模式
    libssh2_session_set_blocking(pSession,FALSE);

    //异步接收循环
    while(1)
    {
        BOOL bEnd = FALSE;

        //接收STDERR循环
        while(1)
        {
            nRet = libssh2_channel_read_stderr(pChannel,pszLineBuf,1);
            if(nRet > 0)
            {
                if(*pszLineBuf == '\n')
                {
                    PCHAR pGccFltBuf;

                    //收到了一行
                    szLineBuf[dwLineBufLen + 1] = 0;

                    if(IsProbablyUTF8(szLineBuf))
                    {
                        ToAnsi(szBuf,512,szLineBuf);
                    }
                    else
                    {
                        strcpy(szBuf,szLineBuf);
                    }

                    //Gcc格式变换
                    pGccFltBuf = GccFilter(szBuf,szLocalWorkDir,szRemoteWorkDir);
                    if(pGccFltBuf)
                    {
                        printf(pGccFltBuf);
                        FormatFree(pGccFltBuf);
                    }
                    else
                    {
                        printf(szBuf);
                    }

                    //收到结束行，跳出接收循环
                    if(!strcmp(szLineBuf,"**FINISHED**\n"))
                    {
                        bEnd = TRUE;
                        break;
                    }

                    pszLineBuf = szLineBuf;
                    dwLineBufLen = 0;
                }
                else if(dwLineBufLen == sizeof(szLineBuf) - 1)
                {
                    PCHAR pGccFltBuf;

                    //缓冲满了但是没收到行尾
                    szLineBuf[dwLineBufLen + 1] = 0;

                    if(IsProbablyUTF8(szLineBuf))
                    {
                        ToAnsi(szBuf,512,szLineBuf);
                    }
                    else
                    {
                        strcpy(szBuf,szLineBuf);
                    }

                    //Gcc格式变换
                    pGccFltBuf = GccFilter(szBuf,szLocalWorkDir,szRemoteWorkDir);
                    if(pGccFltBuf)
                    {
                        printf(pGccFltBuf);
                        FormatFree(pGccFltBuf);
                    }
                    else
                    {
                        printf(szBuf);
                    }

                    pszLineBuf = szLineBuf;
                    dwLineBufLen = 0;
                }
                else
                {
                    //后面可能还有继续接收
                    pszLineBuf++;
                    dwLineBufLen++;
                }
                continue;
            }
            break;
        }

        if(bEnd)
            break;

        //接收STDOUT循环
        while(1)
        {
            nRet = libssh2_channel_read(pChannel,pszLineBuf,1);
            if(nRet > 0)
            {
                if(*pszLineBuf == '\n')
                {
                    PCHAR pGccFltBuf;

                    //收到了一行
                    szLineBuf[dwLineBufLen + 1] = 0;
                    
                    if(IsProbablyUTF8(szLineBuf))
                    {
                        ToAnsi(szBuf,512,szLineBuf);
                    }
                    else
                    {
                        strcpy(szBuf,szLineBuf);
                    }

                    //Gcc格式变换
                    pGccFltBuf = GccFilter(szBuf,szLocalWorkDir,szRemoteWorkDir);
                    if(pGccFltBuf)
                    {
                        printf(pGccFltBuf);
                        FormatFree(pGccFltBuf);
                    }
                    else
                    {
                        printf(szBuf);
                    }

                    pszLineBuf = szLineBuf;
                    dwLineBufLen = 0;
                }
                else if(dwLineBufLen == sizeof(szLineBuf) - 1)
                {
                    PCHAR pGccFltBuf;

                    //缓冲满了但是没收到行尾
                    szLineBuf[dwLineBufLen + 1] = 0;
                    
                    if(IsProbablyUTF8(szLineBuf))
                    {
                        ToAnsi(szBuf,512,szLineBuf);
                    }
                    else
                    {
                        strcpy(szBuf,szLineBuf);
                    }

                    //Gcc格式变换
                    pGccFltBuf = GccFilter(szBuf,szLocalWorkDir,szRemoteWorkDir);
                    if(pGccFltBuf)
                    {
                        printf(pGccFltBuf);
                        FormatFree(pGccFltBuf);
                    }
                    else
                    {
                        printf(szBuf);
                    }

                    pszLineBuf = szLineBuf;
                    dwLineBufLen = 0;
                }
                else
                {
                    //后面可能还有继续接收
                    pszLineBuf++;
                    dwLineBufLen++;
                }
                continue;
            }
            break;
        }
    }
}

VOID CloseRemote()
{
    if(hSftp)
        libssh2_sftp_close(hSftp);

    if(pSftp)
        libssh2_sftp_shutdown(pSftp);

    if(pChannel)
    {
        libssh2_channel_close(pChannel);
        libssh2_channel_free(pChannel);
    }

    if(pSession)
    {
        libssh2_session_disconnect(pSession,"disconnect");
        libssh2_session_free(pSession);
    }

    if(hSocket)
    {
        closesocket(hSocket);
    }

    libssh2_exit();
}

int main(int argc,char *argv[])
{
    lpszMakeCmd = "make";

    if(argc == 2)
    {
        if(!_stricmp(argv[1],"rebuild"))
        {
            lpszMakeCmd = "make clean && make";
        }
        if(!_stricmp(argv[1],"clean"))
        {
            lpszMakeCmd = "make clean";
        }
    }

    //读编译配置
    if(!ReadBuildSetting())
    {
        printf("Read setting failed\n");
        return 0;
    }
    printf("Remote:         %s:%d\n",szRemoteIp,dwRemotePort);
    printf("LocalWorkDir:   %s\n",szLocalWorkDir);
    printf("RemoteWorkDir:  %s\n",szRemoteWorkDir);

    //连接远程服务器
    if(!ConnectToRemote())
    {
        return 0;
    }

    //复制工程文件
    CopyDirsToRemote(pSftp,szLocalWorkDir,szRemoteWorkDir);

    //远程编译
    RemoteCompile();

    //关闭连接
    CloseRemote();
}