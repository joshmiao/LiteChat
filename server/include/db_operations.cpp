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
        unsend_messgae_from_user(sch, "unsend_messgae_from_user"),
        unsend_messgae_from_group(sch, "unsend_messgae_from_group"),
        friend_relation(sch, "friend_ralation"),
        friend_request(sch, "friend_request"),
        group_member(sch, "group_member"),
        users_group(sch, "users_group"),
        
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
        get_friend_relation(friend_relation), 
        get_friend_request(friend_request),
        get_group_member(group_member), 
        get_groups_of_a_user(users_group)
{};

int32_t LiteChatDatabaseAccess::userLogin(int32_t user_id, const std::string& email, const std::string& pwd){
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
    else return user_id != 0 ? user_id : static_cast<int32_t>(getBasicUserDataByEmail(email).get(0));
}

mysqlx::Row LiteChatDatabaseAccess::getBasicUserDataByID(int32_t user_id){
    get_basic_user_data.where("user_id = " + std::to_string(user_id));
    return get_basic_user_data.execute().fetchOne();
}

mysqlx::Row LiteChatDatabaseAccess::getBasicUserDataByEmail(const std::string& email){
    get_basic_user_data.where("email = '" + email + "'");
    return get_basic_user_data.execute().fetchOne();
}

mysqlx::Row LiteChatDatabaseAccess::getBasicGroupData(int32_t group_id){
    get_basic_group_data.where("group_id = " + std::to_string(group_id));
    return get_basic_group_data.execute().fetchOne();
}

mysqlx::RowResult LiteChatDatabaseAccess::searchUser(int32_t user_id, const std::string& user_name){
    return searchUG(get_basic_user_data, "user", user_id, user_name);
}

mysqlx::RowResult LiteChatDatabaseAccess::searchGroup(int32_t group_id, const std::string& group_name){
    return searchUG(get_basic_group_data, "group", group_id, group_name);
}

mysqlx::RowResult LiteChatDatabaseAccess::searchUserHistory(int32_t src_user_id, int32_t dst_user_id, const std::string& time_begin, const std::string& time_end){
    return searchHistory(search_user_history, "user", src_user_id, dst_user_id, time_begin, time_end);
}

mysqlx::RowResult LiteChatDatabaseAccess::searchGroupHistory(int32_t src_user_id, int32_t dst_group_id, const std::string& time_begin, const std::string& time_end){ // id=0为群中所有人的发言
    return searchHistory(search_group_history, "group", src_user_id, dst_group_id, time_begin, time_end);
}

mysqlx::RowResult LiteChatDatabaseAccess::searchUnsendMessageFromUser(int32_t unsend_user_id){
    return searchUnsendMessage(search_unsend_messgae_from_user, unsend_user_id);
}

mysqlx::RowResult LiteChatDatabaseAccess::searchUnsendMessageFromGroup(int32_t unsend_user_id){
    return searchUnsendMessage(search_unsend_messgae_from_group, unsend_user_id);
}

mysqlx::Row LiteChatDatabaseAccess::getUserStatus(int32_t user_id){
    get_user_status.where("user_id = " + std::to_string(user_id));
    return get_user_status.execute().fetchOne();
}

int32_t LiteChatDatabaseAccess::userRegister(const std::string& user_name, const std::string& pwd, const std::string& email, const std::string& birthday){
    if(!emailUnique(email))
        return -1; // -1：邮箱已注册
    mysqlx::TableInsert user_register = basic_user_data.insert("user_id", "user_name", "pwd", "email", "birthday");

    int32_t new_id = USER_ID_BEGIN + static_cast<int32_t>(regsiteredUserCount());
    user_register.values(new_id, user_name, pwd, email, birthday);
    user_register.execute();
    return new_id;
}

void LiteChatDatabaseAccess::addUserHistory(int32_t src_user_id, int32_t dst_user_id, const std::string& send_time, const std::string& content){
    addHistory(message_to_user, "user", src_user_id, dst_user_id, send_time, content);
}

void LiteChatDatabaseAccess::addGroupHistory(int32_t src_user_id, int32_t dst_group_id, const std::string& send_time, const std::string& content){
    addHistory(message_to_user, "group", src_user_id, dst_group_id, send_time, content);
}

void addUnsendMessageFromUser(int32_t unsend_user_id);
void addUnsendMessageFromGroup(int32_t unsend_user_id);

/////////////////////////////////////////////////////////////////////////////////////

int LiteChatDatabaseAccess::regsiteredUserCount(){
    return registered_user_count.execute().fetchOne().get(0);
}

bool LiteChatDatabaseAccess::emailUnique(const std::string& email){
    get_basic_user_data.where("email = '" + email + "'");
    return get_basic_user_data.execute().count() == 0;
}

int LiteChatDatabaseAccess::existedGroupCount(){
    return existed_group_count.execute().fetchOne().get(0);
}

mysqlx::RowResult LiteChatDatabaseAccess::searchUG(mysqlx::TableSelect& table_select, const std::string& type,
        int32_t id, const std::string& name){
    std::string command = "1 == 1";
    if (id != 0) // 用0表示不使用id搜索
        command += " AND " + type + "_id = " + std::to_string(id);
    if (!name.empty()) // 用空字符串表示不使用名称搜索
        command += " AND " + type + "_name = '" + name + "'";

    table_select.where(command);
    return table_select.execute();
}

mysqlx::RowResult LiteChatDatabaseAccess::searchHistory(mysqlx::TableSelect& table_select, const std::string& type,
        int32_t src_id, int32_t dst_id, const std::string& time_begin, const std::string& time_end){
    std::string command = "dst_" + type + "_id = " + std::to_string(dst_id) + 
        " AND send_time BETWEEN " + time_begin + " AND " + time_end;
    if(src_id != 0)
        command += " AND src_user_id = " + std::to_string(src_id);

    table_select.where(command);
    return table_select.execute();
}

mysqlx::RowResult LiteChatDatabaseAccess::searchUnsendMessage(mysqlx::TableSelect& table_select, int32_t unsend_user_id){
    table_select.where("unsend_user_id = " + std::to_string(unsend_user_id));
    return table_select.execute();
}

void LiteChatDatabaseAccess::addHistory(mysqlx::Table& table, const std::string& type,
        int32_t src_id, int32_t dst_id, const std::string& send_time, const std::string& content){
    mysqlx::TableInsert add_history = table.insert("send_time", "src_user_id", "dst_" + type + "_id", "content");

    add_history.values(src_id, dst_id, send_time, content);
    add_history.execute();
}