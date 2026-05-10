#pragma once

namespace WeatherClient
{
    void begin();
    void requestFetch();
    bool isFetching();
    bool hasNewResult();
    void applyResultToUI();
}
