// SharpSensor.h
#ifndef SHARP_SENSOR_H
#define SHARP_SENSOR_H

#include "Arduino.h"

// Перечисление моделей датчиков
enum SharpModel {
    GP2Y0A02YK0F,  // 20-150 см
    GP2Y0A21YK0F,  // 10-80 см
    GP2Y0A41SK0F,  // 4-30 см
    GP2Y0A710K0F   // 100-550 см
};

class SharpSensor {
  public:
    // Конструктор
    SharpSensor();
    
    // Метод для указания модели датчика
    void model(SharpModel sensorModel);
    
    // Метод для подключения к аналоговому пину
    void attach(int analogPin);
    
    // Основной метод получения данных
    float get(String unit = "cm");
    
    // Альтернативные методы для удобства
    float getCM();
    float getMM();
    float getInch();
    float getRaw();
    
    // Метод для усреднения значений
    void setAverage(int samples);
    
    // Калибровочные методы
    void calibrate(float knownDistance);
    
  private:
    int _pin;
    SharpModel _model;
    int _averageSamples;
    bool _isCalibrated;
    float _calibrationFactor;
    
    // Вспомогательные методы
    float _readDistanceCM();
    float _convertToDistance(int rawValue);
    float _applyCalibration(float distance);
    int _readAverage();
    
    struct SensorParams {
        float minDistance;
        float maxDistance;
        float coeffA;
        float coeffB;
        float coeffC;
    };
    
    SensorParams _getParamsForModel(SharpModel model);
};

#endif