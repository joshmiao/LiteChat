DROP SCHEMA LiteChat;
CREATE SCHEMA LiteChat;
USE LiteChat;

CREATE TABLE basic_user_data (
user_id INT PRIMARY KEY,
user_name VARCHAR(32) NOT NULL,
pwd CHAR(32) NOT NULL,
email VARCHAR(32) NOT NULL UNIQUE,
birthday DATE NOT NULL,
avatar_filename VARCHAR(32) DEFAULT("default.jpg"),
signature VARCHAR(128) DEFAULT("")
); 

CREATE TABLE basic_group_data (
group_id INT PRIMARY KEY,
owner_id INT NOT NULL,
group_name VARCHAR(64) NOT NULL UNIQUE,
group_description VARCHAR(256) DEFAULT(""),
CONSTRAINT OWNER_REGISTERED foreign key (owner_id) references basic_user_data(user_id)
);

CREATE TABLE user_status ( -- 储存客户端最后一次回应的时间。当再次上线时，查询send_time > last_response的消息发送给客户端
user_id INT PRIMARY KEY,
is_online BOOL DEFAULT(FALSE),
handle INT DEFAULT(NULL),
token CHAR(32) UNIQUE DEFAULT(NULL),
-- 其它数据（ip等）？
CONSTRAINT USER_REGISTERED_1 foreign key (user_id) references basic_user_data(user_id)
);

CREATE TABLE message_to_user (
send_time DATETIME(3), -- 精确到ms
src_user_id INT,
dst_user_id INT,
content VARCHAR(2048) NOT NULL,
CONSTRAINT IDENTIFIER_MESSAGE_TO_USER PRIMARY KEY(send_time, src_user_id, dst_user_id),
CONSTRAINT SRC_USER_REGISTERED_1 foreign key (src_user_id) references basic_user_data(user_id),
CONSTRAINT DST_USER_REGISTERED_1 foreign key (dst_user_id) references basic_user_data(user_id)
);

CREATE TABLE message_to_group (
send_time DATETIME(3), -- 精确到ms
src_user_id INT,
dst_group_id INT,
content VARCHAR(2048) NOT NULL,
CONSTRAINT IDENTIFIER_MESSAGE_TO_USER PRIMARY KEY(send_time, src_user_id, dst_group_id),
CONSTRAINT SRC_USER_REGISTERED_2 foreign key (src_user_id) references basic_user_data(user_id),
CONSTRAINT DST_GROUP_CREATED_2 foreign key (dst_group_id) references basic_group_data(group_id)
);

CREATE TABLE unsend_message_from_user (
send_time DATETIME(3), -- 精确到ms
unsend_user_id INT,
src_user_id INT,
content VARCHAR(2048) NOT NULL,
CONSTRAINT IDENTIFIER_UNSEND_MESSGAE_FROM_USER PRIMARY KEY(send_time, unsend_user_id, src_user_id),
CONSTRAINT SRC_USER_REGISTERED_3 foreign key (src_user_id) references basic_user_data(user_id),
CONSTRAINT UNSEND_USER_REGISTERED_3 foreign key (unsend_user_id) references basic_user_data(user_id)
);
CREATE INDEX unsend_users_pair ON unsend_message_from_user(unsend_user_id ASC, send_time ASC);

CREATE TABLE unsend_message_from_group (
send_time DATETIME(3), -- 精确到ms
unsend_user_id INT,
src_user_id INT,
dst_group_id INT,
content VARCHAR(2048) NOT NULL,
CONSTRAINT IDENTIFIER_UNSEND_MESSGAE_FROM_GROUP PRIMARY KEY(send_time, unsend_user_id, src_user_id, dst_group_id),
CONSTRAINT UNSEND_USER_REGISTERED_4 foreign key (unsend_user_id) references basic_user_data(user_id),
CONSTRAINT SRC_USER_REGISTERED_4 foreign key (src_user_id) references basic_user_data(user_id),
CONSTRAINT DST_USER_REGISTERED_4 foreign key (dst_group_id) references basic_group_data(group_id)
);
CREATE INDEX unsend_user_group_pair ON unsend_message_from_group(unsend_user_id ASC, send_time ASC);

CREATE TABLE friend_relation ( -- 为加快查找速度，可以同时插入两对：(1,2)(2,1)
user1_id INT, 
user2_id INT,
-- 其它属性？（插入两对后，属性只需要描述一个方向；）
CONSTRAINT IDENTIFIER_FRIEND_RELATION PRIMARY KEY(user1_id, user2_id),
CONSTRAINT USER1_REGISTERED foreign key (user1_id) references basic_user_data(user_id),
CONSTRAINT USER2_REGISTERED foreign key (user2_id) references basic_user_data(user_id)
);

CREATE TABLE friend_request ( -- 处理后删除，未处理则考虑一直发送？
user_from INT,
user_to INT,
request_message VARCHAR(128),
CONSTRAINT IDENTIFIER_FRIEND_REQUEST PRIMARY KEY(user_from, user_to),
CONSTRAINT USER_FROM_REGISTERED foreign key (user_from) references basic_user_data(user_id),
CONSTRAINT USER_TO_REGISTERED foreign key (user_to) references basic_user_data(user_id)
);

CREATE TABLE group_member (
group_id INT,
user_id INT,
-- 其它属性？
CONSTRAINT IDENTIFIER_GROUP_MEMBER PRIMARY KEY(group_id, user_id),
CONSTRAINT GROUP_CREATED_5 foreign key (group_id) references basic_group_data(group_id),
CONSTRAINT MEMBER_REGISTERED foreign key (user_id) references basic_user_data(user_id)
);
CREATE INDEX GROUPS_OF_A_USER ON group_member(user_id ASC);

CREATE TABLE group_request ( -- 处理后删除，未处理则考虑一直发送？
user_id INT,
group_id INT,
request_message VARCHAR(128),
CONSTRAINT IDENTIFIER_REQUEST_GROUP_REGISTERED PRIMARY KEY(user_id, group_id),
CONSTRAINT REQUEST_USER_REGISTERED foreign key (user_id) references basic_user_data(user_id),
CONSTRAINT REQUEST_GROUP_REGISTERED foreign key (group_id) references basic_group_data(group_id)
);