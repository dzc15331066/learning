keepalived 在编译机上面编译时,出现vrrp_if.c找不到IP_MULTICAST_ALL的定义的错误。
原因：
configure 脚本里面判断IP_MULTI_CAST在文件<linux/in.h>里面，vrrp_if.c里面#include<netinet/in.h>,但是编译机上面的netinet/in.h没有#define IP_MULTICAST_AL
所以vrrp_if.c引用IP_MULTICAST_AL时出现未定义的错误。
