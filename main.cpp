#include <QCoreApplication>
#include "xmlparser.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    if (app.arguments().size() < 2)
    {
        qWarning("Use parser <file1.xml>");
        return 1;
    }

    XmlParser parser;

    parser.parseAndPrint(app.arguments().at(1));

    return 0;
}
