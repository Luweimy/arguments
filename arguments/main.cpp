//
//  main.cpp
//  TestCpp
//
//  Created by luwei on 15/10/29.
//  Copyright (c) 2015年 luwei. All rights reserved.
//

#include <iostream>
#include <vector>
#include <assert.h>
#include "torch-arguments.h"

int main(int argc, const char * argv[]) {
    
    //    .Version("1.0")
    //    .Usage("[options] <file ...>")
    //    .Option("-a", 1, [](){
    //
    //    })
    //    .Option("-r", 2, [](){
    //
    //    })
    torch::Arguments argu(1, "xpack command line tools", [](torch::Commander &command, std::vector<std::string> args){
        printf("$xpack args %lu:\n", args.size());
        torch::dumpArgs(args, "xpack");
        if (command.HasOption("-b"))
            printf("-r : %s\n", torch::StringVectorToString(command.GetOptionArgs("-b")).c_str());
        if (command.HasOption("-a"))
            printf("-a : %s\n", torch::StringVectorToString(command.GetOptionArgs("-a")).c_str());
        return true;
    });
    
    argu.Version("1.0").Usage("xpack [subcommands] [options] <file ...>");
    argu.MainCommand()
    .Option("-a", -1, "run action")
    .Option("-b", 0, "break all");
    
    argu.SubCommand("add", 1, "add file to xpack packge", [](torch::Commander &command, std::vector<std::string> args){
        printf("$[cmd] add args %lu:\n", args.size());
        torch::dumpArgs(args, "add");
        if (command.HasOption("-r"))
            printf("-r : %s\n", torch::StringVectorToString(command.GetOptionArgs("-r")).c_str());
        if (command.HasOption("-a"))
            printf("-a : %s\n", torch::StringVectorToString(command.GetOptionArgs("-a")).c_str());
        return true;
    })
    .Usage("usage: xpack add [options] <file ...>")
    .Option("-a", 2, "add|add to")
    .Option("-r", 1, "add|remove from");
    
    argu.SubCommand("remove", -1, "remove file from xpack package", [](torch::Commander &command, std::vector<std::string> args){
        printf("$[cmd] remove args %lu:\n", args.size());
        torch::dumpArgs(args, "remove");
        if (command.HasOption("-r"))
            printf("-r : %s\n", torch::StringVectorToString(command.GetOptionArgs("-r")).c_str());
        if (command.HasOption("-a"))
            printf("-a : %s\n", torch::StringVectorToString(command.GetOptionArgs("-a")).c_str());
        return true;
    })
    .Usage("usage: xpack remove [options] <file ...>")
    .Option("-a", -1, "fast mode, package will be fat")
    .Option("-r", 2, "real remove, package will be cleanlily");
    
    argu.Parse(argc, argv);
    
    return 0;
}
