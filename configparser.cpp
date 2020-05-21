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

#include "configparser.h"

void ConfigurationParser::write()
{
    QFile outFile(filename);
    outFile.open(QIODevice::WriteOnly);

    QXmlStreamWriter stream(&outFile);
    stream.setAutoFormatting(true);

    stream.writeStartDocument();

    stream.writeStartElement("pipelines");

    for (auto pipeline: generator->pipelines)
    {
        stream.writeStartElement("pipeline");
        stream.writeAttribute("blendfactor", QString::number(pipeline->blendFactor));

        for (auto operation: pipeline->imageOperations)
        {
            stream.writeStartElement("operation");
            stream.writeAttribute("name", QString::fromStdString(operation->getName()));
            stream.writeAttribute("enabled", QString::number(operation->isEnabled()));

            for (auto parameter: operation->getBoolParameters())
            {
                stream.writeStartElement("parameter");
                stream.writeAttribute("name", QString::fromStdString(parameter->name));
                stream.writeAttribute("type", "bool");
                stream.writeCharacters(QString::number(parameter->value));
                stream.writeEndElement();
            }

            for (auto parameter: operation->getIntParameters())
            {
                stream.writeStartElement("parameter");
                stream.writeAttribute("name", QString::fromStdString(parameter->name));
                stream.writeAttribute("type", "int");
                stream.writeCharacters(QString::number(parameter->value));
                stream.writeEndElement();
            }

            for (auto parameter: operation->getDoubleParameters())
            {
                stream.writeStartElement("parameter");
                stream.writeAttribute("name", QString::fromStdString(parameter->name));
                stream.writeAttribute("type", "double");
                stream.writeCharacters(QString::number(parameter->value));
                stream.writeEndElement();
            }

            for (auto parameter: operation->getMorphTypeParameters())
            {
                stream.writeStartElement("parameter");
                stream.writeAttribute("name", QString::fromStdString(parameter->name));
                stream.writeAttribute("type", "morphtype");
                stream.writeCharacters(QString::number(parameter->value));
                stream.writeEndElement();
            }

            for (auto parameter: operation->getMorphShapeParameters())
            {
                stream.writeStartElement("parameter");
                stream.writeAttribute("name", QString::fromStdString(parameter->name));
                stream.writeAttribute("type", "morphshape");
                stream.writeCharacters(QString::number(parameter->value));
                stream.writeEndElement();
            }

            for (auto parameter: operation->getInterpolationFlagParameters())
            {
                stream.writeStartElement("parameter");
                stream.writeAttribute("name", QString::fromStdString(parameter->name));
                stream.writeAttribute("type", "interpolationflag");
                stream.writeCharacters(QString::number(parameter->value));
                stream.writeEndElement();
            }

            if (operation->getKernelParameter())
            {
                stream.writeStartElement("parameter");
                stream.writeAttribute("name", QString::fromStdString(operation->getKernelParameter()->name));
                stream.writeAttribute("type", "kernel");

                for (auto element: operation->getKernelParameter()->values)
                {
                    stream.writeStartElement("element");
                    stream.writeCharacters(QString::number(element));
                    stream.writeEndElement();
                }

                stream.writeEndElement();
            }

            stream.writeEndElement();
        }

        stream.writeEndElement();
    }

    stream.writeEndElement();

    stream.writeEndDocument();

    outFile.close();
}

void ConfigurationParser::read()
{
    QFile inFile(filename);
    inFile.open(QIODevice::ReadOnly);

    xml.setDevice(&inFile);

    if (xml.readNextStartElement() && xml.name() == "pipelines")
    {
        generator->pipelines.clear();

        while (xml.readNextStartElement())
        {
            if (xml.name() == "pipeline")
            {
                double blendFactor = xml.attributes().value("blendfactor").toDouble();
                generator->loadPipeline(blendFactor);

                while (xml.readNextStartElement())
                {
                    if (xml.name() == "operation")
                    {
                        QString operationName = xml.attributes().value("name").toString();

                        bool enabled = xml.attributes().value("enabled").toInt();

                        std::vector<bool> boolParameters;
                        std::vector<int> intParameters;
                        std::vector<double> doubleParameters;
                        std::vector<int> morphTypeParameters;
                        std::vector<int> morphShapeParameters;
                        std::vector<int> interpolationFlagParameters;
                        std::vector<float> kernelElements;

                        while (xml.readNextStartElement())
                        {
                            if (xml.name() == "parameter")
                            {
                                QString parameterType = xml.attributes().value("type").toString();

                                if (parameterType == "bool")
                                    boolParameters.push_back(xml.readElementText().toInt());
                                else if (parameterType == "int")
                                    intParameters.push_back(xml.readElementText().toInt());
                                else if (parameterType == "double")
                                    doubleParameters.push_back(xml.readElementText().toDouble());
                                else if (parameterType == "morphtype")
                                    morphTypeParameters.push_back(xml.readElementText().toInt());
                                else if (parameterType == "morphshape")
                                    morphShapeParameters.push_back(xml.readElementText().toInt());
                                else if (parameterType == "interpolationflag")
                                    interpolationFlagParameters.push_back(xml.readElementText().toInt());
                                else if (parameterType == "kernel")
                                {
                                    while (xml.readNextStartElement())
                                    {
                                        if (xml.name() == "element")
                                            kernelElements.push_back(xml.readElementText().toFloat());
                                        else
                                            xml.skipCurrentElement();
                                    }
                                }
                            }
                            else
                            {
                                xml.skipCurrentElement();
                            }
                        }

                        generator->pipelines.back()->loadImageOperation(
                                    operationName.toStdString(),
                                    enabled,
                                    boolParameters,
                                    intParameters,
                                    doubleParameters,
                                    morphTypeParameters,
                                    morphShapeParameters,
                                    interpolationFlagParameters,
                                    kernelElements);
                    }
                    else
                    {
                        xml.skipCurrentElement();
                    }
                }
            }
            else
            {
                xml.skipCurrentElement();
            }
        }
    }

    if (xml.tokenType() == QXmlStreamReader::Invalid) xml.readNext();

    if (xml.hasError()) xml.raiseError();

    inFile.close();
}
