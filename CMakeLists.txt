cmake_minimum_required(VERSION 2.8)

project(acudump C)
#list (APPEND PATH "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_C_STANDARD 99)

# --------------- Determine / Validate Options

# ------- TCPDUMP_EXE
find_program(TCPDUMP_EXE tcpdump)
message(TCPDUMP_EXE=${TCPDUMP_EXE})

if (NOT TCPDUMP_EXE)
	message(FATAL_ERROR "Cannot find tcpdump executable.  Try: sudo apt-get install tcpdump")
endif()

# -------- ACUDUMP_USER
if (NOT ACUDUMP_USER)
	message(FATAL_ERROR "ACUDUMP_USER must be set.  Try cmake ... -DACUDUMP_USER=acurite ...")
endif()

# ---------- ACURITE_INTERNET_BRIDGE_ID
if (NOT ACURITE_INTERNET_BRIDGE_ID)
	message(FATAL_ERROR "You must supply the variable ACURITE_INTERNET_BRIDGE_ID as a -D option to cmake.")
endif()

# ------------ DATA_DIR
set(DATA_DIR ${CMAKE_INSTALL_PREFIX}/data
	CACHE string "Directory where acudump will store its data.")

# ------------- PID_FILE
set(PID_FILE ${CMAKE_INSTALL_PREFIX}/acudump.pid
	CACHE string "PID file for acudump daemon.")

# -------------------------------------------------

message(TCPDUMP_EXE=${TCPDUMP_EXE})
message(ACUDUMP_USER=${ACUDUMP_USER})
message(ACURITE_INTERNET_BRIDGE_ID=${ACURITE_INTERNET_BRIDGE_ID})
message(DATA_DIR=${DATA_DIR})
message(PID_FILE=${PID_FILE})

add_definitions(
	-DACURITE_INTERNET_BRIDGE_ID="${ACURITE_INTERNET_BRIDGE_ID}"
	-DTCPDUMP_EXE="${TCPDUMP_EXE}"
	-DDATA_DIR="${DATA_DIR}"
	-DPID_FILE="${PID_FILE}"
)

# -----------------------------------------------------

# --------- Acudump executable
add_executable(acudump acudump.c)

install(TARGETS
    acudump
    RUNTIME DESTINATION bin
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID
)

# ---------- init.d/acudump daemon start/stop
add_custom_target(initd_acudump
	ALL
	COMMAND ${PROJECT_SOURCE_DIR}/cmake/run_m4 initd_acudump -DACUDUMP_EXE=${CMAKE_INSTALL_PREFIX}/bin/acudump -DACUDUMP_USER=${ACUDUMP_USER} ${CMAKE_CURRENT_SOURCE_DIR}/initd_acudump.m4
	SOURCES initd_acudump.m4
		VERBATIM)

install(PROGRAMS
    ${CMAKE_BINARY_DIR}/initd_acudump
    DESTINATION /etc/init.d
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	RENAME acudump)

