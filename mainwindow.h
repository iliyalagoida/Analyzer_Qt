#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "icon_tabs.h"
#include "backgroundthread.h"
#include <QListWidgetItem>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
        friend class backgroundthread;
public slots:
    void ManageThreadCode(int code, int device_id);
    void UpdateDeviceStatus(int status, int device_id);
    void UpdateDP5StatusString(QString Stat, int device_id);
    void EnableICONOutputChannelRequest(int channel); // приходит оповещение о запросе на включение
    void DisableICONOutputChannelRequest(int channel); // из формы айсипикона
signals:
    void EnableChannel(int channel, int device_id); // этот запрос отсылается в поток с помощью этого сигнала
    void DisableChannel(int channel, int device_id);
private:
    void UpdateICPCONViewers(int device_id);
    XRFDeviceInfo DevicesInfo[10];
    OutputsTab * outputsicontab;
    backgroundthread *pThread;
    QString AppPath;
    QStringList IPAddresses, DevicesNames, CalibPaths;
    bool IsErrors = false;
    bool ReadMainConfiguration();
    void SetMainConfiguration();
    Ui::MainWindow *ui;
    ElementList AnalyzerElementsList;
    QString CalibPath, ResultsPath, SopListPath;
};
#endif // MAINWINDOW_H



/*
 * void TagList::on_tagList_itemClicked(QListWidgetItem *item)
{
    if(item == nullptr)
        return;
    if(item->data(Qt::CheckStateRole) != Qt::Checked)
        item->setData(Qt::CheckStateRole, Qt::Checked);
    else
        item->setData(Qt::CheckStateRole, Qt::Unchecked);
}
*/
