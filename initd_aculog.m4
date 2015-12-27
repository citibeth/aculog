#! /bin/sh
### BEGIN INIT INFO
# Provides: aculog
# Required-Start: $remote_fs $syslog
# Required-Stop: $remote_fs $syslog
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# Short-Description: Aculog
# Description: This file starts and stops Aculog server
# 
### END INIT INFO

changequote(`[', `]')

[ACULOG_EXE]=ACULOG_EXE
[ACULOG_USER]=ACULOG_USER
[ACULOG_PID]=ACULOG_PID

case "$1" in
 start)
   echo 'Starting aculog...'
   # This would be better if we daemonize
   # http://www.thegeekstuff.com/2012/02/c-daemon-process/
   nohup su $[ACULOG_USER] -c $[ACULOG_EXE] 1>/dev/null 2>&1 &
   ;;
 stop)
   echo 'Stopping aculog...'
   kill `cat $[ACULOG_PID]`
   ;;
 restart)
   echo 'Restarting aculog...'
   kill `cat $[ACULOG_PID]`
   sleep 10
   su $[ACULOG_USER] -c $[ACULOG_EXE]
   ;;
 *)
   echo 'Usage: aculog {start|stop|restart}" >&'
   exit 3
   ;;
esac
