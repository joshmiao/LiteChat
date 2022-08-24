#pragma once

constexpr int USER_LOGIN_PWD = 0;
constexpr int REGISTERED_USER_COUNT = 0;
constexpr int EXISTED_GROUP_ID_MAX = 0;

constexpr int BASIC_USER_DATA_USER_ID = 0;
constexpr int BASIC_USER_DATA_USER_NAME = 1;
constexpr int BASIC_USER_DATA_EMAIL = 2;
constexpr int BASIC_USER_DATA_BIRTHDAY = 3;
constexpr int BASIC_USER_DATA_AVATAR_FILENAME = 4;
constexpr int BASIC_USER_DATA_SIGNATURE = 5;

constexpr int BASIC_GROUP_DATA_GROUP_ID = 0;
constexpr int BASIC_GROUP_DATA_GROUP_NAME = 1;
constexpr int BASIC_GROUP_DATA_OWNER = 2;
constexpr int BASIC_GROUP_DATA_GROUP_DESCRIPTION = 3;

constexpr int USER_HISTORY_SEND_TIME = 0;
constexpr int USER_HISTORY_SRC_USER_ID = 1;
constexpr int USER_HISTORY_DST_USER_ID = 2;
constexpr int USER_HISTORY_CONTENT = 3;

constexpr int GROUP_HISTORY_SEND_TIME = 0;
constexpr int GROUP_HISTORY_SRC_USER_ID = 1;
constexpr int GROUP_HISTORY_DST_GROUP_ID = 2;
constexpr int GROUP_HISTORY_CONTENT = 3;

constexpr int USER_UNSEND_MESSAGE_SEND_TIME = 0;
constexpr int USER_UNSEND_MESSAGE_SRC_USER_ID = 1;
constexpr int USER_UNSEND_MESSAGE_CONTENT = 2;

constexpr int GROUP_UNSEND_MESSAGE_SEND_TIME = 0;
constexpr int GROUP_UNSEND_MESSAGE_SRC_USER_ID = 1;
constexpr int GROUP_UNSEND_MESSAGE_DST_GROUP_ID = 2;
constexpr int GROUP_UNSEND_MESSAGE_CONTENT = 3;

constexpr int USER_STATUS_IS_ONLINE = 0;
constexpr int USER_STATUS_HANDLE = 1;
constexpr int USER_STATUS_TOKEN = 2;

constexpr int FRIEND_RELATION_USER1 = 0;
constexpr int FRIEND_RELATION_USER2 = 1;
constexpr int FRIEND_REQUEST_USER_FROM = 0;
constexpr int FRIEND_REQUEST_USER_TO = 1;
constexpr int FRIEND_REQUEST_REQUEST_MESSAGE = 2;

constexpr int GROUP_MEMBER_GROUP_ID = 0;
constexpr int GROUP_MEMBER_USER_ID = 1;
constexpr int GROUP_REQUEST_USER_ID = 0;
constexpr int GROUP_REQUEST_GROUP_ID = 1;
constexpr int GROUP_REQUEST_REQUEST_MESSAGE = 2;
