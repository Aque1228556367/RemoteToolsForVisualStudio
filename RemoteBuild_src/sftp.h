#ifndef SFTP_H
#define SFTP_H

BOOL CreateDirRemote(LIBSSH2_SFTP *pSftp,PCHAR lpszRemoteDir);
BOOL CopyFileToRemote(LIBSSH2_SFTP *pSftp,PCHAR lpszLocalFile,PCHAR lpszRemoteFile);
VOID CopyDirsToRemote(LIBSSH2_SFTP *pSftp,PCHAR lpszLocalDir,PCHAR lpszRemoteDir);

#endif