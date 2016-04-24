# arguments
命令行参数处理   

###概述：
####主命令和子命令(MainCommand & SubCommand)   
主命令是默认存在的，子命令视需求自己添加   
例如：`xpack add file`这条命令中主命令就是xpack这条命令，而add则是xpack命令的子命令   

####可选项Options
可选项不是必须的，命令中可有可无，无论主命令还是子命令都可以有options。
例如：`xpack -h`命令中`-h`就是options    
options同样也可以有参数       
例如：`xpack -a path/to`

每个Arguments对象默认都有一个MainCommand(Commander对象)，用于处理主命令参数和Options  

###`Arguments`  
描述：处理系统参数，管理所有的命令（主命令和子命令）   
- MainCommand()   
- SubCommand()   
- Parse()  


###`Commander`
描述：处理单一命令的参数，管理注册options
- Usage()   
- Option()    
- GetOptionArgs()   
- HasOption()  

```c
#include <iostream>
#include <vector>
#include <assert.h>
#include "torch-arguments.h"
bool OnAdd(torch::Commander &command, std::vector<std::string> args) {
    if (command.HasOption("--password")) {
        printf("password:\t%s\n", command.GetOptionArgs("--password")[0].c_str());
    }
    for (auto path : args) { printf(" add: %s ... ok\n", path.c_str()); }
    return true;
}
bool OnDelete(torch::Commander &command, std::vector<std::string> args) {
    for (auto path : args) { printf(" delete: %s ... ok\n", path.c_str()); }
    return true;
}
bool OnCommit(torch::Commander &command, std::vector<std::string> args) {
    if (command.HasOption("--ignore")) {
        for (auto path : command.GetOptionArgs("--ignore")) { printf(" ignore: %s ... ok\n", path.c_str()); }
    }
    for (auto path : args) { printf(" commit: %s ... ok\n", path.c_str()); }
    return true;
}
bool OnCommitOptionUnlock(torch::Commander &command, std::vector<std::string> args) {
    for (auto path : args) { printf(" unlock: %s ... ok\n", path.c_str()); }
    return true;
}

int main(int argc, const char * argv[]) {
    torch::Arguments app(0, "svn command line client, version 1.0.1.");
    
    app.MainCommand()
    .Usage("usage: svn <subcommand> [options] [args]")
    .Option("-h", 0, "Show help document.", torch::Arguments::CallbackFail);
    
    // ./arguments add --passowrd 123 path1 path2 path3
    std::string addDescription = "add: Put files and directories under version control.";
    app.SubCommand("add", -1, addDescription, OnAdd)
    .Usage("usage: add PATH...")
    .Option("--help", 0, "Show subcommand help document", torch::Arguments::CallbackFail)
    .Option("--force", 0, "force operation to run")
    .Option("--depth", 1, "limit operation by depth ARG")
    .Option("--username", 1, "specify a username ARG")
    .Option("--password", 1, "specify a password ARG");
    
    // ./arguments delete --help
    std::string deleteDescription = "delete: Remove files and directories from version control.";
    app.SubCommand("delete", 1, deleteDescription, OnDelete)
    .Usage("usage:\n\t1. delete PATH...\n\t2. delete URL...")
    .Option("--help", 0, "Show subcommand help document", torch::Arguments::CallbackFail)
    .Option("--force", 0, "force operation to run");
    
    // ./arguments commit ci1 ci2 ci3 --ignore ig1 ig2 ig3 --unlock uk1 uk2 uk3
    std::string commitDescription = "commit: Send changes from your working copy to the repository.";
    app.SubCommand("commit", -1, commitDescription, OnCommit)
    .Usage("usage: commit [PATH...]")
    .Option("--help", 0, "Show subcommand help document", torch::Arguments::CallbackFail)
    .Option("--ignore", -1, "ignore PATH...")
    .Option("--unlock", -1, "unlock PATH...", OnCommitOptionUnlock);

    bool ok = app.Parse(argc, argv);
    if (!ok) {
        // TODO:
    }
    return 0;
}
```

