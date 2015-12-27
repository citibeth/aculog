# aculog
Log weather data from AcuRite Internet Bridge

# Introduction

The AcuRite Internet Bridge (http://www.acurite.com/aculink-internet-bridge-for-acurite-weather-sensors-09150trx.html) allows one to build an at-home sensor network / weather station.  It continuously monitors the state of the sensors, and forwards the data to AcuRite.com.  Unfortunately, the bridge comes with NO SUPPORT to get the data yourself, without first going through their servers; and the on-line user experience is terrible.

The solution is to log the data yourself on your PC / Rasberry Pi. The software presented here is thanks to George Nincehelser (http://nincehelser.com/ipwx/).  It is a "robustified" version of the software presneted in that location.

# Instructions

1. Use Ubuntu 14.04LTS, or some other Linux system.  If you don't have an always-on server, set up a Rasberry Pi for this purpose (http://nincehelser.com/ipwx).

1. Install a second Ethernet card.  I used a TP_Link TG-3468, and it was plug-and-play with my Ubuntu 14.04 (http://www.amazon.com/TP-LINK-TG-3468-1000Mbps-Gigabit-Express/dp/B003CFATNI).

1. , Bridge the new Ethernet card to your main network (https://help.ubuntu.com/community/NetworkConnectionBridge).  For Ubuntu 14.04, this just consists of the following in /etc/nework/interfaces and then rebooting:

        auto lo
        iface lo inet loopback
        
        iface eth0 inet manual
        
        iface eth1 inet manual
        
        auto br0
        iface br0 inet dhcp
          bridge_ports eth0 eth1

1. Test the bridge network by plugging a laptop into your second Ethernet card and trying to get to the Internet.

1. Put batteries in your sensors, and set their internal switches to A, B and C so that each one is set differently.  See AcuRite instructions for further details.

1. Note the MAC ADDRESS on the bottom of the bridge.  This will be used below in CMake.

1. Power on the Internet Bridge and plug it into your new network card.  Look at AcuRite instructions to ensure the bridge is in NORMAL OPERATION mode (both lights blinking).  Every 15 seconds or so, you should see a light blink on your sensor.

1. Use tcpdump to manually check that data are coming over your new Ethernet card from the Internet Bridge.  You should see something as follows.  Note the "id=" lines, which are keyed on your Mac Address and contain your data!

        sudo apt-get install tcpdump
        > sudo /usr/sbin/tcpdump -tttt -A -n -p -U -i eth1 -s0 tcp dst port 80
        
        tcpdump: WARNING: eth1: no IPv4 address assigned
        tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
        listening on eth1, link-type EN10MB (Ethernet), capture size 65535 bytes
        2015-12-27 15:03:28.901903 IP 192.168.1.251.1571 > 174.143.48.162.80: Flags [S], seq 1718479044, win 400, options [mss 536], length 0
        E..,.U..d.........0..#.Pfm......`.............
        2015-12-27 15:03:28.975841 IP 192.168.1.251.1571 > 174.143.48.162.80: Flags [.], ack 3422502709, win 400, length 0
        E..(.V..d.........0..#.Pfm....?5P.............
        2015-12-27 15:03:28.979221 IP 192.168.1.251.1571 > 174.143.48.162.80: Flags [P.], seq 0:75, ack 1, win 400, length 75
        E..s.W..d..Y......0..#.Pfm....?5P.......POST /messages/ HTTP/1.1
        Content-Type: application/x-www-form-urlencoded
        
        2015-12-27 15:03:28.979535 IP 192.168.1.251.1571 > 174.143.48.162.80: Flags [P.], seq 75:115, ack 1, win 400, length 40
        E..P.X..d..{......0..#.Pfm....?5P...uZ..Host: www.acu-link.com
        Content-Length: 
        2015-12-27 15:03:28.980409 IP 192.168.1.251.1571 > 174.143.48.162.80: Flags [P.], seq 115:140, ack 1, win 400, length 25
        E..A.Y..d.........0..#.Pfm.8..?5P.......97
        Connection: close
        
        
        2015-12-27 15:03:29.051719 IP 192.168.1.251.1571 > 174.143.48.162.80: Flags [P.], seq 140:237, ack 1, win 400, length 97
        E....Z..d..@......0..#.Pfm.Q..?5P....-..id=6BA62486EA42&sensor=01651&mt=tower&humidity=A0560&temperature=A021300000&battery=normal&rssi=3
        2015-12-27 15:03:29.153191 IP 192.168.1.251.1571 > 174.143.48.162.80: Flags [.], ack 293, win 108, length 0
        E..(.[..d.........0..#.Pfm....@YP..l..........
        2015-12-27 15:03:29.153726 IP 192.168.1.251.1571 > 174.143.48.162.80: Flags [.], ack 294, win 108, length 0
        E..(.\..d.........0..#.Pfm....@ZP..l..........
        2015-12-27 15:03:29.156313 IP 192.168.1.251.1571 > 174.143.48.162.80: Flags [F.], seq 237, ack 294, win 400, length 0
        E..(.]..d.........0..#.Pfm....@ZP.............
        2015-12-27 15:03:29.156842 IP 192.168.1.251.1571 > 174.143.48.162.80: Flags [R.], seq 237, ack 294, win 400, length 0
        E..(.^..

1. Install prerequisites

        sudo apt-get install tcpdump cmake

1. Create new user for this program

        sudo adduser --disabled-login acurite

1. Edit / create a new file in configme, after configme/ubuntu-14.04.  Make sure to set ACURITE_INTERNET_BRIDGE_ID to the Mac Address you noted above.

1. Configure CMake

        mkdir build
        cd build
        ../configme/ubuntu-14.04 ..
        
1. Build:

        make

1. Install:

        sudo make install

1. Start it going, now and on startup

        sudo update-rc.d aculog defaults
        sudo /etc/init.d/aculog start

1. Verify that it's running:

        acurite@anke:~$ ps aux | grep aculog
        root     27207  0.0  0.0  74948  3924 pts/24   S    14:44   0:00 su acurite -c /export/home/acurite/bin/aculog
        acurite  27215  0.0  0.0   4336  1408 ?        Ss   14:44   0:00 /export/home/acurite/bin/aculog
        acurite  27273  0.0  0.0  15944  2272 pts/5    S+   14:45   0:00 grep --color=auto aculog

1. Watch output appear

        ls ~acurite/data
        tail -f ~acurite/data/acurite-20151227.text

 Sample output:

        ts=2015-12-27 14:47:06.579203&utc=01151227-19:47:06.579203&id=24C86E064A98&sensor=01948&mt=tower&humidity=A0560&temperature=A021200000&battery=normal&rssi=3
        ts=2015-12-27 14:47:08.048149&utc=01151227-19:47:08.048149&id=24C86E064A98&sensor=13427&mt=tower&humidity=A0870&temperature=A014300000&battery=normal&rssi=1
        ts=2015-12-27 14:47:09.828367&utc=01151227-19:47:09.828367&id=24C86E064A98&mt=pressure&C1=3FDB&C2=0D70&C3=00D0&C4=01B5&C5=81C4&C6=1873&C7=09C4&A=07&B=19&C=06&D=0B&PR=A8CA&TR=8621
        ts=2015-12-27 14:47:16.480396&utc=01151227-19:47:16.480396&id=24C86E064A98&sensor=03203&mt=tower&humidity=A0600&temperature=A019100000&battery=normal&rssi=3
        ts=2015-12-27 14:47:22.957555&utc=01151227-19:47:22.957555&id=24C86E064A98&sensor=01948&mt=tower&humidity=A0560&temperature=A021200000&battery=normal&rssi=3
        ts=2015-12-27 14:47:33.361315&utc=01151227-19:47:33.361315&id=24C86E064A98&sensor=03203&mt=tower&humidity=A0600&temperature=A019100000&battery=normal&rssi=3

 Note the rssi field at the end of the line.  This is apparently an indicator of the signal strenght, with 4 being the strongest.  Use this line to help with placement of your sensors and Internet Bridge.

1. Reboot, and verify that data collection continues without intervention upon start-up.

1. Go to http://github.com/citibob/acuparse to parse and use the  data you're now receiving!
