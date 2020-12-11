/***************************************************************************
                             devreaderfactory.cpp
                             --------------------
    begin                : Fri Nov 16 2007
    copyright            : (C) 2007 - 2012 by Roland Riegel
    email                : feedback@roland-riegel.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ibdevreaderfactory.h"

#include "config.h"

#include "devreader.h"
#include "devreader-linux-sysfs-ib.h"

#include <string>
#include <list>
#include <map>

using namespace std;

map<string, DevReader*> InfiniBandDevReaderFactory::m_devReaders;
InfiniBandDevReaderFactory InfiniBandDevReaderFactory::m_instance;

InfiniBandDevReaderFactory::InfiniBandDevReaderFactory()
{
}

InfiniBandDevReaderFactory::~InfiniBandDevReaderFactory()
{
    for(map<string, DevReader*>::const_iterator it = m_devReaders.begin(); it != m_devReaders.end(); ++it)
        delete it->second;

    m_devReaders.clear();
}

int InfiniBandDevReaderFactory::findAllDevices()
{
#if defined HAVE_BSD
#elif defined HAVE_LINUX
    list<string> interfaceNames = LinuxSysFsInfiniBandDevReader::findAllDevices();
#elif defined HAVE_SOLARIS
#endif

    map<string, DevReader*>::iterator devReaderIt = m_devReaders.begin();
    while(devReaderIt != m_devReaders.end())
    {
        list<string>::iterator interfaceIt = interfaceNames.begin();
        list<string>::iterator interfaceItEnd = interfaceNames.end();
        
        while(*interfaceIt != devReaderIt->first && interfaceIt != interfaceItEnd)
            ++interfaceIt;

        // delete all devices which disappeared
        if(interfaceIt == interfaceItEnd)
        {
            delete devReaderIt->second;
            m_devReaders.erase(devReaderIt++);
        }
        // delete all entries in the interface name list which we know of already
        else
        {
            interfaceNames.erase(interfaceIt);
            devReaderIt++;
        }
    }
    
    // the interface name list now contains only devices which just appeared in the system
    for(list<string>::const_iterator it = interfaceNames.begin(); it != interfaceNames.end(); ++it)
    {
        DevReader* newReader = createDevReader(*it);
        if(newReader)
            m_devReaders[*it] = newReader;
    }
    
    return m_devReaders.size();
}

int InfiniBandDevReaderFactory::getDeviceCount()
{
    return m_devReaders.size();
}

const map<string, DevReader*>& InfiniBandDevReaderFactory::getAllDevReaders()
{
    return m_devReaders;
}

DevReader *InfiniBandDevReaderFactory::createDevReader(const string &deviceName)
{
    DevReader* reader = 0;
    
#if defined HAVE_LINUX
    if(LinuxSysFsInfiniBandDevReader::isAvailable())
        reader = new LinuxSysFsInfiniBandDevReader(deviceName);
#endif

    return reader;
}

