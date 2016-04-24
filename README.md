# arguments
命令行参数处理   

概述：
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
- MainCommand()   
- SubCommand()   
- Parse()  
描述：处理系统参数，管理所有的命令（主命令和子命令）   

###`Commander`
- Usage()   
- Option()    
- GetOptionArgs()   
- HasOption()  
描述：处理单一命令的参数，管理注册options



