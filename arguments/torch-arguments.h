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

        /*
         * 由Arguments类创建，不要手动创建
         */
        Commander(const std::string &subcommand, int require, const std::string &desc, Callback callback = nullptr);
        
        /*
         * 设置此Command的使用描述，可选，如：Usage("Usage: command [options] <file ...>")
         */
        Commander& Usage(const std::string &usage);

        /*
         * 注册一个可选Option
         * 参数：
         *  - option: option的名字，不可重复
         *  - require: option要求的参数个数，require<0代表不定参数
         *  - desc: option的文字描述
         *  - callback: option处理回调，当Commander中所有option处理完毕后才会调用，可以传null
         * 说明：注册的option是可选的，命令中可出现也可以不出现，若出现则会先于Command回调
         */
        Commander& Option(const std::string &option, int require, const std::string &desc, Callback callback = nullptr);
        
        /*
         * 获得一个Option的输入参数
         * 注意：Option不存在和不需要参数都会返回空数组，所以请使用HadOption()接口进行判断Option是否存在
         */
        std::vector<std::string> GetOptionArgs(const std::string &option);
        
        /*
         * 判断一个Option是否存在
         */
        bool HasOption(const std::string &option);
        
        /*
         * 根据传入的desc和Usage以及所有注册的Option信息生成帮助文档
         */
        std::string BuildHelp();

        /*
         * 运行当前命令，此时会先优先处理所有Option，然后在处理Command本身
         * 返回值：运行是否成功
         * 注意：不可手动调用，由Arguments类内部处理调用
         */
        bool Execute(std::vector<std::string> args);
        
    private:
        std::vector<std::string> CutArgs(std::vector<std::string> &args, int index, int reqiure);
        int GetOptionArgsNumberBeforeNextOption(const std::vector<std::string> &args, int index);
        struct Option* GetOptionByName(const std::string &name);
        bool BuildArgs(std::vector<std::string> &optionArgs);

    public:
        std::string command;
        std::string description;
        std::string usage;
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
        
        /*
         * 构造函数
         * 参数：
         *  - require: MainCommand要求的参数个数要求，require<0代表不定参数
         *  - desc: MainCommand的描述信息，用于生成帮助文档
         *  - callback: MainCommand的回调，当此命令中所有Option处理完毕后才会调用，可以传null
         * 说明：可以通过MainCommand()获得主命令接口
         */
        Arguments(int require, const std::string &desc, Callback callback = nullptr);
        ~Arguments();
        
        /*
         * 返回主命令，可对主命令添加参数预设等操作，详细操作请见Commander
         */
        Commander& MainCommand();
        
        /*
         * 注册一个子命令
         * 参数：
         *  - subcommand: 子命令名称
         *  - require: 子命令的要求的参数个数，require<0代表不定参数
         *  - desc: 命令的文字描述
         *  - callback: 命令处理回调，当Commander中所有Option处理完毕后才会调用，可以传null
         * 注意：subcommand互斥存在，一次命令中只能运行一个subcommand
         */
        Commander& SubCommand(const std::string &subcommand, int require, const std::string &desc, Callback callback);
        
        /*
         * 开始解析命令行参数，请务必传入main函数的参数
         * 参数：
         *  - argc: 命令行参数个数
         *  - argv: 命令行参数数组指针
         * 返回值：返回是否发生错误，若发生错误，会自动触发打印帮助文档
         */
        bool Parse(int argc, const char * argv[]);
        
    private:
        void ClearArgsToSubCommand(std::vector<std::string> &args, const std::string &subcommand);
        void BuildArgs(int argc, const char * argv[]);
        Commander* GetSubCommand();
        void OnHelp(Commander *command);
        
    private:
        std::string              m_application;
        Commander               *m_mainCommand;
        std::vector<Commander*>  m_subcommandRegistry;
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

    static void DumpStringVector(std::vector<std::string> &args, std::string label="") {
        printf("%s#%s\n", label.c_str(), StringVectorToString(args).c_str());
    }
    
    static void DumpStringVectorMap(std::unordered_map<std::string, std::vector<std::string>> optionArgs, std::string label="") {
        printf("%s#%s\n", label.c_str(), StringVectorMapToString(optionArgs).c_str());
    }
}

#endif /* __TORCH__ARGUMENTS__ */
