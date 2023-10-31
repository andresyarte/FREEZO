#include "TemperatureSensor.h"
#include <iostream>

TemperatureSensor::TemperatureSensor(float Tmin, float Tmax)
{
    this->Tmin = Tmin;
    this->Tmax = Tmax;

    samples.resize(NumSamples);    
    readTemp();
    IsTempValid();

}

float TemperatureSensor::readTemp()
{
    for (i = 0; i < NumSamples; i++) {
    samples[i] = analogRead(thermistorPin);
    delay(10);
    }

    // Promediar las muestras
    TemperatureSensor::average = 0;
    for (i = 0; i < NumSamples; i++) {
        average += samples[i];
    }
    average /= NumSamples;

    Serial.print("Average analog reading "); 
    Serial.println(average);
        
    // Convertir el valor análogo a una resistencia
    average = (4095 / average)  - 1;     // (4095 / ADC - 1). 4095 por tener un ADC de 12 bits (0-4095).
    average = SeriesResistor / average;  // 10K / (4095 / ADC - 1)
    Serial.print("Thermistor resistance "); 
    Serial.print(average);
    Serial.println(" ohms");
    
    steinhart = average / ThermistorNominal;        
    steinhart = log(steinhart);                     
    steinhart /= BCoefficient;                      
    steinhart += 1.0 / (NominalTemp + 273.15);      
    steinhart = 1.0 / steinhart;                    
    steinhart -= 273.15;                            
    
    Serial.print("Temperature "); 
    Serial.print(steinhart);
    Serial.println(" ºC");

    return steinhart;
    //return 0.0;  // Solo un valor de ejemplo
}

bool TemperatureSensor::IsTempValid()
{
    if (readTemp() > Tmin && readTemp() < Tmax) {
        return true;
    }
    else{
        return false;
    }
}