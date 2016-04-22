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
{
    this->command = subcommand;
    this->require = require;
    this->description = desc;
    this->callback = callback;
}

Commander& Commander::Option(const std::string &option, int require, const std::string &desc, Callback callback)
{
    m_options.push_back((struct Option){option, require, desc, callback});
    return *this;
}

bool Commander::Execute(std::vector<std::string> args)
{
    if (!this->BuildOptionArgsMap(args)) {
        this->OnHelp();
        return false;
    }
    for (auto opt : args) {
        printf("[opt] %s\n", opt.c_str());
    }
    //        for (auto opt : m_optionMap) {
    //
    //        }
    return false;
}

struct Commander::Option* Commander::GetOption(const std::string &argv)
{
    for (int i = 0; i < m_options.size(); i++) {
        if (m_options[i].option == argv) {
            return &m_options[i];
        }
    }
    return nullptr;
}
void dumpArgs(std::vector<std::string> &args) {
    for (auto x : args) {
        printf("[argv] %s\n", x.c_str());
    }
    printf("---\n");
}
void dumpOptionArgs(std::unordered_map<std::string, std::vector<std::string>> optionArgs) {
    for (auto x : optionArgs) {
        std::string args;
        for (auto x : x.second) {
            args += x + " ";
        }
        printf("[option] %s : %s\n", x.first.c_str(), args.c_str());
    }
}
std::vector<std::string> Commander::GetOptionArgs(std::vector<std::string> &args, int index, int require)
{
    index++;
    std::vector<std::string> optionArgs;

    if (index + require > args.size()) {
        return std::move(optionArgs);
    }
    for (int i = index; i < index + require; i++) {
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
    
    for (int i = 0; i < args.size(); i++) {
        std::string argv = args[i];
        struct Option *option = this->GetOption(argv);
        if (!option) {
            continue;
        }
        std::vector<std::string> optionArgs = this->GetOptionArgs(args, i, option->require);
        if (optionArgs.size() != option->require) {
            return false;
        }
        m_optionArgsMap.insert(std::make_pair(argv, optionArgs));
        args.erase(args.begin() + i);
    }
 
    for (auto argv : args) {
        m_commandArgs.push_back(argv);
    }
    args.clear();
    return m_commandArgs.size() >= this->require;
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


