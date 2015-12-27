# acudump
Dump weather data from AcuRite Internet Bridge

# Instructions

1. Install prerequisites

        sudo apt-get install tcpdump cmake

1. Create new user for this program

        sudo adduser --disabled-login acurite

1. Edit / create a new file in configme, after configme/ubuntu-14.04
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

```
ts=2015-12-27 14:47:06.579203&utc=01151227-19:47:06.579203&id=24C86E064A98&sensor=01948&mt=tower&humidity=A0560&temperature=A021200000&battery=normal&rssi=3
ts=2015-12-27 14:47:08.048149&utc=01151227-19:47:08.048149&id=24C86E064A98&sensor=13427&mt=tower&humidity=A0870&temperature=A014300000&battery=normal&rssi=1
ts=2015-12-27 14:47:09.828367&utc=01151227-19:47:09.828367&id=24C86E064A98&mt=pressure&C1=3FDB&C2=0D70&C3=00D0&C4=01B5&C5=81C4&C6=1873&C7=09C4&A=07&B=19&C=06&D=0B&PR=A8CA&TR=8621
ts=2015-12-27 14:47:16.480396&utc=01151227-19:47:16.480396&id=24C86E064A98&sensor=03203&mt=tower&humidity=A0600&temperature=A019100000&battery=normal&rssi=3
ts=2015-12-27 14:47:22.957555&utc=01151227-19:47:22.957555&id=24C86E064A98&sensor=01948&mt=tower&humidity=A0560&temperature=A021200000&battery=normal&rssi=3
ts=2015-12-27 14:47:33.361315&utc=01151227-19:47:33.361315&id=24C86E064A98&sensor=03203&mt=tower&humidity=A0600&temperature=A019100000&battery=normal&rssi=3
```

1. Go to http://github.com/citibob/acuparse to parse and use the  data you're now receiving!
