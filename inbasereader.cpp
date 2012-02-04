#include "logreader.h"
#include <iostream>
#include "inotify-cxx.h"
#include <unistd.h>
#include <vector>

LogReader::InBaseReader::InBaseReader(const char *pathToFile)
{
    filePath = pathToFile;
}

void LogReader::InBaseReader::open()
{
    std::cout << "Open " << filepath();
    unsigned int count = 0; //number of trying
    unsigned int counts = 10; //all trying
    file.open(filePath.c_str());
    while((!file.is_open())&&(count < counts))
    {
        std::cout << ".";
        file.open(filePath.c_str());
        count++;
        if(count == 10) std::cout << std::endl << "Exit for timeout" << std::endl;
    }
    std::cout << std::endl;
}


void LogReader::InBaseReader::parse(const std::string notParsed) const
{
    std::string sep(" ");
    LogReader::StringList lecs = LogReader::split(sep, notParsed, false);
}

void LogReader::InBaseReader::watch()
{
    try{
        InotifyWatch watcher(filepath(), IN_MODIFY|IN_MOVE_SELF);
        Inotify inotifyFile;
        size_t count;
        inotifyFile.Add(&watcher);
        for(;;)
        {
            std::string line;
            std::string eventMask;
            inotifyFile.WaitForEvents();
            count = inotifyFile.GetEventCount();
            while(count != 0)
            {
                InotifyEvent event;
                bool got_event = inotifyFile.GetEvent(&event);
                if(got_event)
                {
                    event.DumpTypes(eventMask);
                    if(eventMask=="IN_MOVE_SELF")
                    {
                        std::cout << "Logs rotated." << std::endl;
                        file.close();
                        open();
                        inotifyFile.Remove(watcher);
                        InotifyWatch watcher(filepath(), IN_MODIFY|IN_MOVE_SELF);
                        inotifyFile.Add(&watcher);
                    }
                    if(eventMask=="IN_MODIFY")
                    {
                        while (!file.eof())
                        {
                            getline(file,line);
                            if(!line.empty())
                            {
                                parse(line);
                            }
                        }
                        file.clear();
                        break;
                    }
                }
                count--;
            }
        }
    }
    catch (InotifyException& e)
    {
        throw LogReader::InBaseReaderError(filePath.c_str(), (e.GetMessage()).c_str());
    }
}
