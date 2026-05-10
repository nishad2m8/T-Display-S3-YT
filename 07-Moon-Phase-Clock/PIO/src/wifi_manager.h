#pragma once

namespace WifiManager
{
    enum class State { Idle, Connecting, Connected };

    void  begin();
    void  update();
    State state();
    bool  isConnected();
}
