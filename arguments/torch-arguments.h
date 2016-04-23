//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2015年 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#ifndef __TORCH__ARGUMENTS__
#define __TORCH__ARGUMENTS__

#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace torch {
    
    class Arguments;
    class Commander {
    public:
        typedef std::function<bool(Commander &command, std::vector<std::string> args)> Callback;
        struct Option {
            std::string option; int require; std::string description; Callback callback;
        };
        
        Commander(const std::string &subcommand, int require, const std::string &desc, Callback callback);
        
        /*
         * 注册一个可选Option
         */
        Commander& Option(const std::string &option, int require, const std::string &desc, Callback callback = nullptr);
        
        /*
         * 获得一个Option的参数
         * 注意：若参数不存在也会返回空数组，可能会与Option不需要参数混淆，所以请使用HadOption()接口进行判断。
         */
        std::vector<std::string> GetOptionArgs(const std::string &option);
        
        /*
         * 判断一个Option是否存在
         */
        bool HadOption(const std::string &option);
        
        /*
         * 生成帮助文档
         */
        std::string BuildHelpDocument(const std::string &desc);

        /*
         * 运行一个命令
         * 注意：不可手动调用，由Arguments类内部处理调用
         */
        bool Execute(std::vector<std::string> args);
        
    private:
        std::vector<std::string> CutOptionArgs(std::vector<std::string> &args, int index, int reqiure);
        bool BuildArgs(std::vector<std::string> &optionArgs);
        struct Option* GetOption(const std::string &opt);
        
        int GetOptionArgsNumberBeforeNextOption(const std::vector<std::string> &args, int index);
        void ClearArgsToCurrnetCommand(std::vector<std::string> &args);
        
        void OnHelp();

    public:
        std::string command;
        std::string description;
        Callback    callback;
        const int   require;

    private:
        std::vector<struct Option> m_optionRegistry;
        std::vector<std::string>   m_commandArgs;
        std::unordered_map<std::string, std::vector<std::string>> m_optionArgsMap;
        
    };
    
    
    class Arguments {
    public:
        typedef std::function<bool(Commander &command, std::vector<std::string> args)> Callback;
        
        Arguments();
        ~Arguments();
        
        Arguments& Version(const std::string &version);
        Arguments& Usage(const std::string &usage);
        Arguments& Option(const std::string &option, int require, const std::string &desc, Callback callback);
        
        // command 互斥存在，同时只能存在一个cmd，并且若存在command
        /*
         * 注册一个子命令
         * 参数：
         *  - subcommand: 子命令名称
         *  - require: 子命令的参数个数要求，require<0代表不定参数
         *  - desc: 命令的文字描述
         *  - callback: 命令处理回调，当Commander中所有Option处理完毕后才会调用，可以传null
         * 注意：subcommand互斥存在，一次命令中只能运行一个subcommand
         */
        Commander& Command(const std::string &subcommand, int require, const std::string &desc, Callback callback);
        bool Parse(int argc, const char * argv[]);
        
    private:
        void BuildArgs(int argc, const char * argv[]);
        
        Commander* GetCommand();
        void OnHelp();
        
    private:
        std::string              m_version;
        std::string              m_usage;
        std::string              m_application; // 程序本身的路径
        Commander               *m_defcommand;
        std::vector<Commander*>  m_subcommands;
        std::vector<std::string> m_systemArgs;
    };
    
    static std::string StringVectorToString(const std::vector<std::string> &args) {
        std::string ret = "[";
        for (auto x : args) {
            ret += x + ", ";
        }
        return ret + "]";
    }
    static std::string StringVectorMapToString(std::unordered_map<std::string, std::vector<std::string>> optionArgs) {
        std::string str = "{";
        for (auto x : optionArgs) {
            str += "\"" + x.first + "\":" + StringVectorToString(x.second) + ",\n";
        }
        return str + "}";
    }

    static void dumpArgs(std::vector<std::string> &args, std::string label="") {
        printf("%s#%s\n", label.c_str(), StringVectorToString(args).c_str());
    }
    
    static void dumpOptionArgs(std::unordered_map<std::string, std::vector<std::string>> optionArgs, std::string label="") {
        printf("%s#%s\n", label.c_str(), StringVectorMapToString(optionArgs).c_str());
    }
}

#endif /* __TORCH__ARGUMENTS__ */
