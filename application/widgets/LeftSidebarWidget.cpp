#include "LeftSidebarWidget.h"

#include <DStackedWidget>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QJsonObject>

#include "MainOperationWidget.h"
#include "MsgModel.h"

#include "controller/AppSetting.h"
#include "controller/FileDataManager.h"
#include "docview/docummentproxy.h"

#include "pdfControl/bookmark/BookMarkWidget.h"
#include "pdfControl/catalog/CatalogWidget.h"
#include "pdfControl/note/NotesWidget.h"
#include "pdfControl/search/BufferWidget.h"
#include "pdfControl/search/SearchResWidget.h"
#include "pdfControl/thumbnail/ThumbnailWidget.h"

LeftSidebarWidget::LeftSidebarWidget(CustomWidget *parent)
    : CustomWidget("LeftSidebarWidget", parent)
{
    setMinimumWidth(LEFTMINWIDTH);
    setMaximumWidth(LEFTMAXWIDTH);

    resize(LEFTNORMALWIDTH, this->height());

    initWidget();
    initConnections();

    onSetWidgetVisible(0);  //  默认 隐藏
    slotUpdateTheme();

    dApp->m_pModelService->addObserver(this);
}

LeftSidebarWidget::~LeftSidebarWidget()
{
    dApp->m_pModelService->removeObserver(this);
}

//void LeftSidebarWidget::slotDealWithData(const int &msgType, const QString &msgContent)
//{
//    if (msgType == MSG_WIDGET_THUMBNAILS_VIEW) {//  控制 侧边栏显隐
//        onSetWidgetVisible(msgContent.toInt());
//    }
//}

void LeftSidebarWidget::SlotOpenFileOk(const QString &sPath)
{
    if (m_strPath == sPath) {
        FileDataModel fdm = dApp->m_pDataManager->qGetFileData(sPath);
        int nShow = fdm.qGetData(Thumbnail);
        bool showLeft = nShow == 1 ? true : false;
        onSetWidgetVisible(showLeft);
    }
}

void LeftSidebarWidget::__DealWithPressKey(const QString &sKey)
{
    if (sKey == KeyStr::g_up || sKey == KeyStr::g_pgup || sKey == KeyStr::g_left) {
        onJumpToPrevPage();
    } else if (sKey == KeyStr::g_down || sKey == KeyStr::g_pgdown || sKey == KeyStr::g_right) {
        onJumpToNextPage();
    } else if (sKey == KeyStr::g_del) {
        __DeleteItemByKey();
    }
}

void LeftSidebarWidget::onSetWidgetVisible(const int &nVis)
{
    this->setVisible(nVis);
    if (!nVis && DocummentProxy::instance())
        DocummentProxy::instance()->setViewFocus();
}

void LeftSidebarWidget::slotUpdateTheme()
{
    updateWidgetTheme();
}

//  按钮 按键显示对应 widget
void LeftSidebarWidget::slotSetStackCurIndex(const int &iIndex)
{
    auto pWidget = this->findChild<DStackedWidget *>();
    if (pWidget) {
        pWidget->setCurrentIndex(iIndex);
    }

    //  前一个是 出来搜索结果了, 后一个是正在搜索, 两个都不需要保存在记录中
    if (iIndex == WIDGET_SEARCH || iIndex == WIDGET_BUFFER) {
        emit sigSearchWidgetState(iIndex);
    } else {
        notifyMsg(MSG_LEFTBAR_STATE, QString::number(iIndex));
    }
}

void LeftSidebarWidget::slotFileChangeMsg(const QString &sContent)
{
    QString sCurPath = dApp->m_pDataManager->qGetCurrentFilePath();
    if (m_strPath == sCurPath) {
        MsgModel mm;
        mm.fromJson(sContent);

        QString nValue = mm.getValue();
        int nMsg = mm.getMsgType();
        if (nMsg == MSG_WIDGET_THUMBNAILS_VIEW) {
            onSetWidgetVisible(nValue.toInt());
        }
    }
}

//  上一页
void LeftSidebarWidget::onJumpToPrevPage()
{
    auto pWidget = this->findChild<DStackedWidget *>();
    if (pWidget) {
        int iIndex = pWidget->currentIndex();
        if (iIndex == WIDGET_THUMBNAIL) {
            auto widget = this->findChild<ThumbnailWidget *>();
            if (widget) {
                widget->prevPage();
            }
        }  else if (iIndex == WIDGET_BOOKMARK) {
            auto widget = this->findChild<BookMarkWidget *>();
            if (widget) {
                widget->prevPage();
            }
        } else if (iIndex == WIDGET_NOTE) {
            auto widget = this->findChild<NotesWidget *>();
            if (widget) {
                widget->prevPage();
            }
        }
    }
}

//  下一页
void LeftSidebarWidget::onJumpToNextPage()
{
    auto pWidget = this->findChild<DStackedWidget *>();
    if (pWidget) {
        int iIndex = pWidget->currentIndex();
        if (iIndex == WIDGET_THUMBNAIL) {
            auto widget = this->findChild<ThumbnailWidget *>();
            if (widget) {
                widget->nextPage();
            }
        }  else if (iIndex == WIDGET_BOOKMARK) {
            auto widget = this->findChild<BookMarkWidget *>();
            if (widget) {
                widget->nextPage();
            }
        } else if (iIndex == WIDGET_NOTE) {
            auto widget = this->findChild<NotesWidget *>();
            if (widget) {
                widget->nextPage();
            }
        }
    }
}

void LeftSidebarWidget::initConnections()
{
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));
    connect(this, SIGNAL(sigOpenFileOk(const QString &)), SLOT(SlotOpenFileOk(const QString &)));
    connect(this, SIGNAL(sigFileChangeMsg(const QString &)), SLOT(slotFileChangeMsg(const QString &)));
//    connect(this, SIGNAL(sigDealWithData(const int &, const QString &)), SLOT(slotDealWithData(const int &, const QString &)));
}

//  按delete 键 删除书签 或者 注释
void LeftSidebarWidget::__DeleteItemByKey()
{
    auto pWidget = this->findChild<DStackedWidget *>();
    if (pWidget) {
        int iIndex = pWidget->currentIndex();
        if (iIndex == WIDGET_BOOKMARK) {
            auto widget = this->findChild<BookMarkWidget *>();
            if (widget) {
                widget->DeleteItemByKey();
            }
        } else if (iIndex == WIDGET_NOTE) {
            auto widget = this->findChild<NotesWidget *>();
            if (widget) {
                widget->DeleteItemByKey();
            }
        }
    }
}

void LeftSidebarWidget::initWidget()
{
    auto pVBoxLayout = new QVBoxLayout;
    pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    pVBoxLayout->setSpacing(0);
    this->setLayout(pVBoxLayout);

    auto pStackedWidget = new DStackedWidget;
    pStackedWidget->insertWidget(WIDGET_THUMBNAIL, new ThumbnailWidget(this));
    pStackedWidget->insertWidget(WIDGET_catalog, new CatalogWidget(this));
    pStackedWidget->insertWidget(WIDGET_BOOKMARK, new BookMarkWidget(this));
    pStackedWidget->insertWidget(WIDGET_NOTE, new NotesWidget(this));
    pStackedWidget->insertWidget(WIDGET_SEARCH, new SearchResWidget(this));

    auto buffWidget = new BufferWidget(this);
    connect(this, SIGNAL(sigSearchWidgetState(const int &)), buffWidget, SLOT(SlotSetSpinnerState(const int &)));

    pStackedWidget->insertWidget(WIDGET_BUFFER, buffWidget);
    pStackedWidget->setCurrentIndex(WIDGET_THUMBNAIL);

    pVBoxLayout->addWidget(pStackedWidget);

    auto mainOperation = new MainOperationWidget;
    connect(mainOperation, SIGNAL(sigShowStackWidget(const int &)), SLOT(slotSetStackCurIndex(const int &)));

    pVBoxLayout->addWidget(mainOperation, 0, Qt::AlignBottom);
}

int LeftSidebarWidget::dealWithData(const int &msgType, const QString &msgContent)
{
//    if (m_pMsgList.contains(msgType)) {
//        emit sigDealWithData(msgType, msgContent);
//        return ConstantMsg::g_effective_res;
//    }
    if (msgType == E_FILE_MSG) {
        emit sigFileChangeMsg(msgContent);
    } else if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        emit sigOpenFileOk(msgContent);
    } else if (msgType == MSG_OPERATION_UPDATE_THEME) {
        emit sigUpdateTheme();
    }

    return 0;
}

void LeftSidebarWidget::qSetPath(const QString &sPath)
{
    m_strPath = sPath;
}

void LeftSidebarWidget::keyPressEvent(QKeyEvent *event)
{
    QStringList pFilterList = QStringList() << KeyStr::g_pgup << KeyStr::g_pgdown
                              << KeyStr::g_down << KeyStr::g_up
                              << KeyStr::g_left << KeyStr::g_right
                              << KeyStr::g_del;
    QString key = Utils::getKeyshortcut(event);
    if (pFilterList.contains(key)) {
        __DealWithPressKey(key);
    }

    CustomWidget::keyPressEvent(event);
}
