#include <iostream>
#include <string>
#include <mysqlx/xdevapi.h>
#include "db_operations.h"
#include "db_table_column.h"

// template<typename ... Args>
// std::string string_format( const std::string& format, Args ... args )
// {
//     int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
//     if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
//     auto size = static_cast<size_t>( size_s );
//     std::unique_ptr<char[]> buf( new char[ size ] );
//     std::snprintf( buf.get(), size, format.c_str(), args ... );
//     return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
// }

LiteChatDatabaseAccess::LiteChatDatabaseAccess(const std::string& url): 
        sess(url),

        sch(sess, "LiteChat"),

        basic_user_data(sch, "basic_user_data"),
        basic_group_data(sch, "basic_group_data"),
        user_status(sch, "user_status"),
        message_to_user(sch, "message_to_user"),
        message_to_group(sch, "message_to_group"),
        user_unsend_messgae(sch, "unsend_message_from_user"),
        group_unsend_messgae(sch, "unsend_message_from_group"),
        friend_relation(sch, "friend_relation"),
        friend_request(sch, "friend_request"),
        group_member(sch, "group_member"),
        group_request(sch, "group_request"),
        
        user_login(basic_user_data, "pwd"),
        registered_user_count(basic_user_data, "COUNT(*)"),
        existed_group_id_max(basic_group_data, "MAX(group_id)"),
        get_basic_user_data(basic_user_data, "user_id", "user_name", "email", "DATE_FORMAT(birthday, '%Y-%m-%d')", "avatar_filename", "signature"), 
        get_basic_group_data(basic_group_data, "group_id", "group_name", "owner_id", "group_description"),
        search_user_history(message_to_user, "DATE_FORMAT(send_time, '%Y-%m-%d %T.%f')", "src_user_id", "dst_user_id", "content", "is_file"),
        search_group_history(message_to_group, "DATE_FORMAT(send_time, '%Y-%m-%d %T.%f')", "src_user_id", "dst_group_id", "content", "is_file"),
        search_user_unsend_messgae(user_unsend_messgae, "DATE_FORMAT(send_time, '%Y-%m-%d %T.%f')", "src_user_id", "content", "is_file"), 
        search_group_unsend_messgae(group_unsend_messgae, "DATE_FORMAT(send_time, '%Y-%m-%d %T.%f')", "src_user_id", "dst_group_id", "content", "is_file"), 
        get_user_status(user_status, "is_online", "handle", "token"),
        get_friend_relation(friend_relation, "user1_id", "user2_id"), 
        get_friend_request(friend_request, "user_from", "user_to", "request_message"),
        get_group_member(group_member, "group_id", "user_id"), 
        get_group_request(group_request, "user_id", "group_id", "request_message"),
        
        delete_group(basic_group_data.remove()), 
        delete_user_history(message_to_user.remove()),
        delete_group_history(message_to_group.remove()),
        delete_user_unsend_messgae(user_unsend_messgae.remove()),
        delete_group_unsend_messgae(group_unsend_messgae.remove()),
        remove_user_from_group(group_member.remove()),
        delete_group_request(group_request.remove()),
        delete_friend_relation(friend_relation.remove()), 
        delete_friend_request(friend_request.remove())
{};

ID LiteChatDatabaseAccess::userLogin(ID user_id, const std::string& email, const std::string& pwd){
    if (user_id == 0 && email.empty())
        return -3; // ???????????????id???email????????????

    std::string command = "1 == 1";
    if (user_id != 0)
        command += " AND user_id = " + std::to_string(user_id);
    if (!email.empty()) // ??????????????????????????????????????????
        command +=  "AND email = " + sqlString(email);
    user_login.where(command);
    auto result = user_login.execute();

    if (result.count() == 0) 
        return -2; // ???????????????
    else if (result.fetchOne().get(USER_LOGIN_PWD) != static_cast<mysqlx::abi2::r0::string>(pwd))
        return -1; // ????????????
    else return user_id != 0 ? user_id : static_cast<ID>(getBasicUserDataByEmail(email).get(BASIC_USER_DATA_USER_ID));
}

mysqlx::Row LiteChatDatabaseAccess::getBasicUserDataByID(ID user_id){
    get_basic_user_data.where("user_id = " + std::to_string(user_id));
    return get_basic_user_data.execute().fetchOne();
}

mysqlx::Row LiteChatDatabaseAccess::getBasicUserDataByEmail(const std::string& email){
    get_basic_user_data.where("email = " + sqlString(email));
    return get_basic_user_data.execute().fetchOne();
}

mysqlx::Row LiteChatDatabaseAccess::getBasicGroupData(ID group_id){
    get_basic_group_data.where("group_id = " + std::to_string(group_id));
    return get_basic_group_data.execute().fetchOne();
}

mysqlx::RowResult LiteChatDatabaseAccess::searchUser(ID user_id, const std::string& user_name){
    return searchUG(get_basic_user_data, "user", user_id, user_name);
}

mysqlx::RowResult LiteChatDatabaseAccess::searchGroup(ID group_id, const std::string& group_name){
    return searchUG(get_basic_group_data, "group", group_id, group_name);
}

mysqlx::RowResult LiteChatDatabaseAccess::searchUserHistory(ID user1_id, ID user2_id, const std::string& time_begin, const std::string& time_end, bool is_file){
    std::string command = "(send_time BETWEEN " + time_begin + " AND " + time_end + 
        ") AND ((dst_user_id = " + std::to_string(user1_id) + " AND src_user_id = " + std::to_string(user2_id) + ") OR (dst_user_id = "
         + std::to_string(user2_id) + " AND src_user_id = " + std::to_string(user1_id) + "))";

    search_user_history.where(command);
    return search_user_history.execute();
}

mysqlx::RowResult LiteChatDatabaseAccess::searchGroupHistory(ID src_user_id, ID dst_group_id, const std::string& time_begin, const std::string& time_end, bool is_file){ // id=0???????????????????????????
    std::string command = "dst_group_id = " + std::to_string(dst_group_id) + 
        " AND send_time BETWEEN " + time_begin + " AND " + time_end;
    if(src_user_id != 0)
        command += " AND src_user_id = " + std::to_string(src_user_id);

    search_group_history.where(command);
    return search_group_history.execute();
}

mysqlx::RowResult LiteChatDatabaseAccess::searchUserUnsendMessage(ID unsend_user_id){
    return searchUnsendMessage(search_user_unsend_messgae, "user", unsend_user_id);
}

mysqlx::RowResult LiteChatDatabaseAccess::searchGroupUnsendMessage(ID unsend_user_id){
    return searchUnsendMessage(search_group_unsend_messgae, "group", unsend_user_id);
}

mysqlx::Row LiteChatDatabaseAccess::getUserStatus(ID user_id){
    get_user_status.where("user_id = " + std::to_string(user_id));
    return get_user_status.execute().fetchOne();
}

mysqlx::RowResult LiteChatDatabaseAccess::getFriendRelation(ID user1_id, ID user2_id){
    std::string command = "user1_id = " + std::to_string(user1_id);
    if(user2_id != 0)
        command += " AND user2_id = " + std::to_string(user2_id);

    get_friend_relation.where(command);
    return get_friend_relation.execute();
}

mysqlx::RowResult LiteChatDatabaseAccess::getFriendRequest(ID user_from, ID user_to){
    std::string command = "1 == 1";
    if(user_from != 0)
        command += "AND user_from = " + std::to_string(user_from);
    if(user_to != 0)
        command += "AND user_to = " + std::to_string(user_to);
    
    get_friend_request.where(command);
    return get_friend_request.execute();
}

mysqlx::RowResult LiteChatDatabaseAccess::getGroupRequest(ID user_id, ID group_id){
    std::string command = "1 == 1";
    if(user_id != 0)
        command += "AND user_id = " + std::to_string(user_id);
    if(group_id != 0)
        command += "AND group_id = " + std::to_string(group_id);
    
    get_group_request.where(command);
    return get_group_request.execute();
}

mysqlx::RowResult LiteChatDatabaseAccess::getGroupMember(ID group_id){
    get_group_member.where("group_id = " + std::to_string(group_id));
    return get_group_member.execute();
}

mysqlx::RowResult LiteChatDatabaseAccess::getGroupsOfAUser(ID user_id, ID group_id){
    std::string command = "user_id = " + std::to_string(user_id);
    if(group_id != 0)
        command += "AND group_id = " + std::to_string(group_id);
    get_group_member.where(command);
    return get_group_member.execute();
}

ID LiteChatDatabaseAccess::userRegister(const std::string& user_name, const std::string& pwd, const std::string& email, const std::string& birthday){
    if(!emailUnique(email))
        return -1; // -1??????????????????

    mysqlx::TableInsert user_register = basic_user_data.insert("user_id", "user_name", "pwd", "email", "birthday");
    ID new_id = USER_ID_BEGIN + static_cast<ID>(regsiteredUserCount());
    user_register.values(new_id, user_name, pwd, email, birthday);
    user_register.execute();

    mysqlx::TableInsert create_user_status = user_status.insert("user_id");
    create_user_status.values(new_id);
    create_user_status.execute();

    return new_id;
}

void LiteChatDatabaseAccess::addUserHistory(const std::string& send_time, ID src_user_id, ID dst_user_id, const std::string& content, bool is_file){
    addHistory(message_to_user, "user", send_time, src_user_id, dst_user_id, content, is_file);
}

void LiteChatDatabaseAccess::addGroupHistory(const std::string& send_time, ID src_user_id, ID dst_group_id, const std::string& content, bool is_file){
    addHistory(message_to_group, "group", send_time, src_user_id, dst_group_id, content, is_file);
}

void LiteChatDatabaseAccess::addUserUnsendMessage(const std::string& send_time, ID unsend_user_id, ID src_user_id, const std::string& content, bool is_file){
    mysqlx::TableInsert add_user_unsend_messgae = user_unsend_messgae.insert("send_time", "unsend_user_id", "src_user_id", "content", "is_file");

    add_user_unsend_messgae.values(send_time, unsend_user_id, src_user_id, content, is_file);
    add_user_unsend_messgae.execute();
}

void LiteChatDatabaseAccess::addGroupUnsendMessage(const std::string& send_time, ID unsend_user_id, ID src_user_id, ID dst_group_id, const std::string& content, bool is_file){
    mysqlx::TableInsert add_group_unsend_messgae= group_unsend_messgae.insert("send_time", "unsend_user_id", "src_user_id", "dst_group_id", "content", "is_file");

    add_group_unsend_messgae.values(send_time, unsend_user_id, src_user_id, dst_group_id, content, is_file);
    add_group_unsend_messgae.execute();
}

ID LiteChatDatabaseAccess::createGroup(const std::string& group_name, ID owner_id, const std::string& group_description){
    if(!groupNameUnique(group_name))
        return -1;

    mysqlx::TableInsert create_group = basic_group_data.insert("group_id", "group_name", "owner_id", "group_description");
    ID group_id = existGroupIDMax() + 1;

    create_group.values(group_id, group_name, owner_id, group_description);
    create_group.execute();
    return group_id;
}

void LiteChatDatabaseAccess::addUserToGroup(ID group_id, ID user_id){
    mysqlx::TableInsert add_user_to_group = group_member.insert("group_id", "user_id");

    add_user_to_group.values(group_id, user_id);
    add_user_to_group.execute();
}

int LiteChatDatabaseAccess::createGroupRequest(ID user_id, ID group_id, const std::string& request_message){
    if(!groupRequestUnique(user_id, group_id))
        return -1;
    if(getGroupsOfAUser(user_id, group_id).count() != 0)
        return -2;

    mysqlx::TableInsert create_group_request = group_request.insert("user_id", "group_id", "request_message");

    create_group_request.values(user_id, group_id, request_message);
    create_group_request.execute();
    return 0;
}

void LiteChatDatabaseAccess::createFriendRelation(ID user1_id, ID user2_id){
    mysqlx::TableInsert create_friend_relation = friend_relation.insert("user1_id", "user2_id");
    create_friend_relation.values(user1_id, user2_id);
    create_friend_relation.execute();
    create_friend_relation = friend_relation.insert("user2_id", "user1_id");
    create_friend_relation.values(user1_id, user2_id);
    create_friend_relation.execute();
}

int LiteChatDatabaseAccess::createFriendRequest(ID user_from, ID user_to, const std::string& request_message){
    if(!friendRequestUnique(user_from, user_to))
        return -1;
    if(getFriendRelation(user_from, user_to).count() != 0)
        return -2;

    mysqlx::TableInsert create_friend_request = friend_request.insert("user_from", "user_to", "request_message");

    create_friend_request.values(user_from, user_to, request_message);
    create_friend_request.execute();
    return 0;
}

mysqlx::TableUpdate LiteChatDatabaseAccess::updateBasicUserData(){
    return basic_user_data.update();
}

mysqlx::TableUpdate LiteChatDatabaseAccess::updateBasicGroupData(){
    return basic_group_data.update();
}

mysqlx::TableUpdate LiteChatDatabaseAccess::updateUserStatus(){
    return user_status.update();
}

void LiteChatDatabaseAccess::updateUserStatusWhenLogin(ID user_id, int handle, const std::string& token){
    auto update_user_status_when_login =  updateUserStatus();
    update_user_status_when_login.where("user_id = " + std::to_string(user_id));
    update_user_status_when_login.set("is_online", true);
    update_user_status_when_login.set("handle", handle);
    update_user_status_when_login.set("token", token);
    update_user_status_when_login.execute();
}

void LiteChatDatabaseAccess::updateUserStatusWhenLogout(int handle){
    auto update_user_status_when_login = updateUserStatus();
    update_user_status_when_login.where("handle = " + std::to_string(handle));
    update_user_status_when_login.set("is_online", false);
    update_user_status_when_login.set("handle", nullptr);
    update_user_status_when_login.set("token", nullptr);
    update_user_status_when_login.execute();
}
 
void LiteChatDatabaseAccess::deleteGroup(ID group_id){
    delete_group.where("group_id = " + std::to_string(group_id));
    delete_group.execute();
}

void LiteChatDatabaseAccess::deleteUserHistory(ID user1_id, ID user2_id){
    std::string command = "(src_user_id = " + std::to_string(user1_id) + " AND dst_user_id = " + std::to_string(user2_id)
        + ") OR (src_user_id = " + std::to_string(user2_id) + " AND dst_user_id = " + std::to_string(user1_id) + ")";
    delete_user_history.where(command);
    delete_user_history.execute();
}

void LiteChatDatabaseAccess::deleteGroupHistory(ID group_id){
    delete_group_history.where("dst_group_id = " + std::to_string(group_id));
    delete_group_history.execute();
}

void LiteChatDatabaseAccess::deleteUserUnsendMessage(ID unsend_user_id){
    delete_user_unsend_messgae.where("unsend_user_id = " + std::to_string(unsend_user_id));
    delete_user_unsend_messgae.execute();
}

void LiteChatDatabaseAccess::deleteGroupUnsendMessage(ID unsend_user_id, ID group_id){
    std::string command = "1 == 1";
    if(unsend_user_id != 0)
        command += "AND unsend_user_id = " + std::to_string(unsend_user_id);
    if(group_id != 0)
        command += "AND dst_group_id = " + std::to_string(group_id);

    delete_group_unsend_messgae.where(command);
    delete_group_unsend_messgae.execute();
}

void LiteChatDatabaseAccess::removeUserFromGroup(ID group_id, ID user_id){
    std::string command = "group_id = " + std::to_string(group_id);
    if (user_id != 0)
        command += " AND user_id = " + std::to_string(user_id);

    remove_user_from_group.where(command);
    remove_user_from_group.execute();
}

void LiteChatDatabaseAccess::deleteGroupRequest(ID user_id, ID group_id){
    delete_group_request.where("user_id = " + std::to_string(user_id) + " AND group_id = " + std::to_string(group_id));
    delete_group_request.execute();
}

void LiteChatDatabaseAccess::deleteFriendRelation(ID user1_id, ID user2_id){
    delete_friend_relation.where("user1_id = " + std::to_string(user1_id) + " AND user2_id = " + std::to_string(user2_id));
    delete_friend_relation.execute();
    delete_friend_relation.where("user1_id = " + std::to_string(user2_id) + " AND user2_id = " + std::to_string(user1_id));
    delete_friend_relation.execute();
}

void LiteChatDatabaseAccess::deleteFriendRequest(ID user_from, ID user_to){
    delete_friend_request.where("user_from = " + std::to_string(user_from) + " AND user_to = " + std::to_string(user_to));
    delete_friend_request.execute();
    delete_friend_request.where("user_from = " + std::to_string(user_to) + " AND user_to = " + std::to_string(user_from));
    delete_friend_request.execute();
}

/////////////////////////////////////////////////////////////////////////////////////

std::string LiteChatDatabaseAccess::sqlString(const std::string& s){
    return "'" + s + "'";
}

int LiteChatDatabaseAccess::regsiteredUserCount(){
    return registered_user_count.execute().fetchOne().get(REGISTERED_USER_COUNT);
}

bool LiteChatDatabaseAccess::emailUnique(const std::string& email){
    get_basic_user_data.where("email = " + sqlString(email));
    return get_basic_user_data.execute().count() == 0;
}

int LiteChatDatabaseAccess::existGroupIDMax(){
    auto result = existed_group_id_max.execute().fetchOne().get(EXISTED_GROUP_ID_MAX);
    return result.isNull() ? GROUP_ID_BEGIN - 1: (int)result;
}

bool LiteChatDatabaseAccess::groupNameUnique(const std::string& group_name){
    get_basic_group_data.where("group_name = " + sqlString(group_name));
    return get_basic_group_data.execute().count() == 0;
}

mysqlx::RowResult LiteChatDatabaseAccess::searchUG(mysqlx::TableSelect& table_select, const std::string& type,
        ID id, const std::string& name){
    std::string command = type + "_id = " + std::to_string(id) + " OR " + type + "_name = " + sqlString(name);
        
    table_select.where(command);
    return table_select.execute();
}

mysqlx::RowResult LiteChatDatabaseAccess::searchUnsendMessage(mysqlx::TableSelect& table_select, const std::string& type, ID unsend_user_id){
    table_select.where("unsend_user_id = " + std::to_string(unsend_user_id));
    return table_select.execute();
}

void LiteChatDatabaseAccess::addHistory(mysqlx::Table& table, const std::string& type,
        const std::string& send_time, ID src_id, ID dst_id, const std::string& content, bool is_file){
    mysqlx::TableInsert add_history = table.insert("send_time", "src_user_id", "dst_" + type + "_id", "content", "is_file");

    add_history.values(send_time, src_id, dst_id, content, is_file);
    add_history.execute();
}

bool LiteChatDatabaseAccess::friendRequestUnique(ID user_from, ID user_to){
    return getFriendRequest(user_from, user_to).count() == 0;
}

bool LiteChatDatabaseAccess::groupRequestUnique(ID user_id, ID group_id){
    return getGroupRequest(user_id, group_id).count() == 0;
}