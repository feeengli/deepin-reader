#ifndef DOCUMMENTPROXY_H
#define DOCUMMENTPROXY_H
#include "docummentbase.h"
#include "commonstruct.h"
#include <QObject>
#include <DWidget>
class DocummentProxy;
typedef   struct   STCURDOCUMENTPROXY {
    DWidget *pwgt;
    DocummentProxy *proxy;

    STCURDOCUMENTPROXY()
    {
        pwgt = nullptr;
        proxy = nullptr;
    }

} stCurDocProxy;

class DocummentProxy: public QObject
{
    Q_OBJECT

public:
    DocummentProxy(QObject *parent = nullptr);
    ~DocummentProxy()
    {
        closeFileAndWaitThreadClearEnd();
    }
public:
    static QString CreateInstance(QObject *parent = nullptr);
    static DocummentProxy *instance(QString uuid = QString());
    bool openFile(DocType_EM type, QString filepath, unsigned int ipage = 0, RotateType_EM rotatetype = RotateType_0, double scale = 1.0, ViewMode_EM viewmode = ViewMode_SinglePage);
    bool closeFile();
    QPoint global2RelativePoint(QPoint globalpoint);
    bool setSelectTextStyle(QColor paintercolor = QColor(72, 118, 255, 100), QColor pencolor = QColor(72, 118, 255, 0), int penwidth = 0);
    bool mouseSelectText(QPoint start, QPoint stop);
    void mouseSelectTextClear();
    bool mouseBeOverText(QPoint point);
    void setScaleRotateViewModeAndShow(double scale, RotateType_EM rotate, ViewMode_EM viewmode);
    void scaleRotateAndShow(double scale, RotateType_EM rotate);
    bool getImage(int pagenum, QImage &image, double width, double height);
    int  getPageSNum();
    bool setViewModeAndShow(ViewMode_EM viewmode);
    bool showMagnifier(QPoint point);
    bool closeMagnifier();
    bool setMagnifierStyle(QColor magnifiercolor = Qt::white, int magnifierradius = 100, int magnifierringwidth = 10, double magnifierscale = 2);
    QString addAnnotation(const QPoint &startpos, const QPoint &endpos, QColor color = Qt::yellow);
    bool save(const QString &filepath, bool withChanges);
    bool saveas(const QString &filepath, bool withChanges);
    void search(const QString &strtext, QMap<int, stSearchRes> &resmap, const QColor &color);
    void clearsearch();
    int currentPageNo();
    bool pageJump(int pagenum);
    void docBasicInfo(stFileInfo &info);
    QString removeAnnotation(const QPoint &startpos, AnnoteType_Em type = Annote_Highlight);
    void removeAnnotation(const QString &struuid, int ipage = -1);
    bool pageMove(double mvx, double mvy);
    void title(QString &title);
    Page::Link *mouseBeOverLink(QPoint point);
    bool getSelectTextString(QString &st);
    bool showSlideModel();
    bool exitSlideModel();
    void findNext();
    void findPrev();
    void closeFileAndWaitThreadClearEnd();
    void setAnnotationText(int ipage, const QString &struuid, const QString &strtext);
    void getAnnotationText(const QString &struuid, QString &strtext, int ipage);
    double adaptWidthAndShow(double width);
    double adaptHeightAndShow(double height);
    bool annotationClicked(const QPoint &pos, QString &strtext, QString &struuid);
    void getAllAnnotation(QList<stHighlightContent> &listres);
    int  pointInWhichPage(QPoint pos);
    void jumpToHighLight(const QString &uuid, int ipage);
    void jumpToOutline(const qreal  &realleft, const qreal &realtop, unsigned int ipage = 0);
    bool setBookMarkState(int page, bool state);
    void changeAnnotationColor(int ipage, const QString uuid, const QColor &color);
    void selectAllText();
    void setAutoPlaySlide(bool autoplay, int timemsec = 3000);
    bool getAutoPlaySlideStatu();
    bool isOpendFile();
    void setViewFocus();
    double getMaxZoomratio();
    Outline outline();
    //icon annotation
    QString addIconAnnotation(const QPoint &pos, const QColor &color = Qt::yellow, TextAnnoteType_Em type = TextAnnoteType_Note);
    bool iconAnnotationClicked(const QPoint &pos, QString &strtext, QString &struuid);
    bool  mouseovericonAnnotation(const QPoint &pos);
    void moveIconAnnotation(const QString &uuid, const QPoint &pos);

    QString pagenum2label(int index);
    int label2pagenum(QString label);
    bool haslabel();

    void setparam(const DWidget *pwget, const QString &struuid);
signals:
    void signal_pageChange(int);
    bool signal_pageJump(int);
    void signal_searchRes(stSearchRes);
    void signal_searchover();
    bool signal_mouseSelectText(QPoint start, QPoint stop);
    void signal_setScaleRotateViewModeAndShow(double scale, RotateType_EM rotate, ViewMode_EM viewmode);
    void signal_scaleAndShow(double scale, RotateType_EM rotate);
    bool signal_setViewModeAndShow(ViewMode_EM viewmode);
    void signal_bookMarkStateChange(int page, bool state);
    void signal_openResult(bool);
    void signal_autoplaytoend();
private slots:
    void slot_pageChange(int);
    void slot_changetab(const QString &uuid);
    void slot_closetab(const QString &uuid);

private:
    DWidget *pwgt;
    QString m_path;
    DocummentBase *m_documment;
    bool bcloseing;
    QString m_struuid;
    static QMap<QString, DocummentProxy *> m_proxymap;
};

#endif // DOCUMMENTPROXY_H
