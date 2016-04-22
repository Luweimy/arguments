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
    
    auto subcommand_add = argu.Command("add", 1, "add file to package", [](){
        
    })
    .Option("-a", 2, "add|add to", nullptr)
    .Option("-r", 1, "add|remove from", nullptr);
    
    argu.Command("remove", 1, "remove file from package", [](){
        
    })
    .Option("-a", 2, "remove|add to", nullptr)
    .Option("-r", 1, "remove|remove from", nullptr);
    
    argu.Parse(argc, argv);
    
    return 0;
}
