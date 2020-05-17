// Copyright 2020 José María Castelo Ares

// This file is part of MorphogenCV.

// MorphogenCV is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// MorphogenCV is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with MorphogenCV.  If not, see <https://www.gnu.org/licenses/>.

#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include "generator.h"
#include <vector>
#include <QString>
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

class ConfigurationParser
{
public:
    ConfigurationParser(GeneratorCV *gen, const QString name): generator(gen), filename(name){};

    void write();
    void read();

private:
    GeneratorCV *generator;
    QString filename;
    QXmlStreamReader xml;
};

#endif // CONFIGPARSER_H
