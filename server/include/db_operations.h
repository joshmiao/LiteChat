#ifndef H_DB_OPERATIONS
#define H_DB_OPERATIONS

#include <string>
#include <mysqlx/xdevapi.h>

constexpr int32_t USER_ID_BEGIN = 10000;
constexpr int32_t GROUP_ID_BEGIN = 10000;

class LiteChatDatabaseAccess
{
public:
    LiteChatDatabaseAccess(const std::string& url);

    int32_t userLogin(int32_t user_id, const std::string& email, const std::string& pwd); 
    mysqlx::Row getBasicUserDataByID(int32_t user_id);
    mysqlx::Row getBasicUserDataByEmail(const std::string& email);
    mysqlx::Row getBasicGroupData(int32_t group_id);
    mysqlx::RowResult searchUser(int32_t user_id, const std::string& user_name);
    mysqlx::RowResult searchGroup(int32_t group_id, const std::string& group_name);
    mysqlx::RowResult searchUserHistory(int32_t src_user_id, int32_t dst_user_id, const std::string& time_begin, const std::string& time_end); 
    mysqlx::RowResult searchGroupHistory(int32_t src_user_id, int32_t dst_group_id, const std::string& time_begin, const std::string& time_end); // 0为群中所有人的发言
    mysqlx::RowResult searchUnsendMessageFromUser(int32_t unsend_user_id);
    mysqlx::RowResult searchUnsendMessageFromGroup(int32_t unsend_user_id);
    mysqlx::Row getUserStatus(int32_t user_id);
    mysqlx::RowResult getFriendRelation(int32_t user1_id, int32_t user2_id);
    mysqlx::RowResult getFriendRequest(int32_t user_id);
    mysqlx::RowResult getGroupMember(int32_t group_id);
    mysqlx::RowResult getGroupsOfAUser(int32_t user_id);
    
    int32_t userRegister(const std::string& user_name, const std::string& pwd, const std::string& email, const std::string& birthday);
    void addUserHistory(int32_t src_user_id, int32_t dst_user_id, const std::string& send_time, const std::string& content);
    void addGroupHistory(int32_t src_user_id, int32_t dst_group_id, const std::string& send_time, const std::string& content);
    void addUnsendMessageFromUser(int32_t unsend_user_id);
    void addUnsendMessageFromGroup(int32_t unsend_user_id);
    void createGroup(const std::string& group_name, int32_t owner_id, const std::string& group_description);
    void addUserToGroup(int32_t group_id, int32_t user_id);
    void addGroupToUser(int32_t user_id, int32_t group_id);
    void createFriendRelation(int32_t user1_id, int32_t user2_id);
    void createFriendRequest(int32_t user_from, int32_t user_to, const std::string& request_message);

    void updateBasicUserData(int32_t user_id, const std::string& user_name, const std::string& pwd, const std::string& email, const std::string& birthday, 
        const std::string& avatar_filename, const std::string& signature);
    void updateBasicGroupData(int32_t group_id, const std::string& group_name, int32_t owner_id, const std::string& group_description);
    void updateUserStatus(int32_t user_id, const std::string& datetime);

    void deleteGroup(int32_t group_id);
    void removeUserFromGroup(int32_t group_id, int32_t user_id);
    void removeGroupFromUser(int32_t user_id, int32_t group_id);
    void deleteFriendRelation(int32_t ser1_id, int32_t user2_id);
    void deleteFriendRequest(int32_t user_from, int32_t user_to);

private:
    int regsiteredUserCount(); // 可用于注册时返回新用户的id
    bool emailUnique(const std::string& email); 
    int existedGroupCount(); // 创建群聊时返回新群聊的id
    mysqlx::RowResult searchUG(mysqlx::TableSelect& table_select, const std::string& type, int32_t id, const std::string& name);
    mysqlx::RowResult searchHistory(mysqlx::TableSelect& table_select, const std::string& type,
        int32_t src_id, int32_t dst_id, const std::string& time_begin, const std::string& time_end);
    mysqlx::RowResult searchUnsendMessage(mysqlx::TableSelect& table_select, int32_t unsend_user_id);
    void addHistory(mysqlx::Table& table, const std::string& type, 
        int32_t src_id, int32_t dst_id, const std::string& send_time, const std::string& content);

    mysqlx::Session sess;
    mysqlx::Schema sch;
    mysqlx::Table basic_user_data, basic_group_data, user_status, message_to_user, message_to_group, unsend_messgae_from_user, unsend_messgae_from_group,
        friend_relation, friend_request, group_member, users_group;
    mysqlx::TableSelect user_login, registered_user_count, existed_group_count,
        get_basic_user_data, get_basic_group_data,
        search_user_history, search_group_history, search_unsend_messgae_from_user, search_unsend_messgae_from_group,
        get_user_status,
        get_friend_relation, get_friend_request,
        get_group_member, get_groups_of_a_user;

};
    // mysqlx::TableInsert user_register,
    //     add_user_history, add_group_history,
    //     create_group, add_user_to_group, add_group_to_user,
    //     create_friend_relation, create_friend_request;
    // mysqlx::TableUpdate update_basic_user_data, update_basic_group_data,
    //     update_last_response, update_last_visit_of_user, update_last_visit_of_group;
    // mysqlx::TableRemove user_delete,
    //     delete_group, remove_user_from_group, remove_group_from_user,
    //     delete_friend_relation, delete_friend_request;    

#endif