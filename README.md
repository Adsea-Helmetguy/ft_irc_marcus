# ft_irc

## Instructions to run program

**Server - IMPT MUST START THE SERVER BEFORE YOU CONNECT**
1. ./ircserv port password

**Client**
1. irssi
2. /connect localhost port password

**Joining a Channel**
1. /join <#channelname>

**Leaving a Channel**
1. /part <#channelname> <optional reason>

**Sending a message**
2. just type message in the channel

**How to kill port if needed**
1. sudo lsof -i :<PORT> (Get PID from this command)
2. sudo kill -9 <PID>

## Phase 1: Understanding the Project Scope
**Read and Understand the IRC Protocol**

**Research how IRC works (server-client communication, commands, responses).**
- [ ] Study [RFC 1459] (https://www.rfc-editor.org/rfc/rfc1459.html) (IRC protocol specification).
- [x] Choose a reference IRC client (e.g., WeeChat, HexChat).
- [x] Review the Project Requirements

**Identify the mandatory features.**
- [ ] Understand constraints (C++98, no external libraries, non-blocking I/O).
- [ ] Understand the allowed system calls (socket, poll, etc.).

## Phase 2: Setting Up the Project
**Initialize the Project**

- [x] Create a Makefile with the required rules (all, clean, fclean, re).
- [x] Set up the basic project structure
- [x] Implement basic TCP server using socket, bind, listen, accept.
- [x] Make the server accept client connections on a given port

## Phase 3: Core Server Implementation
**Implement Non-Blocking I/O**

- [x] Use poll() (or select(), epoll(), kqueue()) to handle multiple clients.
- [ ] Make sure the server doesn't hang on read/write operations.

**Handle Client Connections**
- [x] Store connected clients in a list.
- [x] Implement authentication using a password.
- [ ] Implement Basic IRC Commands

1. NICK: Set the client's nickname.
2. USER: Register the username.
3. JOIN: Join a channel.
4. PRIVMSG: Send messages.
5. MSG: Send a private message to a user.
5. PART: Leave a channel.

Handle Channel Management
- [x] Store channels and their members.
- [x] Implement message broadcasting within a channel.
- [ ] Implement Operator Privileges

1. KICK: Remove a user from a channel.
2. INVITE: Invite a user to a private channel.
3. TOPIC: Set the channel topic.
4. MODE: Manage channel modes (invite-only, password, operator, user limit).

### Handle Disconnections and Errors

- [ ] Ensure clients can disconnect properly.
- [ ] Handle partial messages and packet reassembly.
- [ ] Detect and manage invalid commands.

## Phase 4: Testing and Debugging
**Test with an IRC Client**

- [ ] Connect using a real IRC client (WeeChat, HexChat).
- [ ] Verify commands work as expected.

**Edge Case Testing**

- [ ] Handle clients sending incomplete/invalid data.
- [ ] Test multiple clients joining and messaging in a channel.
- [ ] Ensure graceful handling of disconnects.

**Resource Management**

- [ ] Check for memory leaks.
- [ ] Ensure proper cleanup of resources.
