#ifndef NOTIFYSUBJECT_H
#define NOTIFYSUBJECT_H

#include "subjectObserver/ISubject.h"
#include <QThread>
#include "SubjectHeader.h"
#include <QMutex>

/**
 * @brief The NotifySubject class
 * @brief 消息推送服务
 */


class NotifySubject : public QThread, public ISubject
{
    Q_OBJECT
private:
    NotifySubject(QObject *parent = nullptr);
public:
    static NotifySubject *getInstance()
    {
        static NotifySubject subject;
        return &subject;
    }

    // ISubject interface
public:
    void addObserver(IObserver *obs);
    void removeObserver(IObserver *obs);

    // QThread interface
protected:
    void run();

public:
    void stopThreadRun();
    void sendMsg(const int &, const QString &msgContent = "");

private:
    void NotifyObservers(const int &, const QString &);

private:
    QList<IObserver *>  m_observerList;
    QList<MsgStruct>    m_msgList;
    bool                m_bRunFlag = false;

    QMutex              m_mutex;
};

#endif // NOTIFYSUBJECT_H
