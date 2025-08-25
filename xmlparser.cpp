#include "xmlparser.h"

#include <QFile>
#include <QXmlStreamReader>
#include <QTextStream>
#include <QByteArray>
#include <QDebug>

void XmlParser::parseAndPrint(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Cant open file:" << path;
        return;
    }

    QXmlStreamReader xml(&file);
    QTextStream out(stdout);

    int counter = 0; // счетчик командных блоков 1-й - request, 2-й - response и т.д.по порядку
    while (!xml.atEnd())
    {
        xml.readNext();

        if (xml.isStartElement() && xml.name() == "TContextCMD")
        {
            ++counter;
            const bool isReq = (counter % 2 == 1); // нечётные - запросы, чётные - ответы

            const auto attrsCMD = xml.attributes();
            const QString fullCMD = attrsCMD.value("Data").toString();

            out << (isReq ? "Request:\n" : "Response:\n");
            out << "Command: " << fullCMD << "\n";

            // разбираем вложенные поля
            while (!(xml.isEndElement() && xml.name() == "TContextCMD") && !xml.atEnd())
            {
                xml.readNext();

                if (xml.isStartElement() && xml.name() == "TCont")
                {
                    const auto attrs = xml.attributes();
                    QString name = attrs.value("Name").toString();
                    const QString type = attrs.value("Type").toString();
                    const QString data = attrs.value("Data").toString();
                    const int storageLen = attrs.value("StorageLen").toInt();

                    // в задании было сказано что CmdCode -> Cmd, а в примере видно что и A1 -> Cmd
                    if (name == "A1" || name == "CmdCode")
                        name = "Cmd";

                    // ограничиваем строку, учитывая длину данных
                    const int maxLen = storageLen > 0 ? storageLen * 2 : data.size();
                    const QString cut = data.left(maxLen);

                    QString parsedVal;

                    if (type == "A")
                    {
                        // ASCII: hex -> байты -> текст
                        QByteArray bytes = QByteArray::fromHex(cut.toLatin1());
                        parsedVal = QString::fromLatin1(bytes);
                    }
                    else if (type == "H" || type == "N" || type == "B")
                        parsedVal = cut;
                    else
                        parsedVal = cut;

                    out << name << ": " << parsedVal << "\n";
                }
            }
            out << "\n";
        }
    }

    if (xml.hasError())
        qWarning() << "XML parse error in file" << path << ":" << xml.errorString();
}
