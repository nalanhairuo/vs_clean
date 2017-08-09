#include <windows.h>
#include <tchar.h>
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

//待处理文件名与扩展名
TCHAR* szSuffix[] =
{
    _T(".suo"),
    _T(".ncb"),
    _T(".user"),
    _T(".pdb"),
    _T(".obj"),
    _T("ReadMe.txt"),
    _T(".aps"),
    _T(".idb"),
    _T("Readme.txt"),
    _T(".ilk"),
    _T(".plg"),
    _T(".opt"),
    _T(".sbr"),
    _T(".pch"),
    _T(".sdf"),
    _T(".netmodule"),
    _T(".db"),
    _T(".bsc"),
    _T(".old"),
    _T(".o"),
    _T("PREfast"),
    _T("buildchk"),
    _T("BuildLog"),
    _T("buildfre"),
    _T("prefast"),

};

//待处理文件夹
TCHAR* szDir[] =
{
    _T("\\.svn"),
    _T("\\debug"),
    _T("\\Debug"),
    _T("\\Release"),
    _T("\\release"),
    _T("\\ipch"),
    _T("\\Bin"),
    _T("\\bin"),
    _T("\\Obj"),
    _T("\\x64"),
    _T("objchk"),
    _T("objfre"),
};

TCHAR szLongPath[MAX_PATH] = _T("");//本程序所在文件夹路径
TCHAR szLongFile[MAX_PATH] = _T("");//本程序自身路径

//判断是不是可以删除的后缀名
bool CheckSuffix(LPCTSTR lpFilePath)
{
    LPCTSTR tmp = NULL;
    int i;

    for (i = 0; i < (sizeof(szSuffix) / sizeof(TCHAR*)); i++)
    {
        tmp = _tcsstr(lpFilePath, szSuffix[i]);

        if (tmp != NULL)
        {
            return true;
        }
    }

    return false;

}

//判断是不是可以删除的文件夹
//从后面开始判断
bool CheckDiretory(LPCTSTR lpDirPath)
{
    LPCTSTR tmp = NULL;
    int i;

    for (i = 0; i < (sizeof(szDir) / sizeof(TCHAR*)); i++)
    {
        tmp = _tcsstr(lpDirPath, szDir[i]);

        if (tmp != NULL)
        {
            return true;
        }
    }

    return false;
}

//删除文件夹和文件
bool DelFileToRecycle(LPCTSTR lpSourceFile)
{
    SHFILEOPSTRUCT shFile;
    TCHAR FilepathName[MAX_PATH];
    int len = 0;
    _stprintf_s(FilepathName, MAX_PATH, _T("%s"), lpSourceFile);

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
        return false;
    }

    return true;
}

//获取本清理程序所在的路径
void GetSelfPath()
{
    TCHAR szbufPath[MAX_PATH] = _T("");
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

    TCHAR szFullPath[MAX_PATH] = _T("");
    TCHAR szFilePath[MAX_PATH] = _T("");
    BOOL bIsRootPath = FALSE;

    lstrcpy(szFilePath, lpCurrentDir);
    lstrcat(szFilePath, _T("\\*"));

    WIN32_FIND_DATA FindFileData;
    HANDLE hListFile = FindFirstFile(szFilePath, &FindFileData);

    if (hListFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (lstrcmp(FindFileData.cFileName, _T(".")) == 0 || lstrcmp(FindFileData.cFileName, _T("..")) == 0)
            {
                continue;
            }

            bIsRootPath = PathIsRoot(lpCurrentDir);

            if (TRUE == bIsRootPath)
            {
                _stprintf_s(szFullPath, MAX_PATH, _T("%s%s"), lpCurrentDir, FindFileData.cFileName);
            }

            else
            {
                _stprintf_s(szFullPath, MAX_PATH, _T("%s\\%s"), lpCurrentDir, FindFileData.cFileName);
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
