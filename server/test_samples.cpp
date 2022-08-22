#include <iostream>
#include <mysqlx/xdevapi.h>
#include "include/db_operations.h"

int main(){
    LiteChatDatabaseAccess db("mysqlx://root:Sail2Boat3A@127.0.0.1");

    // 注册
    std::cout << db.userRegister("dqwdqd", "00000000000000000000000000000000", "ne#1231.com", "2001-08-02") << std::endl; // 10000
    std::cout << db.userRegister("dqwdqd", "00000000000000000000000000000000", "ne#1231.com", "2001-08-02") << std::endl; // -1
    std::cout << db.userRegister("dqqwqwedqd", "00000000000000000000000000000000", "ne#1qwqwrqwrq231.com", "2001-08-02") << std::endl; // 10001
    std::cout << db.userRegister("dqqwqwedqd", "00000000000000000000000005000000", "ne#1qwqwrqw31.com", "2001-08-02") << std::endl; // 10002
    std::cout << db.userRegister("dqqwqwedqd", "00000000000000000000000005000000", "ne#1qwqwq231.com", "2001-08-02") << std::endl; // 10003
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
    std::cout << result.get(0) << std::endl; // 获取各个列，不包括pwd
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

    // 更新用户信息
    auto update = db.updateBasicUserData();
    update.where("user_id = 10000");
    update.set("birthday", "2001-08-20");
    update.execute();
    std::cout << db.searchUser(10000, "").fetchOne().get(3) << std::endl;
    std::cout << std::endl;

    // 添加聊天记录
    db.addUserHistory("2022-08-20 13:14:15.233", 10000, 10000, "w");
    db.addUserHistory("2022-08-20 13:14:15.233", 10000, 10001, "w");
    db.addUserHistory("2022-08-20 13:14:15.233", 10001, 10001, "w");
    db.addUserHistory("2022-08-20 13:14:15.232", 10001, 10000, "sdasdas");
    db.addUserHistory("2022-08-20", 10001, 10000, "w");
    db.addUserHistory("2022-08-20 13:14:15.234", 10001, 10000, "w");
    db.addUserHistory("2022-08-20 13:14:15.234", 10000, 10001, "wwdqwqwdqwd");
    db.addUserHistory("2005-02-01 11:22:33.444", 10002, 10003, "nothing");
    db.addUserHistory("2005-02-01 11:23:33.444", 10002, 10003, "nothdwqing");
    db.addUserHistory("2005-02-01 11:24:33.444", 10001, 10003, "nothiqwdng");
    db.addUserHistory("2005-02-01 11:25:33.444", 10001, 10002, "noqwdthing");

    // 搜索聊天记录
    auto rowresult = db.searchUserHistory(10000, 10001, "'2000-01-01'", "now()");
    while (rowresult.count() != 0){
        result = rowresult.fetchOne();
        std::cout << result.get(0) << std::endl;
        std::cout << result.get(1) << std::endl;
        std::cout << result.get(2) << std::endl;
        std::cout << result.get(3) <<  std::endl;
    }
    rowresult = db.searchUserHistory(10002, 10003, "'2000-01-01'", "now()");
    while (rowresult.count() != 0){
        result = rowresult.fetchOne();
        std::cout << result.get(0) << std::endl;
        std::cout << result.get(1) << std::endl;
        std::cout << result.get(2) << std::endl;
        std::cout << result.get(3) <<  std::endl;
    }
    std::cout << std::endl;

    // 添加未发送信息
    db.addUserUnsendMessage("2022-07-20 13:14:15.234", 10000, 10002, "wwdqwqwdqwd");
    db.addUserUnsendMessage("2022-07-20 13:14:15.434", 10000, 10002, "wwdqwefwqwd");
    db.addUserUnsendMessage("2022-07-20 13:14:15.534", 10000, 10002, "wwqwd");
    db.addUserUnsendMessage("2022-07-20 13:14:15.634", 10000, 10002, "wwqwd");

    // 搜索未发送信息
    rowresult = db.searchUserUnsendMessage(10000);
    std::cout << "FROM 10000" << std::endl;
    while (rowresult.count() != 0){
        result = rowresult.fetchOne();
        std::cout << result.get(0) << std::endl;
        std::cout << result.get(1) << std::endl;
        std::cout << result.get(2) << std::endl;
    }
    rowresult = db.searchUserUnsendMessage(10002);
    std::cout << "FROM 10002" << std::endl;
    while (rowresult.count() != 0){
        result = rowresult.fetchOne();
        std::cout << result.get(0) << std::endl;
        std::cout << result.get(1) << std::endl;
        std::cout << result.get(2) << std::endl;
    }
    std::cout << std::endl;

    // 删除未发送信息
    db.deleteUserUnsendMessage(10000);
    std::cout << db.searchUserUnsendMessage(10000).count() << std::endl;
    std::cout << std::endl;

    // 添加群
    db.createGroup("123", 10000, "");
    db.createGroup("12r33", 10001, "r56+23");
    db.createGroup("1223", 10002, "r53");
    db.createGroup("1222", 10002, "r62d53");

    // 删除群
    db.deleteGroup(10001);
    
    // 添加加群请求
    std::cout << db.createGroupRequest(10000, 10000, "") << std::endl;
    std::cout << db.createGroupRequest(10000, 10000, "") << std::endl;
    std::cout << db.createGroupRequest(10000, 10000, "") << std::endl;
    std::cout << db.createGroupRequest(10000, 10003, "") << std::endl;
    std::cout << db.createGroupRequest(10000, 10002, "") << std::endl;
    std::cout << std::endl;

    // 删除加群请求
    db.removeGroupRequest(10000, 10000);
    db.removeGroupRequest(10001, 10000);
    db.removeGroupRequest(10002, 10000);

    // 添加好友请求
    std::cout << db.createFriendRequest(10000, 10001, "") << std::endl;
    std::cout << db.createFriendRequest(10000, 10001, "") << std::endl;
    std::cout << db.createFriendRequest(10000, 10001, "") << std::endl;
    std::cout << db.createFriendRequest(10001, 10000, "") << std::endl;
    std::cout << db.createFriendRequest(10001, 10000, "") << std::endl;
    std::cout << db.createFriendRequest(10001, 10002, "") << std::endl;
    std::cout << db.createFriendRequest(10000, 10002, "") << std::endl;
    std::cout << std::endl;

    // 删除好友请求
    db.deleteFriendRequest(10000, 10001);
    db.deleteFriendRequest(10001, 10000);
    db.deleteFriendRequest(10001, 10002);
    db.deleteFriendRequest(10000, 10002);

    // 添加朋友关系
    db.createFriendRelation(10000, 10001);
    db.createFriendRelation(10000, 10002);
    db.createFriendRelation(10001, 10002);

    // 删除朋友关系
    db.deleteFriendRelation(10000, 10001);

    // 更改用户状态
    db.updateUserStatusWhenLogin(10000, 123, "q4d9q4w8d9q4wd9qw4d98qd4");
    db.updateUserStatusWhenLogout(123);
    db.updateUserStatusWhenLogout(10000);
    
    return 0;
}
