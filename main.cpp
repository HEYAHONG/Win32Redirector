
#include <string>
#include <iostream>
#include <vector>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "process.h"
#include "windows.h"
#include "unistd.h"

std::string GetAppFileName()
{
    char path[MAX_PATH]= {0};
    GetModuleFileNameA(NULL,path,MAX_PATH);
    if(strlen(path)>0)
    {
        size_t len=strlen(path);
        for(size_t i=0; i<len; i++)
        {
            if(path[len-i-1]=='\\')
            {
                return std::string(&path[len-i]);
            }
        }
    }
    return std::string();
}

std::string GetAppDirName()
{
    char path[MAX_PATH]= {0};
    GetModuleFileNameA(NULL,path,MAX_PATH);
    if(strlen(path)>0)
    {
        size_t len=strlen(path);
        for(size_t i=0; i<len; i++)
        {
            if(path[len-i-1]=='\\')
            {
                return std::string(path).substr(0,len-i);
            }
        }
    }
    return std::string();
}


std::vector<std::string> TargetSearchPath=
{
    //父目录（指重定向程序父目录）
    "..\\msys64\\usr\\bin",
    "..\\msys64\\mingw32\\bin",
    "..\\msys64\\mingw64\\bin",
    "..\\cygwin64\\bin",
    "..\\msys32\\usr\\bin",
    "..\\msys32\\mingw32\\bin",
    "..\\msys32\\mingw64\\bin",

    //当前目录（指重定向程序所在目录）
    ".\\msys64\\usr\\bin",
    ".\\msys64\\mingw32\\bin",
    ".\\msys64\\mingw64\\bin",
    ".\\cygwin64\\bin",
    ".\\msys32\\usr\\bin",
    ".\\msys32\\mingw32\\bin",
    ".\\msys32\\mingw64\\bin",

    //各个盘符的根目录
    "\\msys64\\usr\\bin",
    "\\msys64\\mingw32\\bin",
    "\\msys64\\mingw64\\bin",
    "\\cygwin64\\bin",
    "\\msys32\\usr\\bin",
    "\\msys32\\mingw32\\bin",
    "\\msys32\\mingw64\\bin",
};

void InitTargetSearchPath()
{
    const char *Win32RedirectorSearchPath=getenv("WIN32REDIRECTOR_SEARCH_PATH");
    if(Win32RedirectorSearchPath != NULL)
    {
        std::string path_list(Win32RedirectorSearchPath);
        while(std::string::npos!=path_list.find(';'))
        {
            std::string::size_type pos=path_list.find(';');
            TargetSearchPath.insert(TargetSearchPath.begin(),path_list.substr(0,pos));
            path_list=path_list.substr(pos+1);
        }
        if(!path_list.empty())
        {
            TargetSearchPath.insert(TargetSearchPath.begin(),path_list);
        }
    }
}

std::string SearchTarget(std::string AppName,bool IsReturnDir=false)
{
    for(std::vector<std::string>::iterator it=TargetSearchPath.begin(); it!=TargetSearchPath.end(); it++)
    {
        std::string path=(*it);
        //添加末尾的反斜杠(\)
        if(path.c_str()[path.length()-1]!='\\')
        {
            path+="\\";
        }
        if(path.c_str()[0]== '.')
        {
            //当以.开头的默认为当前目录或者父目录
            std::string appdir=GetAppDirName();
            if(!appdir.empty())
            {
                std::string realpath=appdir+path+AppName;
                if(access(realpath.c_str(),0)==0)
                {
                    if(IsReturnDir)
                    {
                        return (appdir+path);
                    }
                    else
                    {
                        return realpath;
                    }
                }
            }

        }
        else if(path.c_str()[1]!=':')
        {
            //不是带盘符的路径,添加盘符搜索
            for(char i='A'; i<='Z'; i++)
            {
                char driver[3]= {i,':',0};
                std::string realpath=std::string(driver)+path+AppName;
                if(access(realpath.c_str(),0)==0)
                {
                    if(IsReturnDir)
                    {
                        return (std::string(driver)+path);
                    }
                    else
                    {
                        return realpath;
                    }
                }
            }
        }
        else
        {
            std::string realpath=path+AppName;
            if(access(realpath.c_str(),0)==0)
            {
                if(IsReturnDir)
                {
                    return path;
                }
                else
                {
                    return realpath;
                }
            }
        }
    }

    return std::string();
}

std::string SearchTargetPath(std::string AppName=GetAppFileName())
{
    return SearchTarget(AppName,false);
}

std::string SearchTargetDirPath(std::string AppName=GetAppFileName())
{
    return SearchTarget(AppName,true);
}

std::string StripFileNameExt(std::string FileName)
{
    std::string FileNameNoExt;
    //去掉文件名中的扩展名
    for(size_t i=0; i<FileName.length(); i++)
    {
        if(FileName.c_str()[FileName.length()-i-1]=='.')
        {
            FileNameNoExt=FileName.substr(0,FileName.length()-i-1);
        }
    }

    if(FileNameNoExt.empty())
    {
        FileNameNoExt=FileName;
    }

    return FileNameNoExt;
}

std::string StringReplace(std::string str, const std::string to_replaced, const std::string newchars)
{
    for(std::string::size_type pos(0); pos != std::string::npos; pos += newchars.length())
    {
        pos = str.find(to_replaced,pos);
        if(pos!=std::string::npos)
            str.replace(pos,to_replaced.length(),newchars);
        else
            break;
    }
    return  str;
}

int main(int argc,char* const argv[], char* const envp[])
{
    InitTargetSearchPath();
    std::string TargetPath=SearchTargetPath();
    std::string TargetDirPath=SearchTargetDirPath();
    if(TargetPath.empty() || TargetDirPath.empty())
    {
        //未找到Exe
        {
            //尝试寻找脚本文件（注意:不支持带后缀的脚本）
            std::string BashPath=SearchTargetPath("bash.exe");
            std::string BashDirPath=SearchTargetDirPath("bash.exe");
            if(!BashPath.empty() && !BashDirPath.empty())
            {
                //设置PATH
                const char *path_c_str=getenv("PATH");
                if(path_c_str!=NULL)
                {
                    putenv((char *)(std::string("PATH=")+BashDirPath+";"+std::string(path_c_str)).c_str());
                }
                std::string ScriptName=StripFileNameExt(GetAppFileName());
                if(!ScriptName.empty())
                {
                    std::string ScriptPath=SearchTargetPath(ScriptName);
                    if(!ScriptPath.empty())
                    {
                        ScriptPath=StringReplace(ScriptPath,"\\","/");
                        char * new_argv[argc+2]= {0};
                        //使得argv[0]为Bash路径
                        new_argv[0]=(char *)BashPath.c_str();
                        //使得argv[1]为脚本路径
                        new_argv[1]=(char *)ScriptPath.c_str();
                        for(int i=1; i<argc; i++)
                        {
                            new_argv[i+1]=argv[i];
                        }
                        return spawnv(_P_WAIT,BashPath.c_str(),new_argv);
                    }


                }
            }
        }
        return -1;
    }
    else
    {
        //生成新的argv
        char * new_argv[argc+1]= {0};
        {
            //使得argv[0]为目标自身路径
            new_argv[0]=(char *)TargetPath.c_str();
            //复制原有的数据
            for(int i=1; i<argc; i++)
            {
                new_argv[i]=argv[i];
            }
        }

        //设置PATH
        const char *path_c_str=getenv("PATH");
        if(path_c_str!=NULL)
        {
            putenv((char *)(std::string("PATH=")+TargetDirPath+";"+std::string(path_c_str)).c_str());
        }


        return spawnv(_P_WAIT,TargetPath.c_str(),new_argv);
    }
}
