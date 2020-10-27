#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHBoxLayout>

bool MainWindow::ReadMainConfiguration(){
    QList<QStringList> data;
    this->AppPath = QDir::currentPath();

    QFile file(AppPath +"//BasicConfig.csv");
    if(file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        forever {
            QString line = stream.readLine();
            if(line.isNull()) {break;}
            if(line.isEmpty()) {continue;}

            QStringList row;
            foreach(const QString& cell, line.split(",")) {
                row.append(cell.trimmed());
            }
            data.append(row);
            }

         IPAddresses = data[0];
         DevicesNames = data[1];
         CalibPaths = data[2];

         if(IPAddresses.size() == DevicesNames.size() && DevicesNames.size() == CalibPaths.size() && CalibPaths.size() == IPAddresses.size()){

             return  false;
         }
         else{
             QMessageBox::information(this, "Error","Bad structure of basic configuration file");
             return true;
         }
         file.close();
        }
    else{
        QMessageBox::information(this, "Error","Could not find Basic configuration");
        return true;
    }
    return false;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    IsErrors = ReadMainConfiguration();
    SetMainConfiguration();
    ui->ICPCONmanagetabWidget->clear();
    outputsicontab = new OutputsTab(this);
    ui->ICPCONmanagetabWidget->addTab(outputsicontab,tr("ICPCON"));



    if(!IsErrors){
       for(int i = 0; i < 8; ++i) // ICON ENABLE/DISABLE OUTPUT CLICKED NOTIFIER
       {
           connect(outputsicontab->PushEnable[i], SIGNAL(clicked_channel(int)),this, SLOT(EnableICONOutputChannelRequest(int)));
           connect(outputsicontab->PushDisable[i], SIGNAL(clicked_channel(int)),this, SLOT(DisableICONOutputChannelRequest(int)));
       }
       
       pThread  =  new backgroundthread(IPAddresses, DevicesInfo);
       pThread->OnboardStatusICON.resize(IPAddresses.size(), 0);
       pThread->OnboardStatusDP5.resize(IPAddresses.size(), 0);
       pThread->OnboardStatusMOXA.resize(IPAddresses.size(), 0);
       pThread->AcquireRequestReceived.resize(IPAddresses.size(), false);

       connect(pThread, SIGNAL(CommunicationError(int, int )), this, SLOT(ManageThreadCode(int, int)));
       connect(pThread, SIGNAL(DeviceStatus(int, int )), this, SLOT(UpdateDeviceStatus(int, int)));
       connect(pThread, SIGNAL(StringStatusReceived(QString, int )), this, SLOT(UpdateDP5StatusString(QString, int)));
       // ICPCON OUTPUTS CHANNEL MANAGEMENT SIGNALS ///
       connect(this, SIGNAL(EnableChannel(int, int )), pThread,SLOT(EnableICONChannelRequest(int, int )));
       connect(this, SIGNAL(DisableChannel(int, int )), pThread,SLOT(DisableICONChannelRequest(int, int )));
       // ICPCON OUTPUTS CHANNEL MANAGEMENT SIGNALS ///
       pThread->start();
    }
}

void MainWindow::SetMainConfiguration(){
    if(IsErrors){
        QMessageBox::information(this, "Error","Could not start up!");
        return;
    }
    else{
        foreach(const QString& name, DevicesNames) {
            QListWidgetItem *item= new QListWidgetItem(name, ui->IPAddressListWidget);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
            item->setCheckState(Qt::Checked);
            ui->IPAddressListWidget->addItem(item);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    pThread->Stop = true;
    pThread->GlobalStop = true;
    Sleep(2000);
    pThread->terminate();
    pThread->wait();
    delete pThread;
}


void MainWindow::ManageThreadCode(int code, int device_id){
    switch(code){
    case WSA_ERROR:
        QMessageBox::information(this,"Error", "WSA startup error");
        IsErrors = true;
        break;
    default:
        QMessageBox::information(this,"Error", "Undefined error occured");
        IsErrors = true;
        break;
    }

}

void MainWindow::UpdateDeviceStatus(int status, int device_id){
    switch(status){
    case MOXA_ONLINE:

        ui->IPAddressListWidget->item(device_id)->setBackground(Qt::green);
        if(device_id == ui->IPAddressListWidget->currentRow()){
            ui->MOXALBL->setStyleSheet("QLabel { background-color : cyan; color : red; }");
            ui->MOXALBL->setText("On");
        }

        //QMessageBox::information(this,"Moxa", "Moxa online" + IPAddresses[device_id]);
        break;
    case MOXA_OFFLINE:
         //pThread->OnboardStatusMOXA[device_id] = 0;
        ui->IPAddressListWidget->item(device_id)->setBackground(Qt::red);
        if(device_id == ui->IPAddressListWidget->currentRow()){
        ui->MOXALBL->setStyleSheet("QLabel { background-color : red; color : black; }");
        ui->MOXALBL->setText("Off");
        }
        //QMessageBox::information(this,"Moxa", "Moxa offline"+ IPAddresses[device_id]);
        break;
    case ICPCON_ONLINE:
        /*pThread->*///OnboardStatusICON[device_id] = 1;
        //QMessageBox::information(this,"ICPCON", "ICPCON online"+ IPAddresses[device_id]);
        if(device_id == ui->IPAddressListWidget->currentRow()){
        ui->ICONLBL->setStyleSheet("QLabel { background-color : cyan; color : red; }");
        ui->ICONLBL->setText("On");
        UpdateICPCONViewers(device_id);
        }

        break;
    case ICPCON_OFFLINE:
        /*pThread->*///OnboardStatusICON[device_id] = 0;
        //QMessageBox::information(this,"ICPCON", "ICPCON offline"+ IPAddresses[device_id]);
        if(device_id == ui->IPAddressListWidget->currentRow()){
        ui->ICONLBL->setStyleSheet("QLabel { background-color : red; color : black; }");
        ui->ICONLBL->setText("Off");
        }
        break;
    case DP5_ONLINE:
        if(device_id == ui->IPAddressListWidget->currentRow()){
        ui->DPPLBL->setStyleSheet("QLabel { background-color : cyan; color : red; }");
        ui->DPPLBL->setText("On");
        }
        break;
    case DP5_OFFLINE:
        if(device_id == ui->IPAddressListWidget->currentRow()){
        ui->DPPLBL->setStyleSheet("QLabel { background-color : red; color : black; }");
        ui->DPPLBL->setText("Off");
        }
        break;
    default:
        //QMessageBox::information(this,"xxx", "xxx");
        break;
    }
}

void MainWindow::UpdateDP5StatusString(QString Stat, int device_id){
    DevicesInfo[device_id].DppStatusString = Stat;
    if(device_id == ui->IPAddressListWidget->currentRow()){
       ui->CurrentDP5StatusLabel->setText(Stat);
    }
}


void MainWindow::UpdateICPCONViewers(int device_id){
    for(int i = 0; i < 8; ++i){
        if(DevicesInfo[device_id].ICPCONDevice.Inputs_State[i]){
            outputsicontab->ChannelsViewers[i]->setStyleSheet("background-color:  red;"
                                                                "border-style: outset;"
                                                                "border-width: 2px;"
                                                                "border-radius: 10px;"
                                                                "border-color: beige;"
                                                                "font: bold 14px;"
                                                                "padding: 6px;" );
        }
        else outputsicontab->ChannelsViewers[i]->setStyleSheet("background-color:  rgb(2, 252, 44);"
                                                               "border-style: outset;"
                                                               "border-width: 2px;"
                                                               "border-radius: 10px;"
                                                               "border-color: beige;"
                                                               "font: bold 14px;"
                                                               "padding: 6px;" );

    }
    for(int i = 0; i < 8; ++i){
        if(DevicesInfo[device_id].ICPCONDevice.Outputs_State[i]){
            outputsicontab->PushEnable[i]->setStyleSheet("background-color: red;"
                                                         "border-style: outset;"
                                                         "border-width: 2px;"
                                                         "border-radius: 10px;"
                                                         "border-color: beige;"
                                                         "font: bold 14px;"
                                                         "padding: 6px;" );
        }
        else{
            outputsicontab->PushEnable[i]->setStyleSheet("background-color: cyan;"
                                                         "border-style: outset;"
                                                         "border-width: 2px;"
                                                         "border-radius: 10px;"
                                                         "border-color: beige;"
                                                         "font: bold 14px;"
                                                         "padding: 6px;" );

        }
    }
}

void MainWindow::EnableICONOutputChannelRequest(int channel){
    int device_id = ui->IPAddressListWidget->currentRow();
    QMessageBox::information(this, " channel ", " Enable channel " + QString::number(channel));
    emit EnableChannel(channel, device_id);
}

void MainWindow::DisableICONOutputChannelRequest(int channel){
    int device_id = ui->IPAddressListWidget->currentRow();
    QMessageBox::information(this, " channel ", " Disable channel " + QString::number(channel));
    emit DisableChannel(channel, device_id);
}
