#! /bin/sh
### BEGIN INIT INFO
# Provides: tomcat
# Required-Start: $remote_fs $syslog
# Required-Stop: $remote_fs $syslog
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# Short-Description: Tomcat
# Description: This file starts and stops Tomcat server
# 
### END INIT INFO

`ACUDUMP_EXE'=ACUDUMP_EXE
`ACUDUMP_USER'=ACUDUMP_USER

case "$1" in
 start)
   echo 'Starting acudump...'
   nohup su $`ACUDUMP_USER' -c $`ACUDUMP_EXE' 1>/dev/null &
   ;;
 stop)
   echo 'Stopping acudump...'
   kill `cat acudump.pid`
   ;;
 restart)
   echo 'Restarting acudump...'
   kill `cat acudump.pid'
   sleep 10
   su $`ACUDUMP_USER' -c $`ACUDUMP_EXE'
   ;;
 *)
   echo "Usage: acudump {start|stop|restart}" >&2
   exit 3
   ;;
esac
