cmake_minimum_required(VERSION 2.8)

project(aculog C)
#list (APPEND PATH "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_C_STANDARD 99)

# --------------- Determine / Validate Options

# ------- TCPDUMP_EXE
find_program(TCPDUMP_EXE tcpdump)

if (NOT TCPDUMP_EXE)
	message(FATAL_ERROR "Cannot find tcpdump executable.  Try: sudo apt-get install tcpdump")
endif()

# -------- ACULOG_USER
if (NOT ACULOG_USER)
	message(FATAL_ERROR "ACULOG_USER must be set.  Try cmake ... -DACULOG_USER=acurite ...")
endif()

# ---------- ACURITE_INTERNET_BRIDGE_ID
if (NOT ACURITE_INTERNET_BRIDGE_ID)
	message(FATAL_ERROR "You must supply the variable ACURITE_INTERNET_BRIDGE_ID as a -D option to cmake.")
endif()

# ------------ DATA_DIR
set(DATA_DIR ${CMAKE_INSTALL_PREFIX}/data
	CACHE string "Directory where aculog will store its data.")

# ------------- ACULOG_PID
set(ACULOG_PID ${CMAKE_INSTALL_PREFIX}/aculog.pid
	CACHE string "PID file for aculog daemon.")

# -------------------------------------------------

message(TCPDUMP_EXE=${TCPDUMP_EXE})
message(ACULOG_USER=${ACULOG_USER})
message(ACURITE_INTERNET_BRIDGE_ID=${ACURITE_INTERNET_BRIDGE_ID})
message(DATA_DIR=${DATA_DIR})
message(ACULOG_PID=${ACULOG_PID})

add_definitions(
	-DACURITE_INTERNET_BRIDGE_ID="${ACURITE_INTERNET_BRIDGE_ID}"
	-DTCPDUMP_EXE="${TCPDUMP_EXE}"
	-DDATA_DIR="${DATA_DIR}"
	-DACULOG_PID="${ACULOG_PID}"
)

# -----------------------------------------------------

# --------- Acudump executable
add_executable(aculog aculog.c)

install(TARGETS
    aculog
    RUNTIME DESTINATION bin
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID
)

# ---------- init.d/aculog daemon start/stop
add_custom_target(initd_aculog
	ALL
	COMMAND ${PROJECT_SOURCE_DIR}/cmake/run_m4 initd_aculog -DACULOG_EXE=${CMAKE_INSTALL_PREFIX}/bin/aculog -DACULOG_USER=${ACULOG_USER} -DACULOG_PID=${ACULOG_PID} ${CMAKE_CURRENT_SOURCE_DIR}/initd_aculog.m4
	SOURCES initd_aculog.m4
		VERBATIM)

install(PROGRAMS
    ${CMAKE_BINARY_DIR}/initd_aculog
    DESTINATION /etc/init.d
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	RENAME aculog)

