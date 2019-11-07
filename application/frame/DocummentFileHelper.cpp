#include "DocummentFileHelper.h"
#include <DApplication>
#include <QClipboard>
#include <QDesktopServices>
#include "subjectObserver/MsgHeader.h"
#include "controller/DataManager.h"
#include <DFileDialog>
#include "utils/utils.h"
#include <DMessageBox>
#include "subjectObserver/ModuleHeader.h"
#include "controller/DBManager.h"
#include "controller/MsgSubject.h"

DocummentFileHelper::DocummentFileHelper(QObject *parent)
    : QObject(parent)
{
    m_pDocummentProxy = DocummentProxy::instance();

    initConnections();
}

bool DocummentFileHelper::save(const QString &filepath, bool withChanges)
{
    return m_pDocummentProxy->save(filepath, withChanges);
}

bool DocummentFileHelper::closeFile()
{
    return m_pDocummentProxy->closeFile();
}

void DocummentFileHelper::docBasicInfo(stFileInfo &info)
{
    m_pDocummentProxy->docBasicInfo(info);
}

bool DocummentFileHelper::mouseSelectText(QPoint start, QPoint stop)
{
    return m_pDocummentProxy->mouseSelectText(start, stop);
}

bool DocummentFileHelper::mouseBeOverText(QPoint point)
{
    return m_pDocummentProxy->mouseBeOverText(point);
}

Page::Link *DocummentFileHelper::mouseBeOverLink(QPoint point)
{
    return m_pDocummentProxy->mouseBeOverLink(point);
}

void DocummentFileHelper::mouseSelectTextClear()
{
    m_pDocummentProxy->mouseSelectTextClear();
}

bool DocummentFileHelper::getSelectTextString(QString &st)
{
    return m_pDocummentProxy->getSelectTextString(st);
}

//  保存
void DocummentFileHelper::slotSaveFile()
{
    bool rl = m_pDocummentProxy->save(m_szFilePath, true);
    if (rl) {
        //  保存需要保存 数据库记录
        DBManager::instance()->saveBookMark();
        DataManager::instance()->setBIsUpdate(false);
    }
}

//  另存为
void DocummentFileHelper::slotSaveAsFile()
{
    QString sFilter = getFileFilter();

    if (sFilter != "") {
        DFileDialog dialog;
        dialog.selectFile(m_szFilePath);
        QString filePath = dialog.getSaveFileName(nullptr, tr("Save File"), m_szFilePath, sFilter);
        if (filePath != "") {
            QString sFilePath = getFilePath(filePath);

            bool rl = m_pDocummentProxy->saveas(sFilePath, true);
            if (rl) {
                //insert a new bookmark record to bookmarktabel
                DBManager::instance()->saveasBookMark(m_szFilePath, sFilePath);
                DataManager::instance()->setStrOnlyFilePath(sFilePath);

                DataManager::instance()->setBIsUpdate(false);

                m_szFilePath = sFilePath;
                QFileInfo info(m_szFilePath);
                setAppShowTitle(info.baseName());
            }
        }
    }
}

QString DocummentFileHelper::getFileFilter()
{
    QString sFilter = "";
    if (m_nCurDocType == DocType_PDF) {
        sFilter = Constant::sPdf_Filter;
    } else if (m_nCurDocType == DocType_TIFF) {
        sFilter = Constant::sTiff_Filter;
    } else if (m_nCurDocType == DocType_PS) {
        sFilter = Constant::sPs_Filter;
    } else if (m_nCurDocType == DocType_XPS) {
        sFilter = Constant::sXps_Filter;
    } else if (m_nCurDocType == DocType_DJVU) {
        sFilter = Constant::sDjvu_Filter;
    }
    return sFilter;
}

QString DocummentFileHelper::getFilePath(const QString &inputPath)
{
    QString filePath = inputPath;
    if (m_nCurDocType == DocType_PDF) {
        if (!filePath.endsWith(".pdf")) {
            filePath += ".pdf";
        }
    } else if (m_nCurDocType == DocType_TIFF) {
        if (!filePath.endsWith(".tiff")) {
            filePath += ".tiff";
        }
    } else if (m_nCurDocType == DocType_PS) {
        if (!filePath.endsWith(".ps")) {
            filePath += ".ps";
        }
    } else if (m_nCurDocType == DocType_XPS) {
        if (!filePath.endsWith(".xps")) {
            filePath += ".xps";
        }
    } else if (m_nCurDocType == DocType_DJVU) {
        if (!filePath.endsWith(".djvu")) {
            filePath += ".djvu";
        }
    }
    return filePath;
}

void DocummentFileHelper::setCurDocuType(const QString &sCompleteSuffix)
{
    if (sCompleteSuffix == "pdf" || sCompleteSuffix.endsWith("pdf")) {
        m_nCurDocType = DocType_PDF;
    } else if (sCompleteSuffix == "tiff" || sCompleteSuffix.endsWith("tiff")) {
        m_nCurDocType = DocType_TIFF;
    } else if (sCompleteSuffix == "ps" || sCompleteSuffix.endsWith("ps")) {
        m_nCurDocType = DocType_PS;
    } else if (sCompleteSuffix == "xps" || sCompleteSuffix.endsWith("xps")) {
        m_nCurDocType = DocType_XPS;
    } else if (sCompleteSuffix == "djvu" || sCompleteSuffix.endsWith("djvu")) {
        m_nCurDocType = DocType_DJVU;
    }
}

//  打开　文件路径
void DocummentFileHelper::slotOpenFile(const QString &filePaths)
{
    //  已经打开了文件，　询问是否需要保存当前打开的文件
    if (m_szFilePath != "") {
        //  是否有操作
        bool rl = DataManager::instance()->bIsUpdate();
        if (rl) {
            if (QMessageBox::Yes == DMessageBox::question(nullptr, tr("Save File"), tr("Do you need to save the file opened?"))) {
                m_pDocummentProxy->save(m_szFilePath, true);

                //  保存 书签数据
                DBManager::instance()->saveBookMark();
            }
        }
        m_pDocummentProxy->closeFile();
        sendMsg(MSG_CLOSE_FILE);
    }

    QStringList fileList = filePaths.split(Constant::sQStringSep,  QString::SkipEmptyParts);
    int nSize = fileList.size();
    if (nSize > 0) {
        QString sPath = fileList.at(0);

        QFileInfo info(sPath);

        QString sCompleteSuffix = info.completeSuffix();
        setCurDocuType(sCompleteSuffix);

        m_szFilePath = sPath;
        DataManager::instance()->setStrOnlyFilePath(sPath);

        bool rl = m_pDocummentProxy->openFile(m_nCurDocType, sPath);
        if (!rl) {
            m_szFilePath = "";
            DataManager::instance()->setStrOnlyFilePath("");

            sendMsg(MSG_OPERATION_OPEN_FILE_FAIL, tr("File not supported"));
        }
    }
}

//  设置  应用显示名称
void DocummentFileHelper::setAppShowTitle(const QString &fileName)
{
    QString sTitle = "";
    m_pDocummentProxy->title(sTitle);
    if (sTitle == "") {
        sTitle = fileName;
    }
    sendMsg(MSG_OPERATION_OPEN_FILE_TITLE, sTitle);
}

//  复制
void DocummentFileHelper::slotCopySelectContent(const QString &sCopy)
{
    QClipboard *clipboard = DApplication::clipboard();   //获取系统剪贴板指针
    clipboard->setText(sCopy);
}

/**
 * @brief DocummentFileHelper::slotFileSlider
 * @param nFlag 1 开启放映, 0 退出放映
 */
void DocummentFileHelper::slotFileSlider(const int &nFlag)
{
    if (nFlag == 1) {
        bool bSlideModel = m_pDocummentProxy->showSlideModel();    //  开启幻灯片
        if (bSlideModel) {
            DataManager::instance()->setCurShowState(FILE_SLIDE);
        }
    } else {
        if (DataManager::instance()->CurShowState() == FILE_SLIDE) {
            bool rl = m_pDocummentProxy->exitSlideModel();
            if (rl) {
                DataManager::instance()->setCurShowState(FILE_NORMAL);
            }
        }
    }
}

void DocummentFileHelper::initConnections()
{
    connect(m_pDocummentProxy, &DocummentProxy::signal_openResult, this, [ = ](bool openresult) {
        if (openresult) {
            //  通知 其他窗口， 打开文件成功了！！！
            sendMsg(MSG_OPERATION_OPEN_FILE_OK);
            QFileInfo info(m_szFilePath);
            setAppShowTitle(info.baseName());
        } else {
            sendMsg(MSG_OPERATION_OPEN_FILE_FAIL, tr("Please check if the file is damaged"));
        }
    });
}

void DocummentFileHelper::sendMsg(const int &msgType, const QString &msgContent)
{
    MsgSubject::getInstance()->sendMsg(nullptr, msgType, msgContent);
}

//  文档　跳转页码　．　打开浏览器
void DocummentFileHelper::onClickPageLink(Page::Link *pLink)
{
    Page::LinkType_EM linkType = pLink->type;
    if (linkType == Page::LinkType_NULL) {

    } else if (linkType == Page::LinkType_Goto) {
        int page = pLink->page - 1;
        m_pDocummentProxy->pageJump(page);
    } else if (linkType == Page::LinkType_GotoOtherFile) {

    } else if (linkType == Page::LinkType_Browse) {
        QString surlOrFileName = pLink->urlOrFileName;
        QDesktopServices::openUrl(QUrl(surlOrFileName, QUrl::TolerantMode));
    } else if (linkType == Page::LinkType_Execute) {

    }
}

QPoint DocummentFileHelper::global2RelativePoint(QPoint globalpoint)
{
    return  m_pDocummentProxy->global2RelativePoint(globalpoint);
}

bool DocummentFileHelper::pageMove(double mvx, double mvy)
{
    return m_pDocummentProxy->pageMove(mvx, mvy);
}

int DocummentFileHelper::pointInWhichPage(QPoint pos)
{
    return m_pDocummentProxy->pointInWhichPage(pos);
}

int DocummentFileHelper::getPageSNum()
{
    return m_pDocummentProxy->getPageSNum();
}

int DocummentFileHelper::currentPageNo()
{
    return m_pDocummentProxy->currentPageNo();
}

bool DocummentFileHelper::pageJump(int pagenum)
{
    return m_pDocummentProxy->pageJump(pagenum);
}

bool DocummentFileHelper::getImage(int pagenum, QImage &image, double width, double height)
{
    return m_pDocummentProxy->getImage(pagenum, image, width, height);
}

QImage DocummentFileHelper::roundImage(const QPixmap &img_in, int radius)
{
    if (img_in.isNull())
    {
        return QPixmap().toImage();
    }
    QSize size(img_in.size());
    QBitmap mask(size);
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(mask.rect(), Qt::white);
    painter.setBrush(QColor(0, 0, 0));
    painter.drawRoundedRect(mask.rect(), radius, radius);
    QPixmap image = img_in;
    image.setMask(mask);
    return image.toImage();
}

bool DocummentFileHelper::showMagnifier(QPoint point)
{
    return  m_pDocummentProxy->showMagnifier(point);
}

bool DocummentFileHelper::closeMagnifier()
{
    return m_pDocummentProxy->closeMagnifier();
}

bool DocummentFileHelper::setBookMarkState(int page, bool state)
{
    return m_pDocummentProxy->setBookMarkState(page, state);
}

bool DocummentFileHelper::setViewModeAndShow(ViewMode_EM viewmode)
{
    return m_pDocummentProxy->setViewModeAndShow(viewmode);
}

void DocummentFileHelper::scaleRotateAndShow(double scale, RotateType_EM rotate)
{
    m_pDocummentProxy->scaleRotateAndShow(scale, rotate);
}

double DocummentFileHelper::adaptWidthAndShow(const double &width)
{
    return m_pDocummentProxy->adaptWidthAndShow(width);
}

double DocummentFileHelper::adaptHeightAndShow(const double &height)
{
    return m_pDocummentProxy->adaptHeightAndShow(height);
}

void DocummentFileHelper::clearsearch()
{
    m_pDocummentProxy->clearsearch();
}

void DocummentFileHelper::getAllAnnotation(QList<stHighlightContent> &listres)
{
    m_pDocummentProxy->getAllAnnotation(listres);
}

QString DocummentFileHelper::addAnnotation(const QPoint &startpos, const QPoint &endpos, QColor color)
{
    return m_pDocummentProxy->addAnnotation(startpos, endpos, color);
}

bool DocummentFileHelper::annotationClicked(const QPoint &pos, QString &strtext, QString &struuid)
{
    return m_pDocummentProxy->annotationClicked(pos, strtext, struuid);
}

void DocummentFileHelper::removeAnnotation(const QString &struuid, int ipage)
{
    m_pDocummentProxy->removeAnnotation(struuid, ipage);
}

QString DocummentFileHelper::removeAnnotation(const QPoint &pos)
{
    return m_pDocummentProxy->removeAnnotation(pos);
}

void DocummentFileHelper::setAnnotationText(int ipage, const QString &struuid, const QString &strtext)
{
    m_pDocummentProxy->setAnnotationText(ipage, struuid, strtext);
}

void DocummentFileHelper::getAnnotationText(const QString &struuid, QString &strtext, int ipage)
{
    m_pDocummentProxy->getAnnotationText(struuid, strtext, ipage);
}

void DocummentFileHelper::jumpToHighLight(const QString &uuid, int ipage)
{
    m_pDocummentProxy->jumpToHighLight(uuid, ipage);
}

void DocummentFileHelper::search(const QString &strtext, QMap<int, stSearchRes> &resmap, const QColor &color)
{
    m_pDocummentProxy->search(strtext, resmap, color);
}

void DocummentFileHelper::findNext()
{
    m_pDocummentProxy->findNext();
}

void DocummentFileHelper::findPrev()
{
    m_pDocummentProxy->findPrev();
}
