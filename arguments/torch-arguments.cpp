//
//  Torch
//
//  Created by Luwei.
//  Copyright (c) 2015年 Luwei. All rights reserved.
//  Github: https://github.com/Luweimy/Torch
//

#include "torch-arguments.h"

using namespace torch;

// Commander

Commander::Commander(const std::string &subcommand, int require, const std::string &desc, Callback callback)
:require(require)
{
    this->command = subcommand;
    this->description = desc;
    this->callback = callback;
}

Commander& Commander::Option(const std::string &option, int require, const std::string &desc, Callback callback)
{
    m_optionRegistry.push_back((struct Option){option, require, desc, callback});
    return *this;
}

std::vector<std::string> Commander::GetOptionArgs(const std::string &option)
{
    auto iterator = m_optionArgsMap.find(option);
    if (iterator == m_optionArgsMap.end()) {
        return std::vector<std::string>();
    }
    return iterator->second;
}

bool Commander::HadOption(const std::string &option)
{
    auto iterator = m_optionArgsMap.find(option);
    if (iterator == m_optionArgsMap.end()) {
        return false;
    }
    return true;
}

std::string Commander::BuildHelpDocument(const std::string &desc)
{
    return this->description;
}

bool Commander::Execute(std::vector<std::string> args)
{
    if (!this->BuildArgs(args)) {
        goto error;
    }
    for (auto x : m_optionArgsMap) {
        struct Option *option = this->GetOptionByName(x.first);
        if (!option || (option->callback && !option->callback(*this, x.second))) {
            goto error;
        }
    }
    if (this->callback && !this->callback(*this, m_commandArgs)) {
        goto error;
    }
    return true;
    
error:
    this->OnHelp();
    return false;
}

std::vector<std::string> Commander::CutArgs(std::vector<std::string> &args, int index, int require)
{
    /* 
     * 从参数列表中截取当前Option/Command的参数，并将其参数全部从参数列表删除
     */
    std::vector<std::string> extractedArgs;
    if (require < 0) {
        require = this->GetOptionArgsNumberBeforeNextOption(args, index);
    }
    if (index + require > args.size()) {
        return std::move(extractedArgs);
    }
    for (int i = index; (i < index + require) && (require-- > 0);) {
        if (this->GetOptionByName(args[i])) {
            return std::move(std::vector<std::string>());
        }
        extractedArgs.push_back(args[i]);
        args.erase(args.begin() + i);
    }
    return std::move(extractedArgs);
}

bool Commander::BuildArgs(std::vector<std::string> &args)
{
    /*
     * 先构建Option参数，然后使用参数列表中剩下的参数构建Command参数
     */
    this->ClearArgsToCurrnetCommand(args);
    dumpArgs(args, "ClearArgsToCurrnetCommand()|After");
    
    for (int i = 0; i < args.size(); i++) { // build option args
        std::string argv = args[i];
        struct Option *option = this->GetOptionByName(argv);
        if (!option) {
            continue;
        }
        args.erase(args.begin() + i);
        std::vector<std::string> optionArgs = this->CutArgs(args, i--, option->require);
        if (option->require > 0 && optionArgs.size() != option->require) {
            return false;
        }
        m_optionArgsMap.insert(std::make_pair(argv, optionArgs));
    }
    dumpOptionArgs(m_optionArgsMap);

    m_commandArgs = this->CutArgs(args, 0, this->require);
    if (this->require > 0) {
        return m_commandArgs.size() == this->require;
    }
    return true;
}

struct Commander::Option* Commander::GetOptionByName(const std::string &name)
{
    for (int i = 0; i < m_optionRegistry.size(); i++) {
        if (m_optionRegistry[i].option == name) {
            return &m_optionRegistry[i];
        }
    }
    return nullptr;
}

int Commander::GetOptionArgsNumberBeforeNextOption(const std::vector<std::string> &args, int index)
{
    /*
     * 获取到下一个Option之前的所有参数的个数，用于不定参数确定要收取的参数个数
     * 参数：
     *  - args: 参数列表，不能包含当前Option
     *  - index: 当前处理的列表项游标index
     */
    int num = 0;
    for (int i = index; i < args.size(); i++) {
        if (this->GetOptionByName(args[i])) {
            break;
        }
        else {
            num++;
        }
    }
    return num;
}

void Commander::ClearArgsToCurrnetCommand(std::vector<std::string> &args)
{
    /*
     * 清除到当前命令的所有参数，用于清理和本Commander无关的参数
     */
    while (args.size() > 0) {
        std::string argv = args.front();
        args.erase(args.begin());
        if (argv == this->command) {
            break;
        }
    }
}

void Commander::OnHelp()
{
    printf("%s\n", this->description.c_str());
}

// Arguments

Arguments::Arguments()
{
//    m_defcommand = new Commander("", -1, "", nullptr);
}

Arguments::~Arguments()
{
    for (auto cmd : m_subcommands) {
        delete cmd;
    }
    if (m_defcommand) {
        delete m_defcommand;
    }
}

Arguments& Arguments::Version(const std::string &version)
{
    m_version = version;
    return *this;
}
Arguments& Arguments::Usage(const std::string &usage)
{
    m_usage = usage;
    return *this;
}

Arguments& Arguments::Option(const std::string &option, int require, const std::string &desc, Callback callback)
{
    m_defcommand->Option(option, require, desc, callback);
    return *this;
}

Commander& Arguments::Command(const std::string &subcommand, int require, const std::string &desc, Callback callback)
{
    Commander *commandObject = new Commander(subcommand, require, desc, callback);
    m_subcommands.push_back(commandObject);
    return *commandObject;
}

bool Arguments::Parse(int argc, const char * argv[])
{
    this->BuildArgs(argc, argv);
    Commander *command = this->GetCommand();
    if (!command) {
        this->OnHelp(); return false;
    }
    return command->Execute(m_systemArgs);
}

void Arguments::BuildArgs(int argc, const char * argv[])
{
    for (int i = 0; i < argc; i++) {
        m_systemArgs.push_back(argv[i]);
    }
    m_systemArgs.erase(m_systemArgs.begin());
    m_application = m_systemArgs.front();
}

Commander* Arguments::GetCommand()
{
    for (auto argv : m_systemArgs) {
        for (auto cmd : m_subcommands) {
            if (cmd->command == argv) {
                return cmd;
            }
        }
    }
    return nullptr;
}

void Arguments::OnHelp()
{
    printf("%s\n", m_usage.c_str());
}


