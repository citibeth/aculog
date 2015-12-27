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
