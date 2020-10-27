#ifndef ICON_TABS_H
#define ICON_TABS_H

#include <QObject>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ICPCONPushButton : public QPushButton
    {
        Q_OBJECT
    public:
        ICPCONPushButton(int digit, QWidget *parent);

    signals:
        void clicked_channel(int digit);

    private slots:
        void reemitClicked();

    private:
        int myDigit;
    };




class OutputsTab : public QWidget
{
    Q_OBJECT
    QLabel *lbls[8];
    ICPCONPushButton *PushEnable[8];
    ICPCONPushButton *PushDisable[8];
    QPushButton *ChannelsViewers[8];
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalontalLayout[4];

public:
    explicit OutputsTab(QWidget *parent = nullptr);
    friend class MainWindow;


 };


#endif // ICON_TABS_H
