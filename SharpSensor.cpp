// SharpSensor.cpp
#include "SharpSensor.h"

SharpSensor::SharpSensor() {
    _pin = -1;
    _averageSamples = 1;
    _isCalibrated = false;
    _calibrationFactor = 1.0;
}

void SharpSensor::model(SharpModel sensorModel) {
    _model = sensorModel;
}

void SharpSensor::attach(int analogPin) {
    _pin = analogPin;
    pinMode(_pin, INPUT);
}

int SharpSensor::_readAverage() {
    if (_averageSamples <= 1) {
        return analogRead(_pin);
    }
    
    long sum = 0;
    for (int i = 0; i < _averageSamples; i++) {
        sum += analogRead(_pin);
        delay(2);
    }
    return sum / _averageSamples;
}

SharpSensor::SensorParams SharpSensor::_getParamsForModel(SharpModel model) {
    SensorParams params;
    
    switch(model) {
        case GP2Y0A02YK0F:
            params.minDistance = 20.0;
            params.maxDistance = 150.0;
            params.coeffA = 62.0;
            params.coeffB = 0.6;
            params.coeffC = 0.012;
            break;
            
        case GP2Y0A21YK0F:
            params.minDistance = 10.0;
            params.maxDistance = 80.0;
            params.coeffA = 27.0;
            params.coeffB = 0.42;
            params.coeffC = 0.005;
            break;
            
        case GP2Y0A41SK0F:
            params.minDistance = 4.0;
            params.maxDistance = 30.0;
            params.coeffA = 12.0;
            params.coeffB = 0.2;
            params.coeffC = 0.002;
            break;
            
        case GP2Y0A710K0F:
            params.minDistance = 100.0;
            params.maxDistance = 550.0;
            params.coeffA = 150.0;
            params.coeffB = 1.5;
            params.coeffC = 0.02;
            break;
    }
    
    return params;
}

float SharpSensor::_convertToDistance(int rawValue) {
    if (rawValue <= 0) return _getParamsForModel(_model).maxDistance;
    
    SensorParams params = _getParamsForModel(_model);
    float voltage = rawValue * (5.0 / 1023.0);
    float distanceCM = 0;
    
    if (_model == GP2Y0A41SK0F) {
        distanceCM = 1.0 / (voltage * params.coeffA - params.coeffB) - params.coeffC;
    } else {
        distanceCM = params.coeffA / (voltage - params.coeffB) - params.coeffC;
    }
    
    if (distanceCM < params.minDistance) distanceCM = params.minDistance;
    if (distanceCM > params.maxDistance) distanceCM = params.maxDistance;
    
    return distanceCM;
}

float SharpSensor::_applyCalibration(float distance) {
    return _isCalibrated ? distance * _calibrationFactor : distance;
}

float SharpSensor::_readDistanceCM() {
    if (_pin == -1) return -1;
    
    int rawValue = _readAverage();
    float distance = _convertToDistance(rawValue);
    return _applyCalibration(distance);
}

float SharpSensor::get(String unit) {
    float distanceCM = _readDistanceCM();
    
    if (unit == "mm") return distanceCM * 10.0;
    if (unit == "inch") return distanceCM / 2.54;
    if (unit == "raw") return _readAverage();
    
    return distanceCM;
}

float SharpSensor::getCM() { return get("cm"); }
float SharpSensor::getMM() { return get("mm"); }
float SharpSensor::getInch() { return get("inch"); }
float SharpSensor::getRaw() { return get("raw"); }

void SharpSensor::setAverage(int samples) {
    _averageSamples = max(1, samples);
}

void SharpSensor::calibrate(float knownDistance) {
    if (knownDistance <= 0) return;
    
    float measuredDistance = _readDistanceCM();
    if (measuredDistance > 0) {
        _calibrationFactor = knownDistance / measuredDistance;
        _isCalibrated = true;
    }
}