#include "elements.h"

ElementList::ElementList(){

}

void ElementList::AddElement(std::string name, int bg_ch, int end_ch){
   ListOfElements.push_back(new Element(name, bg_ch, end_ch));
}

void ElementList::UpdateSpectrum(int *arr){
    for(int i = 0; i < 1024; ++i) CurrentSpectrum[i] = arr[i];
}

void ElementList::UpdateSumIntensities(){

}

void ElementList::UpdateMultiIntensities(){

}

void ElementList::UpdateAfterDivideIntensities(){

}

void ElementList::CalculateIntensities(){

}

void ElementList::CalculateConcentrations(){

}

bool ElementList::ReadCalibrationAndSet(QString path){

    return false;
}
