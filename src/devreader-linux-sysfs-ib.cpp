/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

#include "devreader-linux-sysfs-ib.h"

#include <fstream>
#include <string>
#include <list>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

LinuxSysFsInfiniBandDevReader::LinuxSysFsInfiniBandDevReader(const string& deviceName)
    : DevReader(deviceName)
{
}

LinuxSysFsInfiniBandDevReader::~LinuxSysFsInfiniBandDevReader()
{
}

bool LinuxSysFsInfiniBandDevReader::isAvailable()
{
    struct stat s;
    return stat("/sys/class/infiniband", &s) == 0 && S_ISDIR(s.st_mode);
}

list<string> LinuxSysFsInfiniBandDevReader::findAllDevices() {
	list<string> interfaceNames;
	DIR *dir = opendir("/sys/class/infiniband");
	if(!dir) return interfaceNames;
	struct dirent *entry;
	while((entry = readdir(dir))) {
		if(entry->d_name[0] == '.') continue;

		string dev_name(entry->d_name);
		string path = "/sys/class/infiniband/" + dev_name + "/ports";
		DIR *ports_dir = opendir(path.c_str());
		struct dirent *ports_dir_entry;
		while((ports_dir_entry = readdir(ports_dir))) {
			if(ports_dir_entry->d_name[0] == '.') continue;
			interfaceNames.push_back(dev_name + "p" + ports_dir_entry->d_name);
		}
	}
	closedir(dir);
	return interfaceNames;
}

static uint64_t read_uint64_plain_text(const string &entry) {
	ifstream sysEntry(entry.c_str());
	if(!sysEntry.is_open())
		return 0;

	uint64_t num = 0;
	sysEntry >> num;
	if(sysEntry.fail())
		return 0;

	return num;
}

static bool is_directory(const string &path) {
	struct stat stat;
	return ::stat(path.c_str(), &stat) == 0 && S_ISDIR(stat.st_mode);
}

void LinuxSysFsInfiniBandDevReader::readFromDevice(DataFrame &dataFrame) {
	size_t port_i = m_deviceName.find_last_of('p');
	if(port_i == string::npos) {
		string path = "/sys/class/infiniband/" + m_deviceName + "/ports/";
		if(!is_directory(path)) return;
		uint64_t rx_bytes = 0;
		uint64_t tx_bytes = 0;
		uint64_t rx_packets = 0;
		uint64_t tx_packets = 0;
		uint64_t rx_errors = 0;
		uint64_t tx_drops = 0;
		DIR *dir = opendir(path.c_str());
		struct dirent *dir_entry;
		while((dir_entry = readdir(dir))) {
			if(dir_entry->d_name[0] == '.') continue;
			string counters_path = path + dir_entry->d_name + "/counters/";
			rx_bytes += read_uint64_plain_text(counters_path + "port_rcv_data") * 4;
			tx_bytes += read_uint64_plain_text(counters_path + "port_xmit_data") * 4;
			rx_packets += read_uint64_plain_text(counters_path + "port_rcv_packets");
			tx_packets += read_uint64_plain_text(counters_path + "port_xmit_packets");
			rx_errors += read_uint64_plain_text(counters_path + "port_rcv_errors");
			tx_drops += read_uint64_plain_text(counters_path + "port_xmit_discards");
		}
		closedir(dir);
		dataFrame.setTotalDataIn(rx_bytes);
		dataFrame.setTotalDataOut(tx_bytes);
		dataFrame.setTotalPacketsIn(rx_packets);
		dataFrame.setTotalPacketsOut(tx_packets);
		dataFrame.setTotalErrorsIn(rx_errors);
		dataFrame.setTotalDropsOut(tx_drops);
	} else {
		string path = "/sys/class/infiniband/" + m_deviceName.substr(0, port_i) +
			"/ports/" + m_deviceName.substr(port_i + 1) + "/counters/";
		if(!is_directory(path)) return;

		dataFrame.setTotalDataIn(read_uint64_plain_text(path + "port_rcv_data") * 4);
		dataFrame.setTotalDataOut(read_uint64_plain_text(path + "port_xmit_data") * 4);
		dataFrame.setTotalPacketsIn(read_uint64_plain_text(path + "port_rcv_packets"));
		dataFrame.setTotalPacketsOut(read_uint64_plain_text(path + "port_xmit_packets"));
		dataFrame.setTotalErrorsIn(read_uint64_plain_text(path + "port_rcv_errors"));
		dataFrame.setTotalDropsOut(read_uint64_plain_text(path + "port_xmit_discards"));
	}
	dataFrame.setValid(true);
}

