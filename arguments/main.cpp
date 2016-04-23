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
    torch::Arguments argu;
    argu.Version("1.0").Usage("[options] <file ...>");
    
    argu.Command("add", 1, "add file to package", [](torch::Commander &command, std::vector<std::string> args){
        return true;
    })
    .Option("-a", 2, "add|add to")
    .Option("-r", 1, "add|remove from");
    
    argu.Command("remove", -1, "remove file from package", [](torch::Commander &command, std::vector<std::string> args){
        printf("$[cmd] remove args %lu:\n", args.size());
        torch::dumpArgs(args, "remove");
        if (command.HadOption("-r"))
            printf("-r : %s\n", torch::StringVectorToString(command.GetOptionArgs("-r")).c_str());
        if (command.HadOption("-a"))
            printf("-a : %s\n", torch::StringVectorToString(command.GetOptionArgs("-a")).c_str());
        return true;
    })
    .Option("-a", -1, "remove|add to")
    .Option("-r", 2, "remove|remove from");
    
    argu.Parse(argc, argv);
    
    return 0;
}
