
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

std::vector<std::string> TargetSearchPath=
{
    "\\msys64\\usr\\bin",
    "\\msys64\\mingw32\\bin",
    "\\msys64\\mingw64\\bin",
    "\\cygwin64\\bin"
};

std::string SearchTargetPath()
{
    std::string AppName=GetAppFileName();

    for(std::vector<std::string>::iterator it=TargetSearchPath.begin(); it!=TargetSearchPath.end(); it++)
    {
        std::string path=(*it);
        //添加末尾的反斜杠(\)
        if(path.c_str()[path.length()-1]!='\\')
        {
            path+="\\";
        }
        if(path.c_str()[1]!=':')
        {
            //不是带盘符的路径,添加盘符搜索
            for(char i='A'; i<='Z'; i++)
            {
                char driver[3]= {i,':',0};
                std::string realpath=std::string(driver)+path+AppName;
                if(access(realpath.c_str(),0)==0)
                {
                    return realpath;
                }
            }
        }
        else
        {
            std::string realpath=path+AppName;
            if(access(realpath.c_str(),0)==0)
            {
                return realpath;
            }
        }
    }

    return std::string();
}

std::string SearchTargetDirPath()
{
    std::string AppName=GetAppFileName();

    for(std::vector<std::string>::iterator it=TargetSearchPath.begin(); it!=TargetSearchPath.end(); it++)
    {
        std::string path=(*it);
        //添加末尾的反斜杠(\)
        if(path.c_str()[path.length()-1]!='\\')
        {
            path+="\\";
        }
        if(path.c_str()[1]!=':')
        {
            //不是带盘符的路径,添加盘符搜索
            for(char i='A'; i<='Z'; i++)
            {
                char driver[3]= {i,':',0};
                std::string realpath=std::string(driver)+path+AppName;
                if(access(realpath.c_str(),0)==0)
                {
                    return std::string(driver)+path;
                }
            }
        }
        else
        {
            std::string realpath=path+AppName;
            if(access(realpath.c_str(),0)==0)
            {
                return path;
            }
        }
    }

    return std::string();
}

int main(int argc,char* const argv[], char* const envp[])
{
    std::string TargetPath=SearchTargetPath();
    std::string TargetDirPath=SearchTargetDirPath();
    if(TargetPath.empty() || TargetDirPath.empty())
    {
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
