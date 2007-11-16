/***************************************************************************
                                  setting.h
                             -------------------
    begin                : Tue Nov 06 2007
    copyright            : (C) 2007 by Roland Riegel
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

#ifndef SETTING_H
#define SETTING_H

#include "stringutils.h"

#include <iosfwd>
#include <map>
#include <sstream>
#include <string>

class Setting
{
    public:
        Setting(const std::string& id = "", const std::string& description = "", const std::string& value = "") {}
        template<class T>
        Setting(const std::string& id = "", const std::string& description = "", const T& value = T())
            : m_id(id), m_description(description), m_value(toString(value)) {}
        ~Setting() {}

        const std::string& getId() const { return m_id; }
        const std::string& getDescription() const { return m_description; }
        const std::string& getValue() const { return m_value; }
        const std::map<std::string, std::string>& getValueMapping() const { return m_mapping; }

        void setId(const std::string& id) { m_id = id; }
        void setDescription(const std::string& description) { m_description = description; }
        void setValueMapping(const std::map<std::string, std::string>& mapping) { m_mapping = mapping; }

        std::string mapToString() const;
        void assignThroughMap(const std::string& value);

        std::istream& operator>>(std::istream& in);
        std::ostream& operator<<(std::ostream& out) const;

        template<class T>
        operator T() { return fromString<T>(m_value); }

        template<class T>
        Setting& operator=(const T& t) { m_value = toString(t); return *this; }

        template<class T>
        bool operator==(const T& t) const { return toString(t) == m_value; }
        template<class T>
        bool operator!=(const T& t) const { return toString(t) != m_value; }
        template<class T>
        bool operator<(const T& t) const { return fromString<T>(m_value) < t; }
        template<class T>
        bool operator>(const T& t) const { return fromString<T>(m_value) > t; }
        template<class T>
        bool operator<=(const T& t) const { return fromString<T>(m_value) <= t; }
        template<class T>
        bool operator>=(const T& t) const { return fromString<T>(m_value) >= t; }

        bool operator==(const Setting& s) const { return s.m_value == m_value; }
        bool operator!=(const Setting& s) const { return s.m_value != m_value; }

    private:
        std::string m_id;
        std::string m_description;
        std::string m_value;
        std::map<std::string, std::string> m_mapping;
};

#endif
