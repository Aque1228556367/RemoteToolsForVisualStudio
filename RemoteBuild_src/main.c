/*
  VS2012 Զ�̱������

  VS2012 SSHԶ�̱���Linux�����Զ�ʶ���Ƿ���UTF8����

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
            if((p[0] >> 7) == 0)            // ���λΪ0, ANSI ���ݵ�.
                dwCharBytes = 1; 
            else if((p[0] >> 5) == 0x06)    // ����5λ���� 110 -> 2�ֽڱ����UTF8�ַ������ֽ�
                dwCharBytes = 2; 
            else if((p[0] >> 4) == 0x0E)    // ����4λ���� 1110 -> 3�ֽڱ����UTF8�ַ������ֽ�
                dwCharBytes = 3; 
            else if((p[0] >> 3) == 0x1E)    // ����3λ���� 11110 -> 4�ֽڱ����UTF8�ַ������ֽ�
                dwCharBytes = 4; 
            else if((p[0] >> 2) == 0x3E)    // ����2λ���� 111110 -> 5�ֽڱ����UTF8�ַ������ֽ�
                dwCharBytes = 5; 
            else if((p[0] >> 1) == 0x7E)    // ����1λ���� 1111110 -> 6�ֽڱ����UTF8�ַ������ֽ�
                dwCharBytes = 6; 
            else
                dwCharBytes = -1;           // �Ƿ�UTF8���ֽ�

            if(dwCharBytes != 1)
            {
                bRet = TRUE;                // �п�����UTF8
                bFirstByte = FALSE;
            }

            if(dwCharBytes == -1)           // �Ƿ�UTF8���ֽ�
            {
                bRet = FALSE;
                break;
            }
        }
        else
        {
            if((p[0] >> 6) == 0x02)         // �����ֽڱ����� 10xxx ��ͷ
            {
                dwCharBytes--;
                if(dwCharBytes == 1) 
                    bFirstByte = TRUE;      // ��һ���ֽ�����һ���ַ��Ŀ�ʼ
            }
            else
            {
                bRet = FALSE;               // �����ֽڲ���Ҫ��
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

        //����Զ�̷�����
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

        //��ʼ��SSH
        printf("Initializing SSH session...\n");
        pSession = libssh2_session_init();
        if(!pSession)
        {
            printf(" failed\n");
            break;
        }
        libssh2_session_set_blocking(pSession,TRUE);

        //����
        nRet = libssh2_session_handshake(pSession,hSocket);
        if(nRet)
        {
            printf(" handshake failed. %d\n",nRet);
            break;
        }

        //��½
        printf("Login...\n");
        nRet = libssh2_userauth_password(pSession,szUsername,szPassword);
        if(nRet)
        {
            printf(" failed. %d\n",nRet);
            break;
        }

        //��ʼ��SFTP
        printf("Initializing SFTP...\n");
        pSftp = libssh2_sftp_init(pSession);
        if(!pSftp)
        {
            printf(" failed\n");
            break;
        }

        //��ʼ��ͨ��
        printf("Open channel...\n");
        pChannel = libssh2_channel_open_session(pSession);
        if(!pChannel)
        {
            printf(" failed\n");
            break;
        }

        //��SHELL
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

    //ִ������
    sprintf(szCmd,"cd %s && %s\n",szRemoteWorkDir,lpszMakeCmd);
    nRet = libssh2_channel_write(pChannel,szCmd,strlen(szCmd));
    strcpy(szCmd,"echo **FINISHED** >&2\n");
    nRet = libssh2_channel_write(pChannel,szCmd,strlen(szCmd));

    pszLineBuf = szLineBuf;
    dwLineBufLen = 0;

    //����Ϊ�첽ģʽ
    libssh2_session_set_blocking(pSession,FALSE);

    //�첽����ѭ��
    while(1)
    {
        BOOL bEnd = FALSE;

        //����STDERRѭ��
        while(1)
        {
            nRet = libssh2_channel_read_stderr(pChannel,pszLineBuf,1);
            if(nRet > 0)
            {
                if(*pszLineBuf == '\n')
                {
                    PCHAR pGccFltBuf;

                    //�յ���һ��
                    szLineBuf[dwLineBufLen + 1] = 0;

                    if(IsProbablyUTF8(szLineBuf))
                    {
                        ToAnsi(szBuf,512,szLineBuf);
                    }
                    else
                    {
                        strcpy(szBuf,szLineBuf);
                    }

                    //Gcc��ʽ�任
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

                    //�յ������У���������ѭ��
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

                    //�������˵���û�յ���β
                    szLineBuf[dwLineBufLen + 1] = 0;

                    if(IsProbablyUTF8(szLineBuf))
                    {
                        ToAnsi(szBuf,512,szLineBuf);
                    }
                    else
                    {
                        strcpy(szBuf,szLineBuf);
                    }

                    //Gcc��ʽ�任
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
                    //������ܻ��м�������
                    pszLineBuf++;
                    dwLineBufLen++;
                }
                continue;
            }
            break;
        }

        if(bEnd)
            break;

        //����STDOUTѭ��
        while(1)
        {
            nRet = libssh2_channel_read(pChannel,pszLineBuf,1);
            if(nRet > 0)
            {
                if(*pszLineBuf == '\n')
                {
                    PCHAR pGccFltBuf;

                    //�յ���һ��
                    szLineBuf[dwLineBufLen + 1] = 0;
                    
                    if(IsProbablyUTF8(szLineBuf))
                    {
                        ToAnsi(szBuf,512,szLineBuf);
                    }
                    else
                    {
                        strcpy(szBuf,szLineBuf);
                    }

                    //Gcc��ʽ�任
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

                    //�������˵���û�յ���β
                    szLineBuf[dwLineBufLen + 1] = 0;
                    
                    if(IsProbablyUTF8(szLineBuf))
                    {
                        ToAnsi(szBuf,512,szLineBuf);
                    }
                    else
                    {
                        strcpy(szBuf,szLineBuf);
                    }

                    //Gcc��ʽ�任
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
                    //������ܻ��м�������
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

    //����������
    if(!ReadBuildSetting())
    {
        printf("Read setting failed\n");
        return 0;
    }
    printf("Remote:         %s:%d\n",szRemoteIp,dwRemotePort);
    printf("LocalWorkDir:   %s\n",szLocalWorkDir);
    printf("RemoteWorkDir:  %s\n",szRemoteWorkDir);

    //����Զ�̷�����
    if(!ConnectToRemote())
    {
        return 0;
    }

    //���ƹ����ļ�
    CopyDirsToRemote(pSftp,szLocalWorkDir,szRemoteWorkDir);

    //Զ�̱���
    RemoteCompile();

    //�ر�����
    CloseRemote();
}