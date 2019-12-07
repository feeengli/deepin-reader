#include "ImageLabel.h"
#include <DGuiApplicationHelper>
#include <QDebug>
#include <QPainter>
#include <QPalette>
#include "controller/DBManager.h"
#include "utils/utils.h"

ImageLabel::ImageLabel(DWidget *parent)
    : DLabel(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
}

void ImageLabel::setSelect(const bool &select)
{
    m_bSelect = select;
    if (m_bSelect) {
        if (m_nRadius == 8) {
            m_nHighLightLineWidth = 4;
        } else if (m_nRadius == 10) {
            m_nHighLightLineWidth = 6;
        }
    }
    update();
}

void ImageLabel::rotateImage(int angle, bool rotate)
{
    if (rotate == false) {
        return;
    }

    if (angle > 360) {
        angle %= 360;
    }

    m_nRotate = angle;
    m_bRotate = rotate;

    setFixedSize(height(), width());

    update();
}

void ImageLabel::rotateImage(QPainter &painter)
{
    painter.translate(width() / 2, height() / 2);
    painter.rotate(m_nRotate);
    painter.translate(-width() / 2, -height() / 2);
    painter.save();
}

void ImageLabel::paintEvent(QPaintEvent *e)
{
    DLabel::paintEvent(e);

    int local = 0;
    int width = this->width();
    int heigh = this->height();
    int penwidth = 0;
    QPalette p(this->palette());
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    if (m_bRotate) {
        rotateImage(painter);
    }

    penwidth = m_nHighLightLineWidth;

    if (m_bSelect) {
        local = 1;
        width -= 2;
        heigh -= 2;
        //        penwidth = 8;
        painter.setPen(
            QPen(DGuiApplicationHelper::instance()->applicationPalette().highlight().color(),
                 penwidth, Qt::SolidLine));
    } else {
        local = 2;
        width -= 4;
        heigh -= 4;
        penwidth = 1;
        painter.setPen(QPen(
            DGuiApplicationHelper::instance()->applicationPalette().frameShadowBorder().color(),
            penwidth, Qt::SolidLine));
    }

    if (m_bSetBp) {
        //填充图片
        QPainterPath path;
        QRectF pixrectangle(local, local, width, heigh);
        path.addRoundedRect(pixrectangle, m_nRadius, m_nRadius);
        painter.setClipPath(path);
        QRect bprectangle(local, local, width, heigh);
        painter.drawPixmap(bprectangle, m_background);
        QRectF rectangle(local, local, width, heigh);
        painter.drawRoundedRect(rectangle, m_nRadius, m_nRadius);
    }

    if (m_bshowbookmark) {
        QString ssPath = ":/resources/image/";
        DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
        if (themeType == DGuiApplicationHelper::LightType)
            ssPath += "light";
        else if (themeType == DGuiApplicationHelper::DarkType)
            ssPath += "dark";

        ssPath += "/checked/bookmarkbig_checked_light.svg";
        QPixmap pixmap(Utils::renderSVG(ssPath, /*QSize(16,16)*/ QSize(36, 36)));
        QPainter painter1(this);
        painter1.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        painter1.drawPixmap(this->width() - 36 - 6, 0, 36, 36, pixmap);
    }

    m_bRotate = false;
}
