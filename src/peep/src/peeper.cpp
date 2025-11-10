#include "peep/peeper.h"
#include <QGuiApplication>

namespace bit::peep
{
void peeper::show()
{
    int argc    = 0;
    char** argv = nullptr;
    QGuiApplication app(argc, argv);
}
} // namespace bit::peep
