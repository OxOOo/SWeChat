#ifndef CHAT_COMMON_H
#define CHAT_COMMON_H

#define SERVER_DATABASE_PATH "database"
#define SERVER_ADDRESS "0.0.0.0"
#define SERVER_PORT 8888

#define COMMAND_ERROR "error" // {msg: string}
#define COMMAND_MSG "msg" // {msg: string}

#define COMMAND_LOGIN "login" // {username: string, password: string}
#define COMMAND_LOGIN_RES "login_rst" // {success: boolean, msg: string}
#define COMMAND_REGISTER "register" // {username: string, password: string}
#define COMMAND_REGISTER_RES "register_res" // {success: boolean, msg: string}

#define COMMAND_FLASH "cmd_flash" // 刷新 {}
#define COMMAND_USERS "all_users" // {users: [{username: string, online: boolean}]}
#define COMMAND_FRIENDS "my_friends" // {friends: [{username: string, online: boolean, unread: int}]}

// username 均不是指自己
#define COMMAND_ADD_FRIEND "add_friend" // {username: string}

#define COMMAND_QUERY_MSGS "query_msgs" // {username: string}
#define COMMAND_MSGS "msgs" // {username: string, msgs: [{}]}
#define COMMAND_SEND_MSG "send_msg" // {username: string, msg: string}
#define COMMAND_NEW_MSG "new_msg" // {username: string, id: int, msg: string, datetime: string, sender: string, read: boolean}
#define COMMAND_ACCEPT_MSG "accept_msg" // {username: string, id: int}
#define COMMAND_REJECT_MSG "reject_msg" // {username: string, id: int}

#endif // CHAT_COMMON_H