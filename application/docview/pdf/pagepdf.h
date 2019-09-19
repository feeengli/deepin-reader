#ifndef PAGEPDF_H
#define PAGEPDF_H
#include "../pagebase.h"
#include <QImage>
#include <poppler-qt5.h>

class PagePdf: public PageBase
{
    Q_OBJECT
public:
    PagePdf(QWidget *parent = 0);
    bool showImage(double scale = 1, RotateType_EM rotate = RotateType_Normal) override;
    bool getImage(QImage &image, double width, double height) override;
    bool getSlideImage(QImage &image, double &width, double &height) override;
    void appendWord(stWord word);
    void setPage(Poppler::Page *page);
    bool loadLinks();
    //Annotation
    QString addAnnotation(QPoint screenPos);
    QString removeAnnotation(const QPoint &pos);
    void removeAnnotation(const QString &struuid);
    bool annotationClicked(const QPoint &pos, QString &strtext);
private:
    void removeAnnotation(Poppler::Annotation *annotation);
    QString addHighlightAnnotation(const QList<QRectF> &listrect, const QColor &color);
    Poppler::Page *m_page;
};

#endif // PAGEPDF_H
