////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//              ColdSpot, a Java virtual machine implementation.              //
//                    Copyright (C) 2014, Mario Morgenthum                    //
//                                                                            //
//                                                                            //
//  This program is free software: you can redistribute it and/or modify      //
//  it under the terms of the GNU General Public License as published by      //
//  the Free Software Foundation, either version 3 of the License, or         //
//  (at your option) any later version.                                       //
//                                                                            //
//  This program is distributed in the hope that it will be useful,           //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of            //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             //
//  GNU General Public License for more details.                              //
//                                                                            //
//  You should have received a copy of the GNU General Public License         //
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <fstream>

#include <zip.h>

#include <jvm/Global.hpp>

namespace coldspot
{

    class SystemClassFileInputStream::Impl
    {
    public:

        std::ifstream stream;
        zip_file *zipFileEntry;

        Impl() : zipFileEntry(0)
        {
        }

        ~Impl()
        {
            if (zipFileEntry != 0)
            {
                zip_fclose(zipFileEntry);
            }
        }
    };


    SystemClassFileInputStream::SystemClassFileInputStream()
    {

        _impl = new Impl;
    }


    SystemClassFileInputStream::~SystemClassFileInputStream()
    {

        DELETE_OBJECT(_impl);
    }


    bool SystemClassFileInputStream::load(const String &className)
    {

        String fileName = toFileName(className);

        if (loadFromClasslibrary(fileName))
        {
            return true;
        }

        return loadPlain(fileName);
    }


    void SystemClassFileInputStream::read(uint8_t *buffer, uint32_t size)
    {

        if (_impl->zipFileEntry != 0)
        {
            int64_t remaining = size;
            while (remaining != 0)
            {
                remaining -= zip_fread(_impl->zipFileEntry, buffer, remaining);
            }
        }
        else
        {
            _impl->stream.read(reinterpret_cast<char *>(buffer), size);
        }
    }


    bool SystemClassFileInputStream::loadFromClasslibrary(
        const String &fileName)
    {

        auto &javaLibrary = _vm->jdk_handler()->library();

        auto begin = javaLibrary.begin();
        auto end = javaLibrary.end();

        while (begin != end)
        {
            zip *zipFile = (zip * ) * begin;

            _impl->zipFileEntry = zip_fopen(zipFile, fileName.c_str(), 0);
            if (_impl->zipFileEntry != 0)
            {
                return true;
            }

            ++begin;
        }

        return false;
    }


    bool SystemClassFileInputStream::loadPlain(const String &fileName)
    {

        bool loaded = false;

        auto &systemProperties = _vm->options()->systemProperties;

        String javaClassPath(systemProperties.get("java.class.path")->value);
        String pathSeparator(systemProperties.get("path.separator")->value);
        String fileSeparator(systemProperties.get("file.separator")->value);

        char *classPath = new char[javaClassPath.length() + 1];
        memcpy(classPath, javaClassPath.c_str(), javaClassPath.length());
        classPath[javaClassPath.length()] = 0;

        char *classPathEntry = strtok(classPath, pathSeparator.c_str());

        while (classPathEntry != 0)
        {

            StringBuilder builder;
            builder << classPathEntry << fileSeparator << fileName;

            _impl->stream.open(builder.str().c_str());
            if (_impl->stream.is_open())
            {
                loaded = true;
                break;
            }

            classPathEntry = strtok(0, pathSeparator.c_str());
        }

        delete[] classPath;

        return loaded;
    }


    String SystemClassFileInputStream::toFileName(const String &className)
    {

        StringBuilder builder;

        builder << className;
        builder << ".class";

        return builder.str();
    }

}
