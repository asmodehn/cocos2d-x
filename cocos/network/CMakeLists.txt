if(WIN32)
  set(PLATFORM_SRC
    WebSocket.cpp
  )

  set(PLATFORM_LINK
    websockets
  )
elseif(NOT APPLE AND UNIX AND NOT CMAKE_TOOLCHAIN_FILE) #assumed linux
  set(PLATFORM_SRC
    WebSocket.cpp
  )

  set(PLATFORM_LINK
    websockets
    ssl
    crypto
  )
elseif(CMAKE_TOOLCHAIN_FILE) #assumed linux
  set(PLATFORM_SRC
    WebSocket.cpp
  )

  set(PLATFORM_LINK
    websockets
  )
endif()

set(NETWORK_SRC
  HttpClient.cpp
  SocketIO.cpp
  ${PLATFORM_SRC}
)

add_library(network STATIC
  ${NETWORK_SRC}
)

target_link_libraries(network
  curl
  ${PLATFORM_LINK}
)

set_target_properties(network
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

