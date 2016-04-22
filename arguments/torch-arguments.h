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
        typedef std::function<void()> Callback;
        struct Option {
            std::string option; int require; std::string description; Callback callback;
        };
        
        Commander(const std::string &subcommand, int require, const std::string &desc, Callback callback);
        
        Commander& Option(const std::string &option, int require, const std::string &desc, Callback callback);
        
        bool Execute(std::vector<std::string> args);
        
    private:
        struct Option* GetOption(const std::string &argv);
        std::vector<std::string> GetOptionArgs(std::vector<std::string> &args, int index, int reqiure);
        bool BuildOptionArgsMap(std::vector<std::string> &optionArgs);
        void OnHelp();

    public:
        std::string command;
        std::string description;
        Callback    callback;
        int         require;

    private:
        std::vector<struct Option> m_options;
        std::vector<std::string>   m_commandArgs;
        std::unordered_map<std::string, std::vector<std::string>> m_optionArgsMap;
        
    };
    
    
    class Commander;
    class Arguments {
    public:
        typedef std::function<void()> Callback;
        
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
    
}

#endif /* __TORCH__ARGUMENTS__ */
