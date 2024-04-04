# TITLE

## Recommended priorities

- SyncedVarmapManager* (highest)
- NetworkingManager*
- LobbyBackendManager*
- LobbyFrontendManager* (user-provided)
- AuthorizationManager*

- GameplayManager (user-provided)
- InputSyncManager
- Various gameplay objects (user-provided)

- WindowManager (lowest)

## Breakdown by event

### PRE_UPDATE
- NetworkingManager prepares itself to record telemetry
- USER WINDOW

### BEGIN_UPDATE
- NetworkingManager receives messages, calls handlers
- LobbyBackendManager ... ???
- LobbyFrontendManager ... ???
- AuthorizationManager ... ???
- USER WINDOW

### UPDATE_1
- LobbyFrontendManager ... ???
- << game objects delete selves >>
- USER WINDOW

### UPDATE_2
- USER WINDOW

### END_UPDATE
- SyncedVarmapManager composes messages
- NetworkingManager sends messages
- USER WINDOW

### POST_UPDATE
- LobbyBackendManager ... ???
- USER WINDOW

### PRE_DRAW
- USER WINDOW
- WindowManager clears MRT (main render texture)

### DRAW_1
- USER WINDOW

### DRAW_2
- USER WINDOW
- WindowManager clears the window, draws MRT in it

### DRAW_GUI
- USER WINDOW

### POST_DRAW
- USER WINDOW

### DISPLAY
- USER WINDOW
- WindowManager displays window
