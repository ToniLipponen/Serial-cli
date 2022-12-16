#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <mutex>
#include <chrono>

#include <fcntl.h>      // Contains file controls like O_RDWR
#include <errno.h>      // Error integer and strerror() function
#include <termios.h>    // Contains POSIX terminal control definitions
#include <unistd.h>     // write(), read(), close()
using namespace std;

int intToBaudRate(int arg)
{
    switch(arg)
    {
        case 50:        return B50;
        case 75:        return B75;
        case 110:       return B110;
        case 134:       return B134;
        case 150:       return B150;
        case 200:       return B200;
        case 300:       return B300;
        case 600:       return B600;
        case 1200:      return B1200;
        case 1800:      return B1800;
        case 2400:      return B2400;
        case 4800:      return B4800;
        case 9600:      return B9600;
        case 19200:     return B19200;
        case 38400:     return B38400;
        case 57600:     return B57600;
        case 115200:    return B115200;
        case 230400:    return B230400;
        case 460800:    return B460800;
        case 500000:    return B500000;
        case 576000:    return B576000;
        case 921600:    return B921600;
        case 1000000:   return B1000000;
        case 1152000:   return B1152000;
        case 1500000:   return B1500000;
        case 2000000:   return B2000000;
        case 2500000:   return B2500000;
        case 3000000:   return B3000000;
        case 3500000:   return B3500000;
        case 4000000:   return B4000000;
        default: break;
    }

    std::cerr << "Invalid baud rate" << std::endl;
    exit(2);
}

void printHelp()
{
    std::cout << "Usage: 'appname /dev/someSerialDevice 9600'\nWhere 9600 is the baud rate.\n\nYou can omit the baud rate and the application will use 9600 as the default." << std::endl;
}

int main(int argc, char** argv)
{
    int baudRate = B9600;

    if(argc < 2)
    {
        printHelp();
        return 1;
    }
    else if(argc > 2)
    {
        std::stringstream baudSStream(argv[2]);
        baudSStream >> baudRate;
        baudRate = intToBaudRate(baudRate);
    }

    auto device = open(argv[1], O_RDWR | O_NONBLOCK);
    
    if(device == -1)
    {
        std::cerr << "Failed to open device: " << argv[1] << std::endl;
        return 3;
    }

    struct termios tty;

    if(tcgetattr(device, &tty) != 0) {
        return 4;
    }

    cfmakeraw(&tty);
    cfsetispeed(&tty, baudRate);
    cfsetospeed(&tty, baudRate);

    tcsetattr(device, 0, &tty);

    bool shouldExit = false;
    std::mutex mutex;
    
    std::thread readThread([&]()
    {
        std::string content;

        while(!shouldExit)
        {
            content.clear();
            mutex.lock();
            char readBuffer[1 << 10]{0};
            
            int readBytes = 0;
            do
            {
                readBytes = read(device, readBuffer, sizeof(readBuffer));

                if(readBytes < 0)
                {
                    break;
                }
                
                content.append(readBuffer, readBytes);
            }while(readBytes);

            mutex.unlock();

            if(readBytes)
                std::cout << content;

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    std::thread writeThread([&]()
    {
        while(!shouldExit)
        {
            std::string sendBuffer;
            std::getline(std::cin, sendBuffer);

            if(sendBuffer.find("/exit") != sendBuffer.npos)
            {
                shouldExit = true;
                break;
            }
            
            int sent = 0;

            mutex.lock();
            while(sent != sendBuffer.size())
            {
                sent += write(device, sendBuffer.c_str() + sent, sendBuffer.size() - sent);
            }
            mutex.unlock();

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
    
    readThread.join();
    writeThread.join();
    
    if(close(device) == -1)
    {
        std::cerr << "Failed to close device" << std::endl;
    }

    return 0;
}
