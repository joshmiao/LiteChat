#include <iostream>
#include <string>
#include <mysqlx/xdevapi.h>
#include "db_operations.h"

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
        unsend_messgae_from_user(sch, "unsend_message_from_user"),
        unsend_messgae_from_group(sch, "unsend_message_from_group"),
        friend_relation(sch, "friend_ralation"),
        friend_request(sch, "friend_request"),
        group_member(sch, "group_member"),
        groups_of_a_user(sch, "users_group"),
        
        user_login(basic_user_data, "pwd"),
        registered_user_count(basic_user_data, "COUNT(*)"),
        existed_group_count(basic_group_data, "COUNT(*)"),
        get_basic_user_data(basic_user_data, "user_id", "user_name", "email", "DATE_FORMAT(birthday, '%Y-%m-%d')", "avatar_filename", "signature"), 
        get_basic_group_data(basic_group_data, "group_id", "group_name", "owner_id", "group_description"),
        search_user_history(message_to_user, "UNIX_TIMESTAMP(send_time)", "src_user_id", "dst_user_id", "content"),
        search_group_history(message_to_group, "UNIX_TIMESTAMP(send_time)", "src_user_id", "dst_group_id", "content"),
        search_unsend_messgae_from_user(unsend_messgae_from_user, "UNIX_TIMESTAMP(send_time)", "src_user_id", "content"), 
        search_unsend_messgae_from_group(unsend_messgae_from_group, "UNIX_TIMESTAMP(send_time)", "src_user_id", "dst_group_id", "content"), 
        get_user_status(user_status, "is_online", "handle", "UNIX_TIMESTAMP(last_response)"),
        get_friend_relation(friend_relation, "user1_id", "user2_id"), 
        get_friend_request(friend_request, "user_from", "user_to", "request_message"),
        get_group_member(group_member, "group_id", "user_id"), 
        get_groups_of_a_user(groups_of_a_user, "user_id", "group_id"),
        
        delete_group(basic_group_data.remove()), 
        delete_unsend_messgae_from_user(unsend_messgae_from_user.remove()),
        delete_unsend_messgae_from_group(unsend_messgae_from_group.remove()),
        remove_user_from_group(group_member.remove()), 
        remove_group_from_user(groups_of_a_user.remove()),
        delete_friend_relation(friend_relation.remove()), 
        delete_friend_request(friend_request.remove())
{};

ID LiteChatDatabaseAccess::userLogin(ID user_id, const std::string& email, const std::string& pwd){
    if (user_id == 0 && email.empty())
        return -3; // 传输错误，id与email同时为空

    std::string command = "1 == 1";
    if (user_id != 0)
        command += " AND user_id = " + std::to_string(user_id);
    if (!email.empty()) // 用空字符串表示不使用名称搜索
        command += std::string(" AND email = '") + email + "'";
    user_login.where(command);
    auto result = user_login.execute();

    if (result.count() == 0) 
        return -2; // 用户不存在
    else if (result.fetchOne().get(0) != static_cast<mysqlx::abi2::r0::string>(pwd))
        return -1; // 密码错误
    else return user_id != 0 ? user_id : static_cast<ID>(getBasicUserDataByEmail(email).get(0));
}

mysqlx::Row LiteChatDatabaseAccess::getBasicUserDataByID(ID user_id){
    get_basic_user_data.where("user_id = " + std::to_string(user_id));
    return get_basic_user_data.execute().fetchOne();
}

mysqlx::Row LiteChatDatabaseAccess::getBasicUserDataByEmail(const std::string& email){
    get_basic_user_data.where("email = '" + email + "'");
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

mysqlx::RowResult LiteChatDatabaseAccess::searchUserHistory(ID src_user_id, ID dst_user_id, const std::string& time_begin, const std::string& time_end){
    return searchHistory(search_user_history, "user", src_user_id, dst_user_id, time_begin, time_end);
}

mysqlx::RowResult LiteChatDatabaseAccess::searchGroupHistory(ID src_user_id, ID dst_group_id, const std::string& time_begin, const std::string& time_end){ // id=0为群中所有人的发言
    return searchHistory(search_group_history, "group", src_user_id, dst_group_id, time_begin, time_end);
}

mysqlx::RowResult LiteChatDatabaseAccess::searchUnsendMessageFromUser(ID unsend_user_id){
    return searchUnsendMessage(search_unsend_messgae_from_user, unsend_user_id);
}

mysqlx::RowResult LiteChatDatabaseAccess::searchUnsendMessageFromGroup(ID unsend_user_id){
    return searchUnsendMessage(search_unsend_messgae_from_group, unsend_user_id);
}

mysqlx::Row LiteChatDatabaseAccess::getUserStatus(ID user_id){
    get_user_status.where("user_id = " + std::to_string(user_id));
    return get_user_status.execute().fetchOne();
}

mysqlx::RowResult LiteChatDatabaseAccess::getFriendRelation(ID user1_id, ID user2_id){
    std::string command = "user1_id = '" + std::to_string(user1_id) + "'";
    if(user2_id != 0)
        command += " AND user2_id = '" + std::to_string(user2_id) + "'";

    get_friend_relation.where(command);
    return get_friend_relation.execute();
}

mysqlx::RowResult LiteChatDatabaseAccess::getFriendRequest(ID user_to){
    get_friend_request.where("user_to = '" + std::to_string(user_to) + "'");
    return get_friend_request.execute();
}

mysqlx::RowResult LiteChatDatabaseAccess::getGroupMember(ID group_id){
    get_group_member.where("group_id = '" + std::to_string(group_id) + "'");
    return get_group_member.execute();
}

mysqlx::RowResult LiteChatDatabaseAccess::getGroupsOfAUser(ID user_id){
    get_groups_of_a_user.where("user_id = '" + std::to_string(user_id) + "'");
    return get_group_member.execute();
}

ID LiteChatDatabaseAccess::userRegister(const std::string& user_name, const std::string& pwd, const std::string& email, const std::string& birthday){
    if(!emailUnique(email))
        return -1; // -1：邮箱已注册

    mysqlx::TableInsert user_register = basic_user_data.insert("user_id", "user_name", "pwd", "email", "birthday");
    ID new_id = USER_ID_BEGIN + static_cast<ID>(regsiteredUserCount());
    user_register.values(new_id, user_name, pwd, email, birthday);
    user_register.execute();

    mysqlx::TableInsert create_user_status = user_status.insert("user_id");
    create_user_status.values(new_id);
    create_user_status.execute();

    return new_id;
}

void LiteChatDatabaseAccess::addUserHistory(const std::string& send_time, ID src_user_id, ID dst_user_id, const std::string& content){
    addHistory(message_to_user, "user", send_time, src_user_id, dst_user_id, content);
}

void LiteChatDatabaseAccess::addGroupHistory(const std::string& send_time, ID src_user_id, ID dst_group_id, const std::string& content){
    addHistory(message_to_user, "group", send_time, src_user_id, dst_group_id, content);
}

void LiteChatDatabaseAccess::addUnsendMessageFromUser(const std::string& send_time, ID unsend_user_id, ID src_user_id, const std::string& content){
    mysqlx::TableInsert add_unsend_messgae_from_user = unsend_messgae_from_user.insert("send_time", "unsend_user_id", "src_user_id", "content");

    add_unsend_messgae_from_user.values(send_time, unsend_user_id, src_user_id, content);
    add_unsend_messgae_from_user.execute();
}

void LiteChatDatabaseAccess::addUnsendMessageFromGroup(const std::string& send_time, ID unsend_user_id, ID src_user_id, ID dst_group_id, const std::string& content){
    mysqlx::TableInsert add_unsend_messgae_from_group= unsend_messgae_from_user.insert("send_time", "unsend_user_id", "src_user_id", "dst_group_id", "content");

    add_unsend_messgae_from_group.values(send_time, unsend_user_id, src_user_id, dst_group_id, content);
    add_unsend_messgae_from_group.execute();
}

ID LiteChatDatabaseAccess::createGroup(const std::string& group_name, ID owner_id, const std::string& group_description){
    if(!groupNameUnique(group_name))
        return -1;

    mysqlx::TableInsert create_group = basic_group_data.insert("group_id", "group_name", "owner_id", "group_description");
    ID group_id = GROUP_ID_BEGIN + existGroupCount();

    create_group.values(group_id, group_name, owner_id, group_description);
    create_group.execute();
    return group_id;
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

void LiteChatDatabaseAccess::updateUserStatusWhenLogin(ID user_id, int handle){
    auto update_user_status_when_login =  updateUserStatus();
    update_user_status_when_login.where("user_id = '" + std::to_string(user_id) + "'");
    update_user_status_when_login.set("is_online", true);
    update_user_status_when_login.set("handle", handle);
    update_user_status_when_login.execute();
}

void LiteChatDatabaseAccess::deleteGroup(ID group_id){
    delete_group.where("group_id = " + std::to_string(group_id));
    delete_group.execute();
}

void LiteChatDatabaseAccess::deleteUnsendMessageFromUser(ID unsend_user_id){
    delete_unsend_messgae_from_user.where("unsend_user_id = " + std::to_string(unsend_user_id));
    delete_unsend_messgae_from_user.execute();
}

void LiteChatDatabaseAccess::deleteUnsendMessageFromGroup(ID unsend_user_id){
    delete_unsend_messgae_from_group.where("unsend_user_id = " + std::to_string(unsend_user_id));
    delete_unsend_messgae_from_group.execute();
}

/////////////////////////////////////////////////////////////////////////////////////

int LiteChatDatabaseAccess::regsiteredUserCount(){
    return registered_user_count.execute().fetchOne().get(0);
}

bool LiteChatDatabaseAccess::emailUnique(const std::string& email){
    get_basic_user_data.where("email = '" + email + "'");
    return get_basic_user_data.execute().count() == 0;
}

int LiteChatDatabaseAccess::existGroupCount(){
    return existed_group_count.execute().fetchOne().get(0);
}

bool LiteChatDatabaseAccess::groupNameUnique(const std::string& group_name){
    get_basic_group_data.where("group_name = '" + group_name + "'");
    return get_basic_group_data.execute().count() == 0;
}

mysqlx::RowResult LiteChatDatabaseAccess::searchUG(mysqlx::TableSelect& table_select, const std::string& type,
        ID id, const std::string& name){
    std::string command = "1 == 1";
    if (id != 0) // 用0表示不使用id搜索
        command += " AND " + type + "_id = " + std::to_string(id);
    if (!name.empty()) // 用空字符串表示不使用名称搜索
        command += " AND " + type + "_name = '" + name + "'";

    table_select.where(command);
    return table_select.execute();
}

mysqlx::RowResult LiteChatDatabaseAccess::searchHistory(mysqlx::TableSelect& table_select, const std::string& type,
        ID src_id, ID dst_id, const std::string& time_begin, const std::string& time_end){
    std::string command = "dst_" + type + "_id = " + std::to_string(dst_id) + 
        " AND send_time BETWEEN " + time_begin + " AND " + time_end;
    if(src_id != 0)
        command += " AND src_user_id = " + std::to_string(src_id);

    table_select.where(command);
    return table_select.execute();
}

mysqlx::RowResult LiteChatDatabaseAccess::searchUnsendMessage(mysqlx::TableSelect& table_select, ID unsend_user_id){
    table_select.where("unsend_user_id = " + std::to_string(unsend_user_id));
    return table_select.execute();
}

void LiteChatDatabaseAccess::addHistory(mysqlx::Table& table, const std::string& type,
        const std::string& send_time, ID src_id, ID dst_id, const std::string& content){
    mysqlx::TableInsert add_history = table.insert("send_time", "src_user_id", "dst_" + type + "_id", "content");

    add_history.values(send_time, src_id, dst_id, content);
    add_history.execute();
}