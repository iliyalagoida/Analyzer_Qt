#ifndef ELEMENTS_H
#define ELEMENTS_H


#include <string>
#include <vector>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QMessageBox>
#include <QDir>
#include <memory>
#include <atomic>

class ElementList
{
private:
    struct Element{
        Element(std::string nm, int bg, int ch) : name(nm), begin_channel(bg), end_channel(ch) {};
        std::string name;
        int begin_channel;
        int end_channel;
        double intencity_clear, intencity_KeV, sum_intensity, multi_intensity, after_divide_intensity;
    };
    std::vector<Element *> ListOfElements;
    int CurrentSpectrum[1024];
    void UpdateSumIntensities();
    void UpdateMultiIntensities();
    void UpdateAfterDivideIntensities();
public:
    ElementList();
    void AddElement(std::string name="El", int bg_ch=0, int end_ch=0);
    void UpdateSpectrum(int *arr);
    void CalculateIntensities();
    void CalculateConcentrations();
    double GetCurrentConcentrationOfElement(int element);
    double GetCurrentElementIntensity(int element, int type); // 0 - clear, 1 - KeV, 2 - Sum, 3 - Multi, 4 - Divides
    bool ReadCalibrationAndSet(QString path);
};


#endif // ELEMENTS_H
