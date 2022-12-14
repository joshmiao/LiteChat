#ifndef REQUEST_TYPE_H
#define REQUEST_TYPE_H

#define _LOGIN 1000
#define _REGISTER 1001
#define _PRIVATE_MESSAGE 1002
#define _GROUP_MESSAGE 1003
#define _GET_FRIENDS 1004
#define _GET_GROUPS 1005
#define _GET_HISTORY_PRIVATE 1006
#define _GET_HISTORY_GROUP 1007
#define _SEARCH_USER 1008
#define _ADD_FRIEND 1009
#define _GET_FRIEND_REQUEST 1010
#define _ACCEPT_FRIEND 1011
#define _DELETE_FRIEND 1012
#define _LOGOUT 1013
#define _CREATE_GROUP 1014
#define _SEARCH_GROUP 1015
#define _ADD_GROUP 1016
#define _GET_MEMBER_REQUEST 1017
#define _ACCEPT_MEMBER 1018
#define _DELETE_MEMBER 1019
#define _INVITE_MEMBER 1020
#define _DELETE_GROUP 1021
#define _GET_GROUP_MEMBERS 1023
#define _ALIVE 999

#define _UNREAD_PRIVATE_MESSAGE 2001
#define _UNREAD_GROUP_MESSAGE 2002

#define _SEND_PRIVATE_FILE

#define _ERROR -1
#define _REQUEST -2
#define _RECEIVE -3
#define _TOKEN -4

#endif // REQUEST_TYPE_H
