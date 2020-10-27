#include "icon_tabs.h"


ICPCONPushButton::ICPCONPushButton(int digit, QWidget *parent)
    : QPushButton(parent)
{
    myDigit = digit;
    setText(QString::number(myDigit));
    connect(this, SIGNAL(clicked()), this, SLOT(reemitClicked()));
}

void ICPCONPushButton::reemitClicked()
{
    emit clicked_channel(myDigit);
}


OutputsTab::OutputsTab(QWidget *parent) : QWidget(parent){

    horizontalLayout = new QHBoxLayout();
    for(int i = 0; i < 4; ++i)verticalontalLayout[i] = new QVBoxLayout();

    for(int i = 0; i < 8; ++i){
        lbls[i] = new QLabel();
        PushDisable[i] = new ICPCONPushButton(i, this);
        PushEnable[i] = new ICPCONPushButton(i, this);
        ChannelsViewers[i] = new QPushButton();
        this->ChannelsViewers[i]->setText("");
        this->ChannelsViewers[i]->setStyleSheet("background-color:  rgb(2, 252, 44);"
                                           "border-style: outset;"
                                           "border-width: 2px;"
                                           "border-radius: 10px;"
                                           "border-color: beige;"
                                           "font: bold 14px;"
                                           "padding: 6px;" );
        this->PushEnable[i]->setText("On");

        this->PushEnable[i]->setStyleSheet("background-color: cyan;"
                                           "border-style: outset;"
                                           "border-width: 2px;"
                                           "border-radius: 10px;"
                                           "border-color: beige;"
                                           "font: bold 14px;"
                                           "padding: 6px;" );
        this->PushDisable[i]->setText("Off");
        this->PushDisable[i]->setStyleSheet("background-color: grey;"
                                            "color : rgb(168, 255, 211);"
                                           "border-style: outset;"
                                           "border-width: 2px;"
                                           "border-radius: 10px;"
                                           "border-color: beige;"
                                           "font: bold 14px;"
                                           "padding: 6px;" );
        this->lbls[i]->setText("Channel : " + QString::number(i));
    }

    {

   }

    for(int i = 0; i < 8; ++i){
        verticalontalLayout[0]->addWidget(ChannelsViewers[i]);
        verticalontalLayout[1]->addWidget(PushEnable[i]);
        verticalontalLayout[2]->addWidget(PushDisable[i]);
        verticalontalLayout[3]->addWidget(lbls[i]);
    }

    horizontalLayout->addLayout(verticalontalLayout[0]);
    horizontalLayout->addLayout(verticalontalLayout[1]);
    horizontalLayout->addLayout(verticalontalLayout[2]);
    horizontalLayout->addLayout(verticalontalLayout[3]);
    this->setLayout(horizontalLayout);
}
