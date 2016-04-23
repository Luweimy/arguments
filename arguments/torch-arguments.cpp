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

bool Commander::Execute(std::vector<std::string> args)
{
    if (!this->BuildOptionArgsMap(args)) {
        goto error;
    }
    for (auto x : m_optionArgsMap) {
        struct Option *option = this->GetOption(x.first);
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

std::vector<std::string> Commander::CutOptionArgs(std::vector<std::string> &args, int index, int require)
{
    args.erase(args.begin() + index);
    if (index + require > args.size()) {
        return std::move(std::vector<std::string>());
    }

    std::vector<std::string> optionArgs;
    for (int i = index; i < index + require; i++) {
        if (this->GetOption(args[i])) {
            return std::move(std::vector<std::string>());
        }
        optionArgs.push_back(args[i]);
    }
    while (require-- > 0) {
        args.erase(args.begin() + index);
        
    }
    return std::move(optionArgs);
}

bool Commander::BuildOptionArgsMap(std::vector<std::string> &args)
{
    while (args.size() > 0) {
        std::string argv = args.front();
        args.erase(args.begin());
        if (argv == this->command) {
            break;
        }
    }
    dumpArgs(args);
//    dumpOptionArgs(m_optionArgsMap);
    for (int i = 0; i < args.size(); i++) {
        std::string argv = args[i];
        struct Option *option = this->GetOption(argv);
        if (!option) {
            continue;
        }
        std::vector<std::string> optionArgs = this->CutOptionArgs(args, i--, option->require);
        if (optionArgs.size() != option->require) {
            return false;
        }
        m_optionArgsMap.insert(std::make_pair(argv, optionArgs));
    }
    
    int require = this->require;
    while (require-- > 0 && args.size() > 0) {
        m_commandArgs.push_back(args.front());
        args.erase(args.begin());
    }
    
    return m_commandArgs.size() == this->require;
}

struct Commander::Option* Commander::GetOption(const std::string &opt)
{
    for (int i = 0; i < m_optionRegistry.size(); i++) {
        if (m_optionRegistry[i].option == opt) {
            return &m_optionRegistry[i];
        }
    }
    return nullptr;
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


