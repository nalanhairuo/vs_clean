#include <windows.h>
#include <tchar.h>
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
TCHAR* szSuffix[] =
{
    TEXT(".suo"),
    TEXT(".ncb"),
    TEXT(".user"),
    TEXT(".pdb"),
    TEXT(".obj"),
    TEXT("ReadMe.txt"),
    TEXT(".aps"),
    TEXT(".idb"),
    TEXT("Readme.txt"),
    TEXT(".ilk"),
    TEXT(".plg"),
    TEXT(".opt"),
    TEXT(".sbr"),
    TEXT(".pch"),
    TEXT(".sdf"),
    TEXT(".netmodule"),
    TEXT(".db"),
    TEXT(".bsc"),
    TEXT(".old"),
    TEXT(".o"),
    TEXT("PREfast"),
    TEXT("buildchk"),
    TEXT("BuildLog"),
    TEXT("buildfre"),
    TEXT("prefast"),

};

TCHAR* szDir[] =
{
    TEXT("\\.svn"),
    TEXT("\\debug"),
    TEXT("\\Debug"),
    TEXT("\\Release"),
    TEXT("\\release"),
    TEXT("\\ipch"),
    TEXT("\\Bin"),
    TEXT("\\bin"),
    TEXT("\\Obj"),
    TEXT("\\x64"),
    TEXT("objchk"),
    TEXT("objfre"),
};

TCHAR szLongPath[MAX_PATH] = TEXT("");//本程序所在文件夹路径
TCHAR szLongFile[MAX_PATH] = TEXT("");//本程序自身路径

//判断是不是可以删除的后缀名
BOOL CheckSuffix(LPCTSTR lpFilePath)
{
    LPCTSTR tmp = NULL;
    int i;

    for (i = 0; i < (sizeof(szSuffix) / sizeof(TCHAR*)); i++)
    {
        tmp = _tcsstr(lpFilePath, szSuffix[i]);

        if (tmp != NULL)
        {
            return TRUE;
        }
    }

    return FALSE;

}

//判断是不是可以删除的文件夹
//从后面开始判断
BOOL CheckDiretory(LPCTSTR lpDirPath)
{
    LPCTSTR tmp = NULL;
    int i;

    for (i = 0; i < (sizeof(szDir) / sizeof(TCHAR*)); i++)
    {
        tmp = _tcsstr(lpDirPath, szDir[i]);

        if (tmp != NULL)
        {
            return TRUE;
        }
    }

    return FALSE;
}

//删除文件夹和文件
BOOL DelFileToRecycle(LPCTSTR lpSourceFile)
{
    SHFILEOPSTRUCT shFile;
    TCHAR FilepathName[MAX_PATH];
    int len = 0;
    _stprintf_s(FilepathName, MAX_PATH, TEXT("%s"), lpSourceFile);

    len = lstrlen(FilepathName);
    FilepathName[len] = '\0';
    FilepathName[len + 1] = '\0';

    ZeroMemory(&shFile, sizeof(shFile));
    shFile.pFrom = FilepathName;
    shFile.wFunc = FO_DELETE;
    //静默，保留撤销（回收站），取消警告
    shFile.fFlags = FOF_SILENT | FOF_ALLOWUNDO | FOF_NOCONFIRMATION;

    if (SHFileOperation(&shFile) != 0)
    {
        return FALSE;
    }

    return TRUE;
}

//获取本清理程序所在的路径
void GetSelfPath()
{
    TCHAR szbufPath[MAX_PATH] = TEXT("");
    LPCTSTR lpTemp = NULL;
    GetModuleFileName(NULL, szbufPath, MAX_PATH);
    GetLongPathName(szbufPath, szLongPath, MAX_PATH);
    GetLongPathName(szbufPath, szLongFile, MAX_PATH);
    lpTemp = _tcsrchr(szLongPath, '\\');
    *_tcsstr(szLongPath, lpTemp) = 0;
}

//搜索并且删除垃圾文件
void SearchAndDelete(LPTSTR lpCurrentDir)
{
    WIN32_FIND_DATA FindFileData;
    HANDLE hListFile;
    TCHAR szFullPath[MAX_PATH] = TEXT("");
    TCHAR szFilePath[MAX_PATH] = TEXT("");
    BOOL bIsRootPath = FALSE;

    lstrcpy(szFilePath, lpCurrentDir);
    lstrcat(szFilePath, TEXT("\\*"));

    hListFile = FindFirstFile(szFilePath, &FindFileData);

    if (hListFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (lstrcmp(FindFileData.cFileName, TEXT(".")) == 0 || lstrcmp(FindFileData.cFileName, TEXT("..")) == 0)
            {
                continue;
            }

            bIsRootPath = PathIsRoot(lpCurrentDir);

            if (TRUE == bIsRootPath)
            {
                _stprintf_s(szFullPath, MAX_PATH, TEXT("%s%s"), lpCurrentDir, FindFileData.cFileName);
            }

            else
            {
                _stprintf_s(szFullPath, MAX_PATH, TEXT("%s\\%s"), lpCurrentDir, FindFileData.cFileName);
            }

            if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (CheckDiretory(szFullPath))
                {
                    DelFileToRecycle(szFullPath);
                }

                SearchAndDelete(szFullPath);
            }

            else
            {
                //这里删除文件
                if (lstrcmp(szLongFile, szFullPath) != 0)//自身跳过
                {
                    if (CheckSuffix(szFullPath))
                    {
                        DelFileToRecycle(szFullPath);
                    }
                }
            }
        } while (FindNextFile(hListFile, &FindFileData));
    }

    FindClose(hListFile);
}

int main(void)
{
    //printf("Visual Studio垃圾文件清理!");
    GetSelfPath();
    SearchAndDelete(szLongPath);

    return 0;
}
