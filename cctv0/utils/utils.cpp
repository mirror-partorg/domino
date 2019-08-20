#include "utils.h"
#include <QSvgRenderer>
#include <QPainter>

QPixmap getPixmap(const QString name, int width, int height)
{
    QSvgRenderer svgRenderer(QString(":/images/theme.svg"));
    QPixmap pixmap(width, height);
    pixmap.fill( Qt::transparent );
    QPainter painter(&pixmap);
    svgRenderer.render(&painter, name, QRect(0, 0, width,height));
    return pixmap;

}


