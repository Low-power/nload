/***************************************************************************
                             devreader-linux-sys.h
                             ---------------------
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

#ifndef DEVREADER_LINUX_SYSFS_IB_H
#define DEVREADER_LINUX_SYSFB_IB_H

#include "devreader.h"

#include <string>
#include <list>

class LinuxSysFsInfiniBandDevReader : public DevReader
{
    public:
        LinuxSysFsInfiniBandDevReader(const std::string& deviceName);
        virtual ~LinuxSysFsInfiniBandDevReader();

        static bool isAvailable();
        static std::list<std::string> findAllDevices();
        
    protected:
        void readFromDevice(DataFrame& dataFrame);
};

#endif

