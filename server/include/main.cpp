#include <iostream>
#include <mysqlx/xdevapi.h>
#include "db_operations.h"

int main(){
    LiteChatDatabaseAccess db("mysqlx://root:Sail2Boat3A@127.0.0.1");

    // 注册三个账号
    std::cout << db.userRegister("dqwdqd", "00000000000000000000000000000000", "ne#1231.com", "2001-08-02") << std::endl; // 10000
    std::cout << db.userRegister("dqwdqd", "00000000000000000000000000000000", "ne#1231.com", "2001-08-02") << std::endl; // -1
    std::cout << db.userRegister("dqqwqwedqd", "00000000000000000000000000000000", "ne#1qwqwrqwrq231.com", "2001-08-02") << std::endl; // 10001
    std::cout << std::endl;

    // 登录
    std::cout << db.userLogin(1, "ne#1231.com", "00000000000000000000000000000000") << std::endl;     // -2
    std::cout << db.userLogin(1, "", "00000000000000000000000000000000") << std::endl;                // -2
    std::cout << db.userLogin(0, "aaane#1231.com", "00000000000000000000000000000000") << std::endl;  // -2
    std::cout << db.userLogin(0, "ne#1231.com", "00000000000000000000000000000001") << std::endl;     // -1
    std::cout << db.userLogin(10000, "", "00000000000000000000000000000001") << std::endl;            // -1
    std::cout << db.userLogin(10000, "ne#1231.com", "00000000000000000000000000000001") << std::endl; // -1
    std::cout << db.userLogin(0, "ne#1231.com", "00000000000000000000000000000000") << std::endl;     // 10000
    std::cout << db.userLogin(10000, "", "00000000000000000000000000000000") << std::endl;            // 10000
    std::cout << db.userLogin(10000, "ne#1231.com", "00000000000000000000000000000000") << std::endl; // 10000
    std::cout << std::endl;

    // 获取用户信息
    auto result = db.getBasicUserDataByID(10000);
    std::cout << result.get(0) << std::endl; // 获取各个列
    std::cout << result.get(1) << std::endl;
    std::cout << result.get(2) << std::endl;
    std::cout << result.get(3) << std::endl;
    std::cout << result.get(4) << std::endl;
    std::cout << result.get(5) << std::endl;
    std::cout << std::endl;

    // 搜索用户信息
    std::cout << db.searchUser(0, "").count() << std::endl;
    std::cout << db.searchUser(1, "").count() << std::endl;
    std::cout << db.searchUser(10000, "").count() << std::endl;
    std::cout << db.searchUser(10001, "").count() << std::endl;
    std::cout << db.searchUser(10000, "dqwdqd").count() << std::endl;
    std::cout << db.searchUser(0, "dqwdqd").count() << std::endl;
    std::cout << db.searchUser(0, "dqwd49749qd").count() << std::endl;
    std::cout << db.searchUser(10000, "dqwdwqdwdqd").count() << std::endl;
    std::cout << std::endl;

    // 搜索聊天记录
    auto rowresult = db.searchUserHistory(10000, 10001, "'2000-01-01'", "now()");
    while (rowresult.count() != 0){
        result = rowresult.fetchOne();
        std::cout << result.get(0) << std::endl;
        std::cout << result.get(1) << std::endl;
        std::cout << result.get(2) << std::endl;
        std::cout << result.get(3) <<  std::endl;
    }

    return 0;
}
