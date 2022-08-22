#ifndef H_DB_OPERATIONS
#define H_DB_OPERATIONS

#include <string>
#include <mysqlx/xdevapi.h>

constexpr int32_t USER_ID_BEGIN = 10000;
constexpr int32_t GROUP_ID_BEGIN = 10000;
typedef int32_t ID;

class LiteChatDatabaseAccess
{
public:
    LiteChatDatabaseAccess(const std::string& url);

    ID userLogin(ID user_id, const std::string& email, const std::string& pwd); 
    mysqlx::Row getBasicUserDataByID(ID user_id);
    mysqlx::Row getBasicUserDataByEmail(const std::string& email);
    mysqlx::Row getBasicGroupData(ID group_id);
    mysqlx::RowResult searchUser(ID user_id, const std::string& user_name);
    mysqlx::RowResult searchGroup(ID group_id, const std::string& group_name);
    mysqlx::RowResult searchUserHistory(ID src_user_id, ID dst_user_id, const std::string& time_begin, const std::string& time_end); 
    mysqlx::RowResult searchGroupHistory(ID src_user_id, ID dst_group_id, const std::string& time_begin, const std::string& time_end); // 0为群中所有人的发言
    mysqlx::RowResult searchUnsendMessageFromUser(ID unsend_user_id);
    mysqlx::RowResult searchUnsendMessageFromGroup(ID unsend_user_id);
    mysqlx::Row getUserStatus(ID user_id);
    mysqlx::RowResult getFriendRelation(ID user1_id, ID user2_id);
    mysqlx::RowResult getFriendRequest(ID user_id);
    mysqlx::RowResult getGroupMember(ID group_id);
    mysqlx::RowResult getGroupsOfAUser(ID user_id);
    
    ID userRegister(const std::string& user_name, const std::string& pwd, const std::string& email, const std::string& birthday);
    void addUserHistory(const std::string& send_time, ID src_user_id, ID dst_user_id, const std::string& content);
    void addGroupHistory(const std::string& send_time, ID src_user_id, ID dst_group_id, const std::string& content);
    void addUnsendMessageFromUser(const std::string& send_time, ID unsend_user_id, ID src_user_id, const std::string& content);
    void addUnsendMessageFromGroup(const std::string& send_time, ID unsend_group_id, ID src_user_id, ID dst_group_id, const std::string& content);
    ID createGroup(const std::string& group_name, ID owner_id, const std::string& group_description);
    void addUserToGroup(ID group_id, ID user_id);
    void addGroupToUser(ID user_id, ID group_id);
    void createFriendRelation(ID user1_id, ID user2_id);
    void createFriendRequest(ID user_from, ID user_to, const std::string& request_message);

    mysqlx::TableUpdate updateBasicUserData();
    mysqlx::TableUpdate updateBasicGroupData();
    mysqlx::TableUpdate updateUserStatus();
    void updateUserStatusWhenLogin(ID user_id, int handle);

    void deleteGroup(ID group_id);
    void deleteUnsendMessageFromUser(ID unsend_user_id);
    void deleteUnsendMessageFromGroup(ID unsend_user_id);
    void removeUserFromGroup(ID group_id, ID user_id);
    void removeGroupFromUser(ID user_id, ID group_id);
    void deleteFriendRelation(ID ser1_id, ID user2_id);
    void deleteFriendRequest(ID user_from, ID user_to);

private:
    std::string sqlString(std::string s);
    int regsiteredUserCount();
    bool emailUnique(const std::string& email); 
    int existGroupCount();
    bool groupNameUnique(const std::string& email);
    mysqlx::RowResult searchUG(mysqlx::TableSelect& table_select, const std::string& type, ID id, const std::string& name);
    mysqlx::RowResult searchHistory(mysqlx::TableSelect& table_select, const std::string& type,
        ID src_id, ID dst_id, const std::string& time_begin, const std::string& time_end);
    mysqlx::RowResult searchUnsendMessage(mysqlx::TableSelect& table_select, ID unsend_user_id);
    void addHistory(mysqlx::Table& table, const std::string& type, 
        const std::string& send_time, ID src_id, ID dst_id, const std::string& content);

    mysqlx::Session sess;
    mysqlx::Schema sch;
    mysqlx::Table basic_user_data, basic_group_data, user_status, message_to_user, message_to_group, unsend_messgae_from_user, unsend_messgae_from_group,
        friend_relation, friend_request, group_member, groups_of_a_user;
    mysqlx::TableSelect user_login, registered_user_count, existed_group_count,
        get_basic_user_data, get_basic_group_data,
        search_user_history, search_group_history, search_unsend_messgae_from_user, search_unsend_messgae_from_group,
        get_user_status,
        get_friend_relation, get_friend_request,
        get_group_member, get_groups_of_a_user;
    mysqlx::TableRemove delete_group, 
        delete_unsend_messgae_from_user, delete_unsend_messgae_from_group,
        remove_user_from_group, remove_group_from_user,
        delete_friend_relation, delete_friend_request;
};
        

#endif