//
// Created by Teddy BLANCO on 2023-03-17.
//

#ifndef MARTIN_HPP
# define MARTIN_HPP

# define LISTEN_BUFFER_SIZE 

//EXCEPTION ERR MESSAGES (SERVEX = SERVER EXCEPTION) -> should stop server
# define SERVEX_SOCKET_FAIL "Error: ServerBooting: socket function fail"
# define SERVEX_SETSOCKOPT_FAIL "Error: ServerBooting: setsockopt function fail"
# define SERVEX_BIND_FAIL "Error: ServerBooting: bind function fail"
# define SERVEX_LISTEN_FAIL "Error: ServerBooting: listen function fail"
# define SERVEX_ACCEPT_FAIL "Error: addNewClient: accept function fail"
# define SERVEX_POLL_FAIL "Error: addNewClient: poll function fail"
# define SERVEX_INSERT_FAIL "Error: indexInfoIt: map.insert(): map.insert fail in _indexInfo"
# define SERVEX_FIND_FAIL "Error: indexInfoIt: map.find(): index not found in _indexInfo"


//SERVER ERROR MESSAGES (SERR = SERVER ERROR) -> should never happen
# define SERR_POLLINDEXSIGNAL "Server error: poll trigger but index signal not found"

#endif
